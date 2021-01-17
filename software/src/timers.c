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

#define SYSLOG_MSG_BUF_SIZE 132                   //The maximum size +1 (null character) of syslog messages.
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
#define ADC_READING_COUNT 3

#define MEM_USAGE_TIMER_PERIOD_MS 1000
#define POLL_TEMP_TIMER_PERIOD_MS 1000
#define PID_LOOP_TIMER_PERIOD_MS 1000

#define MAX_HEATSINK_TEMP 79.0

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
    log_msg(LL_INFO, syslog_msg_buf);
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
    log_msg(LL_INFO, syslog_msg_buf);

    return tempC;
}

/**
 * @brief Get the voltage between the sense pins.
 * @return the Voltage in volts.
 */
static float get_voltage(void) {
    uint16_t voltage_adc = read_adc(ADC3, FS_VOLTAGE_2_048, 0);
//    uint16_t voltage_adc = get_adc_value(ADS111X_I2C_ADDRESS, ADC3, FS_VOLTAGE_2_048, SAMPLES_PER_SECOND_8, 0);
    float    voltage = voltage_adc/VOLTAGE_CNV_FACTOR;
    if( voltage < 0.0 ) {
        voltage = 0.0;
    }

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "voltage_adc=0x%04x voltage=%.3f", voltage_adc, voltage);
    log_msg(LL_INFO, syslog_msg_buf);

    return voltage;
}

/**
 * @brief Get the current.
 * @param off If 1 then power is off and no current will be drawn.
 * @return The current in amps.
 */
static float get_current(uint8_t off) {
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
    float amps = measured_mv / ADS71220A_MV_PER_AMP;
    //amps cannot go negative
    if( amps < 0.0 ) {
        amps = 0.0;
    }

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "current_adc=0x%04x, sensor_mv=%.1f, measured_mv=%.1f, amps=%.3f", adc_value, sensor_mv, measured_mv, amps);
    log_msg(LL_INFO, syslog_msg_buf);

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
    update_fan_state(FAN1, temp);
    update_fan_state(FAN2, temp);
    update_fan_state(FAN3, temp);
    update_fan_state(FAN4, temp);
    update_fan_state(FAN5, temp);

    if( temp >= MAX_HEATSINK_TEMP ) {
        set_load(0.0);
        temp_alarm = true;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! MAX TEMP REACHED (%.1f/%.1f C). TURNED OFF LOAD !!!", temp, MAX_HEATSINK_TEMP );
        log_msg(LL_INFO, syslog_msg_buf);
    }
    else {
        temp_alarm = false;
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


/***
 * @brief Called periodically to read the voltage and current values.
 *        and set the load accordingly.
 */
static void pid_loop_cb(void *arg) {
    int64_t start_t = mgos_uptime_micros();
    float volts = get_voltage();
    uint8_t load_off = 0;

    int64_t v_read_t = mgos_uptime_micros();
    //If we have no volts the load must be off
    if( volts == 0 ) {
        load_off=1;
    }

    float amps = get_current(load_off);
    float watts = amps * volts;
    int64_t a_read_t = mgos_uptime_micros();

    //if the user has requested watts
    if( target_watts > 0 ) {
        target_amps = target_watts / volts;
    }

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "watts=%.3f, Took %d us to read the voltage, took %d us to read the current. Total %d us.", watts, (int)(v_read_t-start_t), (int)(a_read_t-v_read_t), (int)(a_read_t-start_t) );
    log_msg(LL_INFO, syslog_msg_buf);

    amps_now = amps;
    volts_now = volts;
    watts_now = watts;

    (void) arg;
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
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "PWM LOAD VALUE: %.3f", factor);
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
