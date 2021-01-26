#include <mgos.h>
#include <mgos_i2c.h>
#include <mgos_pwm.h>
#include <ads111x.h>
#include <mgos_syslog.h>
#include <mgos_system.h>

#include "mgos_time.h"
#include "timer.h"
#include "fan.h"
#include "log.h"

#define ADS111X_I2C_ADDRESS 72
#define PWM_PIN             12
#define PWM_FREQ            78000
#define VOLTAGE_CNV_FACTOR  158.46
#define MCP9700_CODES_PER_VOLT 16022.0
#define MCP9700_VOUT_0C        0.5
#define MCP9700_TC             0.01

#define CORRECTION_FACTOR 0.91621
#define ADS71205A_MV_PER_AMP 185*CORRECTION_FACTOR
#define ADS71220A_MV_PER_AMP 100*CORRECTION_FACTOR
#define ADS71230A_MV_PER_AMP 66*CORRECTION_FACTOR

#define NO_AMPS_MV 18.7
#define CURRENT_ADC_CODES_TO_MV 30.75
#define ADC_READING_COUNT 1

#define MEM_USAGE_TIMER_PERIOD_MS 1000
#define POLL_TEMP_TIMER_PERIOD_MS 1000
#define PID_LOOP_TIMER_PERIOD_MS 250

#define MAX_HEATSINK_TEMP 79.0

#define OFF_LOAD_FACTOR 0.15 //The PWM value at which the load is off
#define OFF_VOLTAGE 0.05     //If the voltage is lower than this the load is considered off

#define DEFAULT_PROPORTIANAL_COEFF 0.08
#define DEFAULT_INTEGRAL_COEFF 0.003
#define DEFAULT_DERIVATIVE_COEFF 0.005

static float kP = DEFAULT_PROPORTIANAL_COEFF;
static float kI = DEFAULT_INTEGRAL_COEFF;
static float kD = DEFAULT_DERIVATIVE_COEFF;


extern char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];

//Note The target value can either be the required amps or the required power, not both.
//     If target_watts is set then the target amps is set using the current voltage.
float target_amps = 0.0;
float target_watts = 0.0;
bool  temp_alarm = false;
float temp_now = 0.0;
float amps_now = 0.0;
float volts_now = 0.0;
float watts_now = 0.0;
float pwmValue = 0.0;


/**
 * @brief Callback to send periodic updates of the memory and file system state.
 **/
static void mem_usage_cb(void *arg) {

    size_t heap_zize          = mgos_get_heap_size();
    size_t free_heap_size     = mgos_get_free_heap_size();
    size_t min_free_heap_size = mgos_get_min_free_heap_size();
    size_t fs_size            = mgos_get_fs_size();
    size_t fs_free_size       = mgos_get_free_fs_size();
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "uptime=%.2lf, heap=%d, heap_free=%d heap_min=%d, total_disk_space=%d, free_disk_space=%d",  mgos_uptime(), heap_zize, free_heap_size, min_free_heap_size, fs_size, fs_free_size);
    log_msg(LL_DEBUG, syslog_msg_buf);
    (void) arg;
}

/**
 * @brief Read ADC value. Read the ADC a number of times and return the average value.
 * @param adc The ADC to read.
 * @param fs_voltage_id The ID of the max ADC voltage.
 * @param single_ended If 1 then read the ADC in single ended mode.
 * @return
 */
static uint16_t read_adc(uint8_t adc, uint8_t fs_voltage_id, bool single_ended) {
    //return get_adc_value(ADS111X_I2C_ADDRESS, adc, fs_voltage_id, SAMPLES_PER_SECOND_8, single_ended);

    uint32_t adc_value_accum = 0;
    uint8_t adc_value_index;

    for( adc_value_index=0 ; adc_value_index < ADC_READING_COUNT ; adc_value_index++ ) {
        adc_value_accum += (uint32_t)get_adc_value(ADS111X_I2C_ADDRESS, adc, fs_voltage_id, SAMPLES_PER_SECOND_32, single_ended);
    }

    return (uint16_t)(adc_value_accum/ADC_READING_COUNT);
}

/**
 * @brief Get the temp
 * @return the Temp in 0C
 */
static float get_temp(void) {
    uint16_t temp_adc = read_adc(ADC0, FS_VOLTAGE_2_048, 1);
//    uint16_t temp_adc = get_adc_value(ADS111X_I2C_ADDRESS, ADC0, FS_VOLTAGE_2_048, SAMPLES_PER_SECOND_8, 1);
    float mcp9700_volts = temp_adc / MCP9700_CODES_PER_VOLT;
    float tempC = ( mcp9700_volts - MCP9700_VOUT_0C ) / MCP9700_TC;

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "temp_adc=0x%04x, mcp9700_volts=%.3f, tempC=%.1f C, temp_alarm=%d", temp_adc, mcp9700_volts, tempC, temp_alarm);
    log_msg(LL_DEBUG, syslog_msg_buf);

    return tempC;
}

/**
 * @brief Get the voltage between the sense pins.
 * @return the Voltage in volts.
 */
static float get_voltage(void) {
    float volts_cal_value = mgos_sys_config_get_ydev_volts_cal_factor();

    uint16_t voltage_adc = read_adc(ADC3, FS_VOLTAGE_2_048, 0);
//    uint16_t voltage_adc = get_adc_value(ADS111X_I2C_ADDRESS, ADC3, FS_VOLTAGE_2_048, SAMPLES_PER_SECOND_8, 0);
    float    voltage = (voltage_adc/VOLTAGE_CNV_FACTOR) * volts_cal_value;
    if( voltage < 0.0 ) {
        voltage = 0.0;
    }

    return voltage;
}

/**
 * @brief Get the current.
 * @param off If 1 then power is off and no current will be drawn.
 * @return The current in amps.
 */
static float get_current(uint8_t off) {
    float amps_cal_value = mgos_sys_config_get_ydev_amps_cal_factor();
    uint16_t adc_value = read_adc(ADC1, FS_VOLTAGE_1_024, 1);
//    uint16_t adc_value = get_adc_value(ADS111X_I2C_ADDRESS, ADC1, FS_VOLTAGE_1_024, SAMPLES_PER_SECOND_8, 1);
    static float no_amps_mv =  NO_AMPS_MV;
    if( off ) {
        //Calibrate the offset voltage here
        no_amps_mv = adc_value / CURRENT_ADC_CODES_TO_MV;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "no_amps_mv=%.1f", no_amps_mv);
        mgos_syslog_log_info(__FUNCTION__, syslog_msg_buf);
    }

    float sensor_mv = adc_value / CURRENT_ADC_CODES_TO_MV;
    float measured_mv = sensor_mv - no_amps_mv;
    float amps = (measured_mv / ADS71220A_MV_PER_AMP) * amps_cal_value;
    //amps cannot go negative
    if( amps < 0.0 ) {
        amps = 0.0;
    }
/*
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "current_adc=0x%04x, sensor_mv=%.1f, measured_mv=%.1f, amps=%.3f", adc_value, sensor_mv, measured_mv, amps);
    log_msg(LL_INFO, syslog_msg_buf);
*/
    return amps;
}

/**
 * @brief Get the temp value.
 * @return temp in C
 */
float get_temp_alarm(void) {
    return temp_alarm;
}

/***
 * @brief Called periodically to read the temperature of the heat sink.
 */
static void temp_cb(void *arg) {
    float temp = get_temp();

    //Set the fans on/off as required
    set_cooling(temp);

    if( temp >= MAX_HEATSINK_TEMP ) {
        set_load(0.0);
        temp_alarm = true;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! MAX TEMP REACHED (%.1f/%.1f C). TURNED OFF LOAD !!!", temp, MAX_HEATSINK_TEMP );
        log_msg(LL_WARN, syslog_msg_buf);
    }
    else {
        temp_alarm = false;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Temp = %.1f (Max = %.1f C).", temp, MAX_HEATSINK_TEMP );
        log_msg(LL_DEBUG, syslog_msg_buf);
    }

    temp_now = temp;

}

/**
 * @brief Get the temp value.
 * @return temp in C
 */
float get_temp_C(void) {
    return temp_now;
}

static float get_pid_power(float t_amps, float amps)
{
    float error = 0.0;
    float integral = 0.0;
    float derivative = 0.0;
    static float prevError = 0.0;
    float power  = 1.0;

    if ( t_amps != amps ) {
        error = t_amps-amps;
        integral = integral + error;

        if( error == 0.0 || amps > t_amps ) {
            integral = 0.0;
        }

        if( error > 1.0 || error < -1.0 ) {
            integral = 0;
        }

        derivative = error-prevError;
        prevError = error;
        power = error*kP + integral*kI + derivative*kD;
    }

    return power;
}

/***
 * @brief Called periodically to read the voltage and current values.
 *        and set the load accordingly.
 */
static void pid_loop_cb(void *arg) {
//    int64_t start_t = mgos_uptime_micros();
    float volts = get_voltage();
    uint8_t load_off = 0;
    float errorFactor = 0.0;
    float mulFactor = 0.0;
    float newPWMFactor = 0;

//    int64_t v_read_t = mgos_uptime_micros();
    //If we have no volts the load must be off or
    //if the PWM setting is set to a value which turns the load off
//    if( volts < OFF_VOLTAGE || pwmValue < OFF_LOAD_FACTOR || target_amps <= 0.0 ) {
    if( volts < OFF_VOLTAGE || target_amps <= 0.0 ) {
        load_off=1;
    }

    float amps = get_current(load_off);
    float watts = amps * volts;
//    int64_t a_read_t = mgos_uptime_micros();

    //if the user has requested watts
    if( target_watts > 0 ) {
        target_amps = target_watts / volts;
    }
    errorFactor = get_pid_power(target_amps, amps);
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "amps=%.6f, errorFactor=%.6f", amps, errorFactor);
    log_msg(LL_INFO, syslog_msg_buf);

    if( load_off ) {
        mgos_pwm_set(PWM_PIN, PWM_FREQ, 0.0);
        target_amps = 0.0;
        pwmValue = 0.0;
    }
    //If error reduced to a max of 15%
//    else if( errorFactor > 0.005 || errorFactor < -0.005 ) {
        mulFactor = 1+errorFactor;
        newPWMFactor = pwmValue*mulFactor;

    //    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Took %d us to read the voltage, took %d us to read the current. Total %d us.",(int)(v_read_t-start_t), (int)(a_read_t-v_read_t), (int)(a_read_t-start_t) );
    //    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, ">>>>>>>>>>> errorFactor=%.6f, mulFactor=%.6f, pwmValue=%.6f, mulFactor=%f, amps=%.3f, target_amps=%.3f, volts=%.3f, load_off=%d", errorFactor, mulFactor, pwmValue, mulFactor, amps, target_amps, volts, load_off);


        mgos_pwm_set(PWM_PIN, PWM_FREQ, newPWMFactor);
        pwmValue = newPWMFactor;
//    }



    amps_now = amps;
    volts_now = volts;
    watts_now = watts;


    (void) arg;
}

/**
 * @brief Set the PID control loop coefficients.
 * @param p The proportional coefficient.
 * @param i The integral coefficient.
 * @param d The derivative coefficient.
 */
void set_pid_coeffs(float p, float i, float d) {
    kP = p;
    kI = i;
    kD = d;
}

/**
 * @brief Get the amps value.
 * @return amps
 */
float get_amps(void) {
    return amps_now;
}


/**
 * @brief Get the volts value.
 * @return volts
 */
float get_volts(void) {
    return volts_now;
}


/**
 * @brief Get the watts value.
 * @return watts
 */
float get_watts(void) {
    return watts_now;
}

/**
 * @brief Set the load value.
 * @param  factor A value between 0.0 and 1.0 to set the PWM load output pin.
 * @return void
 */
void set_load(float factor) {
    mgos_pwm_set(PWM_PIN, PWM_FREQ, factor);
    pwmValue = factor;
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "-----------> PWM LOAD VALUE: %.6f", factor);
    log_msg(LL_INFO, syslog_msg_buf);
}

/**
 * @brief Set the target amps value.
 * @return void
 */
void set_target_amps(float amps) {
    //If we set the required amps value reset any previous request for power.
    target_watts = 0.0;
    target_amps = amps;
    pwmValue = 0.15;

    //Set initial PWM level to start PDI loop
    if( amps >= 0.1) {
        pwmValue = 0.25;
    }
    else if( amps >= 0.5) {
        pwmValue = 0.3;
    }
    else if( amps >= 1.0) {
        pwmValue = 0.333;
    }
    else if( amps >= 1.5) {
        pwmValue = 0.349;
    }
    else if( amps >= 2.0) {
        pwmValue = 0.362;
    }

    set_load(pwmValue);
}

/**
 * @brief Get the target amps value.
 * @return the target amps.
 */
float get_target_amps(void) {
    return target_amps;
}

/**
 * @brief Set the target watts value.
 * @return void
 */
void set_target_watts(float watts) {
    target_watts = watts;
}

/**
 * @brief Get the target watts value.
 * @return the target watts.
 */
float get_target_watts(void) {
    return target_watts;
}

/***
 * @brief Init all timer functions.
 */
void init_timers(void) {

    mgos_set_timer(MEM_USAGE_TIMER_PERIOD_MS, MGOS_TIMER_REPEAT, mem_usage_cb, NULL);
    mgos_set_timer(POLL_TEMP_TIMER_PERIOD_MS, MGOS_TIMER_REPEAT, temp_cb,  NULL);
    mgos_set_timer(PID_LOOP_TIMER_PERIOD_MS,  MGOS_TIMER_REPEAT, pid_loop_cb,  NULL);

}
