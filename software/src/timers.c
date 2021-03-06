#include <mgos.h>
#include <mgos_i2c.h>
#include <mgos_pwm.h>
#include <ads111x.h>
#include <mgos_syslog.h>
#include <mgos_system.h>
#include <math.h>

#include "mgos_time.h"
#include "timer.h"
#include "fan.h"
#include "log.h"

#define ADS111X_I2C_ADDRESS 72
#define PWM_PIN             12
#define PWM_FREQ            78000
//#define VOLTAGE_CNV_FACTOR  225.38
#define VOLTAGE_CNV_FACTOR  140.25
#define MCP9700_CODES_PER_VOLT 16022.0
#define MCP9700_VOUT_0C        0.5
#define MCP9700_TC             0.01

#define CORRECTION_FACTOR 0.91621
#define CODES_PER_AMP 2878.0

#define NO_AMPS_MV 18.7
#define ADC_READING_COUNT 3

#define MEM_USAGE_TIMER_PERIOD_MS 1000
#define POLL_TEMP_TIMER_PERIOD_MS 1000
#define PID_LOOP_TIMER_PERIOD_MS 250

#define OFF_LOAD_FACTOR 0.15 //The PWM value at which the load is off
#define OFF_VOLTAGE 0.05     //If the voltage is lower than this the load is considered off

#define DEFAULT_PROPORTIONAL_COEFF 0.08
#define DEFAULT_INTEGRAL_COEFF 0.04
#define DEFAULT_DERIVATIVE_COEFF 0.000

#define MAX_VOLTAGE 100 //The max voltage that should be applied to the load.
#define MAX_AMPS 10.0

#define MIN_LOAD_CURRENT 0.01 //The minimum setable load current.
#define INITIAL_LOAD_PWM 0.25

#define BUZZER_GPIO 4 // The GPIO pin that the buzzer is connected to.
#define WARNING_TONE_PERIOD_MS 500

//If enabled this reports the memory usage on the serial output and via syslog
//#define REPORT_MEM_USAGE 1

//If enabled this sets the unit to a mode where the REST pwm call must be used to set the PWM load output directly.
//#define PWM_DEBUG 1

//If enabled this reports the temp device reads on the serial output and via syslog
//#define SHOW_TEMP_DEBUG 1

//If enabled this reports the load input voltage debug on the serial port and via syslog
//#define DEBUG_VOLTAGE_READ 1

//If enabled this reports pid loop debug info on the serial port and via syslog
#define PID_LOOP_DEBUG 1

//If enabled this reports attempts to set the load (from user) on the serial port and via syslog
#define SET_LOAD_DEBUG 1

static bool warningToneOn = false; //If true then the buzzer will emit a tone
static bool warningToneHigh = false; //If the warning tone is on and this is true then a high warning tone will be heard.
                                     //If the warning tone is on and this is false then a low warning tone will be heard.

static float kP = DEFAULT_PROPORTIONAL_COEFF;
static float kI = DEFAULT_INTEGRAL_COEFF;
static float kD = DEFAULT_DERIVATIVE_COEFF;

extern char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];

float target_amps                   = 0.0;
bool  target_amps_changed           = false;
float max_watts                     = 0.0;
bool  temp_alarm                    = false;
float temp_now                      = 0.0;
float amps_now                      = 0.0;
float volts_now                     = 0.0;
float watts_now                     = 0.0;
float pwmValue                      = 0.0;
float maxPWMValue                   = 1.0;

float min_load_voltage              = 0.0;
bool  min_load_voltage_alarm        = false;
bool  load_on                       = false;
bool  last_load_on_state            = false;
bool  max_watts_alarm               = true;
int64_t start_load_on_time_usecs    = 0.0; //The time at which the load was last switched on
int64_t stop_load_on_time_usecs     = 0.0; //The time at which the load was switched off
int64_t last_pid_loop_us            = 0;
float ampSeconds                    = 0.0;
float amp_microseconds              = 0.0;
float watt_microseconds             = 0.0;
float previous_amp_microseconds     = 0.0;
float previous_watt_microseconds    = 0.0;
bool  max_load_voltage_alarm        = false;

#define PWM_TABLE_SIZE 19
//Lookup table for relationship between amps and PWM value
//First column is amps, second column is PWM value
//These values were arrived at testing by setting the PWM value and measuring the current.
static float pwm_lut[PWM_TABLE_SIZE][2] = {
        {0.016, 0.002},
        {0.042, 0.003},
        {0.068, 0.004},
        {0.095, 0.005},
        {0.122, 0.006},
        {0.148, 0.007},
        {0.174, 0.008},
        {0.201, 0.009},
        {0.227, 0.010},
        {0.49,  0.020},
        {0.905, 0.035},
        {2.022, 0.077},
        {3.63,  0.141},
        {5.01,  0.201},
        {6.0,   0.247},
        {6.8,   0.292},
        {7.7,   0.341},
        {8.5,   0.39},
        {9.6,   0.457},
};

/**
 * @brief Get the load resistance index.
 * @param amps The required load current in amps.
 * @param high If 1 then take the value above >= the a in the pwm_lut table, else take the <= the pwm_lut in the table.
 * @return The pwm table index.
 */
static int get_amps_index(float amps, int high) {
    int index=0, s_index=0;

    if( high ) {
        //Search from bottom up to find first value >= amps
        for( index=0 ; index <PWM_TABLE_SIZE ; index++ ) {
            if ( pwm_lut[index][0] >= amps ) {
                s_index = index;
                break;
            }
        }
    }
    else  {
        //Search from top down to find first value <= amps
        for( index=PWM_TABLE_SIZE-1 ; index >= 0 ; index-- ) {
            if( pwm_lut[index][0] <= amps ) {
                s_index = index;
                break;
            }
        }
    }

    return s_index;
}

/**
 * @brief Determine the PWM value to give the required load current.
 *        This uses the pwm look up table and performs linear interpolation to determine the PWM value.
 * @param amps The required load current in amps.
 * @return the pwm value to set.
 */
static float get_pwm_value(float amps) {
    float x0,y0,x1,y1;
    int low_index = get_amps_index(amps, 0);
    int high_index = get_amps_index(amps, 1);

    //If we have the exact value
    if( low_index == high_index ) {
        return pwm_lut[low_index][1];
    }
    x0= pwm_lut[low_index][0];
    y0= pwm_lut[low_index][1];

    x1= pwm_lut[high_index][0];
    y1= pwm_lut[high_index][1];

    return y0 + ((y1-y0)/(x1-x0)) * (amps - x0);
}

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
 * @brief Turn the load off.
 * @return
 */
static void set_load_off() {
    set_load(0.0);
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Turned load off");
    log_msg(LL_INFO, syslog_msg_buf);
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
    float mcp9700_volts = temp_adc / MCP9700_CODES_PER_VOLT;
    float tempC = ( mcp9700_volts - MCP9700_VOUT_0C ) / MCP9700_TC;

#ifdef SHOW_TEMP_DEBUG
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "temp_adc=0x%04x, mcp9700_volts=%.3f, tempC=%.1f C, temp_alarm=%d", temp_adc, mcp9700_volts, tempC, temp_alarm);
    log_msg(LL_INFO, syslog_msg_buf);
#endif

    return tempC;
}

/**
 * @brief Get the voltage between the sense pins.
 * @return the Voltage in volts.
 */
static float get_voltage(void) {
    float volts_cal_value = mgos_sys_config_get_ydev_volts_cal_factor();

    uint16_t voltage_adc = read_adc(ADC3, FS_VOLTAGE_2_048, 0);
    //Protect from wrapping
    if( voltage_adc == 0xffff ) {
        voltage_adc=0;
    }
    float    voltage = (voltage_adc/VOLTAGE_CNV_FACTOR) * volts_cal_value;
    if( voltage < 0.0 ) {
        voltage = 0.0;
    }
    if( voltage >=  MAX_VOLTAGE ) {
        max_load_voltage_alarm = true;
        //Set alarm if voltage to high in order to prompt user to remove/reduce it.
        set_audio_alarm(true);
    }

#ifdef DEBUG_VOLTAGE_READ
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "voltage_adc=%04x, voltage = %.1f, max_load_voltage_alarm=%d", voltage_adc, voltage, max_load_voltage_alarm);
    log_msg(LL_INFO, syslog_msg_buf);
#endif

    return voltage;
}

/**
 * @brief Get the current.
 * @return The current in amps.
 */
static float get_current() {
    float amps_cal_value = mgos_sys_config_get_ydev_amps_cal_factor();
    uint16_t adc_value = read_adc(ADC1, FS_VOLTAGE_2_048, 1);
    static float no_amps_codes =  0;

    if( !load_on ) {
        //Calibrate the offset voltage here
        no_amps_codes = adc_value;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "no_amps_codes=%.1f", no_amps_codes);
        log_msg(LL_INFO, syslog_msg_buf);
    }

    float amps = ( (adc_value-no_amps_codes) / CODES_PER_AMP) * amps_cal_value;

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "adc_value=%04x, amps=%.3f", adc_value, amps);
    log_msg(LL_INFO, syslog_msg_buf);

    //amps cannot go negative
    if( amps < 0.0 ) {
        amps = 0.0;
    }

    return amps;
}

/**
 * @brief Get the temp value.
 * @return temp in C
 */
bool get_temp_alarm(void) {
    return temp_alarm;
}

/**
 * @brief Reset temp alarm.
 */
void reset_temp_alarm(void) {
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! RESET TEMP ALARM");
    log_msg(LL_WARN, syslog_msg_buf);
    temp_alarm = false;
    set_audio_alarm(false);
}

/**
 * @brief Get the min load voltage alarm state.
 * @return True if the alarm is set
 */
bool get_min_load_voltage_alarm(void) {
    return min_load_voltage_alarm;
}

/**
 * @brief Det the status of teh audio alarm.
 * @return true if the warning audio alarm is on.
 */
bool get_audio_alarm(void) {
    return warningToneOn;
}

/**
 * @brief Set the audio alarm on/off
 * @param on true if the alarm is on.
 */
void set_audio_alarm(bool on) {
    warningToneOn=on;
}

/**
 * @brief Reset min load voltage alarm.
 */
void reset_min_load_voltage_alarm(void) {
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! RESET MIN LOAD VOLTAGE ALARM");
    log_msg(LL_WARN, syslog_msg_buf);
    min_load_voltage_alarm = false;
    set_audio_alarm(false);
}

/**
 * @brief Get the max load voltage alarm state.
 * @return True if the alarm is set
 */
bool get_max_load_voltage_alarm(void) {
    return max_load_voltage_alarm;
}

/**
 * @brief Get the max watts alarm state.
 * @return True if the alarm is set
 */
bool get_max_watts_alarm(void) {
    return max_watts_alarm;
}

/**
 * @brief Reset max load voltage alarm.
 */
void reset_max_load_voltage_alarm(void) {
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! RESET MAX LOAD VOLTAGE ALARM");
    log_msg(LL_WARN, syslog_msg_buf);
    max_load_voltage_alarm = false;
    set_audio_alarm(false);
}

/***
 * @brief Called periodically to read the temperature of the heat sink.
 */
static void temp_cb(void *arg) {
    float temp = get_temp();

    //Set the fans on/off as required
    set_cooling(temp);

    if( temp >= MAX_HEATSINK_TEMP ) {
        set_load_off();
        temp_alarm = true;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!! MAX TEMP REACHED (%.1f/%.1f C). TURNED OFF LOAD !!!", temp, MAX_HEATSINK_TEMP );
        log_msg(LL_WARN, syslog_msg_buf);
        set_audio_alarm(true);
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

/**
 * @brief Get the pid loop power (error)
 * @param t_amps Target amps.
 * @param amps The amps value now.
 * @return The error factor.
 */
static float get_pid_power(float t_amps, float amps)
{
    float error = 0.0;
    float integral = 0.0;
    float derivative = 0.0;
    static float prevError = 0.0;
    float power  = 0.0;

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

#ifdef PID_LOOP_DEBUG
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "get_pid_power(): amps_t=%.6f, amps=%.6f, power=%.6f, kP=%.6f, kI=%.6f, kD=%.6f", t_amps, amps, power, kP, kI, kD);
    log_msg(LL_INFO, syslog_msg_buf);
#endif

    return power;
}

/**
 * @brief Get the amp hours value while the load is on.
 * @return The amp hours value.
 */
double get_amp_hours(void) {
    double amp_hours = amp_microseconds / 3.6E9;
    return amp_hours;
}

/**
 * @brief Get the watt hours value while the load is on.
 * @return The watt hours value.
 */
double get_watt_hours(void) {
    float watt_hours = watt_microseconds / 3.6E9;
    return watt_hours;
}

/**
 * @brief Get the amp hours value once the load has been switched off.
 * @return The amp hours value.
 */
double get_previous_amp_hours(void) {
    double amp_hours = previous_amp_microseconds / 3.6E9;
    return amp_hours;
}

/**
 * @return Return the number of seconds that the load has been on for.
 */
double get_load_on_secs(void) {
    int64_t elapsed_usecs = 0;
    if( load_on ) {
        int64_t usecs_now = mgos_uptime_micros();
        elapsed_usecs = usecs_now - start_load_on_time_usecs;
    }
    return elapsed_usecs/1E6;
}

/**
 * @return Return the number of seconds that the load was previously on for.
 */
double get_previous_load_on_secs(void) {
    double previous_load_on_secs = 0.0;
    //If the load has been through one on/off cycle
    if( stop_load_on_time_usecs > start_load_on_time_usecs ) {
        previous_load_on_secs = (stop_load_on_time_usecs-start_load_on_time_usecs)/1E6;
    }
    return previous_load_on_secs;
}

/**
 * @brief Get the watt hours value once the load has been switched off.
 * @return The watt hours value.
 */
double get_previous_watt_hours(void) {
    float watt_hours = previous_watt_microseconds / 3.6E9;
    return watt_hours;
}

/**
 * @brief Update the amps and watts usage
 * @param amps The amps beeing drawn now.
 * @param watts The watts being drawn now.
 * @return
 */
static void update_usage_stats(float amps, float watts) {
    int64_t us_now = mgos_uptime_micros();
    int64_t elapsed_us = us_now - last_pid_loop_us; //The elapsed time since the last call to update_usage_stats()

    amp_microseconds += elapsed_us * amps; //Add to the calc of amp microseconds
    watt_microseconds += elapsed_us * watts; //Add to the calc of watt microseconds

}

/**
 * @brief Update the load settings and stats while the load is on.
 */
static void update_load(float amps, float watts) {
    float pidPower = 0.0;
    float mulFactor = 0.0;
    float newPWMFactor = 0;

    if( load_on ) {
        if( pwmValue < maxPWMValue ) {
            pidPower = get_pid_power(target_amps, amps);
            mulFactor = 1+pidPower;
            newPWMFactor = pwmValue*mulFactor;
            set_load(newPWMFactor);
        }
        else {
            snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "!!!!!!! Reached maxPWMValue=%f/%f", pwmValue, maxPWMValue);
            log_msg(LL_INFO, syslog_msg_buf);
        }
    }
    else {
        set_load_off();
    }

    update_usage_stats(amps, watts);

}

/**
 * @brief Called when load is switch on to start recording load stats.
 */
static void start_recording_load_stats(void) {
    start_load_on_time_usecs = mgos_uptime_micros(); //Record the time that the load was switched on
    amp_microseconds   = 0.0;
    watt_microseconds  = 0.0;
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Load switch on");
    log_msg(LL_INFO, syslog_msg_buf);
}

/**
 * @brief Called when load is switch off to stop recording load stats.
 */
static void stop_recording_load_stats(float amps, float watts) {
    stop_load_on_time_usecs = mgos_uptime_micros(); //Record the time that the load was switched off
    update_usage_stats(amps, watts);
    //Save the last recorded usage stats now the power has gone off
    previous_amp_microseconds = amp_microseconds;
    previous_watt_microseconds = watt_microseconds;
    //Reset the usage stats now
    amp_microseconds   = 0.0;
    watt_microseconds  = 0.0;
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Load switch off");
    log_msg(LL_INFO, syslog_msg_buf);
}

/**
 * @brief Set load current. This is not accurate but gives a starting point for the PID loop.
 *                          There is an inaccuracy in the calculation but the main inaccuracy
 *                          may come from the load voltage dropping when the current is applied.
 *                          This maybe due to the src impedance or resistance of the wires.
 *                          connecting to the load.
 * @param loadA The load current in amps.
 * @return
 */
static void set_initial_load_current(double amps) {
    if( amps < MIN_LOAD_CURRENT ) {
        set_load_off();
    }
    else if( volts_now > 0 ) {
        float pwm = get_pwm_value(amps);
        set_load(pwm);
#ifdef SET_LOAD_DEBUG
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "set_initial_load_current(): amps = %.3f, volts = %.1f, pwmValue=%.3f", amps, volts_now, pwmValue);
        log_msg(LL_INFO, syslog_msg_buf);
#endif
    }
}

/***
 * @brief Called periodically to read the voltage and current values.
 *        and set the load accordingly.
 */
static void pid_loop_cb(void *arg) {
    float volts = get_voltage();
    float amps = 0.0;
    float watts = 0;

    min_load_voltage = mgos_sys_config_get_ydev_load_off_voltage();
    max_watts = mgos_sys_config_get_ydev_max_watts();

    //Limit the min volts
    if( volts < 0.03 ) {
        volts = 0.0;
    }

    //If we have no volts the load must be off or
    //or the PWM setting is set to a value which turns the load off
    //or the max load voltage has been reached
    if( volts < OFF_VOLTAGE || target_amps <= 0.0 || max_load_voltage_alarm) {
        load_on = false;
    }

    //If we have an input voltage and the target amps is set to a non zero value.
    if( volts >= OFF_VOLTAGE && volts < MAX_VOLTAGE && target_amps > 0.0 ) {
        load_on = true;
    }

    if( load_on && min_load_voltage > 0.0 ) {
        if( volts < min_load_voltage ) {
            load_on = false;
            snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Load turned off as voltage dropped to %.2f volts", min_load_voltage);
            log_msg(LL_INFO, syslog_msg_buf);
            min_load_voltage_alarm = true;
            set_audio_alarm(true);
        }
    }

    amps = get_current();
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "load_on=%d, volts = %.1f, amps=%.3f, pwmValue=%.4f, warningToneOn=%d", load_on, volts, amps, pwmValue, warningToneOn);
    log_msg(LL_INFO, syslog_msg_buf);

    watts = amps * volts;
    if( watts > max_watts ) {
        max_watts_alarm = true;
    }
    else {
        max_watts_alarm = false;
    }

    if( !load_on ) {
        set_load_off();
        target_amps = 0.0;
        pwmValue = 0.0;
    }

    //If the load on state has changed
    if( load_on != last_load_on_state ) {
        //If the load has just been turned on
        if( load_on ) {
            start_recording_load_stats();
            set_initial_load_current(target_amps);
        }
        //If the load has just been switched off
        else {
            stop_recording_load_stats(amps, watts);
        }
        target_amps_changed = false;
        last_load_on_state = load_on;
    }
    else if( load_on ) {
        if( target_amps_changed ) {
            set_initial_load_current(target_amps);
            target_amps_changed = false;
        }
        else {
            update_load(amps, watts);
        }
    }

    amps_now = amps;
    volts_now = volts;
    watts_now = watts;

    last_pid_loop_us = mgos_uptime_micros();

    (void) arg;
}

/**
 * @brief Set the warning tone.
 * @param on If true then turn on the tone.
 * @param freqHz The output freq in Hz.
 */
static void warningTone(bool on, int freqHz) {
    if( load_on && on ) {
        mgos_pwm_set(BUZZER_GPIO, freqHz, 0.5);
    }
    else {
        mgos_pwm_set(BUZZER_GPIO, freqHz, 0.0);
    }
}

/***
 * @brief Called when testing the control of the voltage and current measurement using the pwm REST method.
 */
static void TESTpid_loop_cb(void *arg) {
    if( pwmValue > 0.0 ) {
        load_on=true;
    }

    float volts = get_voltage();
    float amps = get_current();
    float watts = amps * volts;

    amps_now = amps;
    volts_now = volts;
    watts_now = watts;

    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "volts=%.1f, amps=%.3f, watts=%.3f, pwmValue=%.6f,", volts_now, amps_now, watts_now, pwmValue);
    log_msg(LL_INFO, syslog_msg_buf);

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
    if( factor < maxPWMValue ) {
        mgos_pwm_set(PWM_PIN, PWM_FREQ, factor);
        pwmValue = factor;
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "-----------> SET_PWM=%.6f", factor);
        log_msg(LL_INFO, syslog_msg_buf);
    }
    else {
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, ">>>>>>>>>>>> NOT SET PWM LOAD to: %.6f, left at %f", factor, pwmValue);
        log_msg(LL_INFO, syslog_msg_buf);
    }
}

/**
 * @brief Set the target amps value.
 * @return void
 */
void set_target_amps(float amps) {
    float load_watts = volts_now * amps;
    //Don't allow the RPC interface to set higher than the max amps or watts.
    if( amps <= MAX_AMPS && load_watts <= max_watts ) {
        target_amps = amps;
        target_amps_changed = true;
    }
}

/**
 * @brief Get the target amps value.
 * @return the target amps.
 */
float get_target_amps(void) {
    return target_amps;
}

/**
 * @brief Set the max PWM value. This limits the max current.
 * @return void
 */
void set_max_pwm(float pwm) {
    maxPWMValue = pwm;
}

/**
 * @brief Called periodically to set the warning tone on/off. If on then the tone will toggle between two
 * frequencies.
 */
static void warning_tone_cb(void *arg) {
    int freqHz=500;
    if( warningToneOn ) {
        if( warningToneHigh ) {
            freqHz=1000;
            warningToneHigh=false;
        }
        else {
            warningToneHigh=true;
        }
    }
    warningTone(warningToneOn, freqHz);
}

/***
 * @brief Init all timer functions.
 */
void init_timers(void) {
    last_pid_loop_us = mgos_uptime_micros();
    max_watts = mgos_sys_config_get_ydev_max_watts();

#ifdef REPORT_MEM_USAGE
    mgos_set_timer(MEM_USAGE_TIMER_PERIOD_MS, MGOS_TIMER_REPEAT, mem_usage_cb, NULL);
#endif

    mgos_set_timer(POLL_TEMP_TIMER_PERIOD_MS, MGOS_TIMER_REPEAT, temp_cb,  NULL);

#ifdef PWM_DEBUG
    // This is only used when debugging and setting the PWM directly using the REST pwm command.
    mgos_set_timer(PID_LOOP_TIMER_PERIOD_MS,  MGOS_TIMER_REPEAT, TESTpid_loop_cb,  NULL);
#else
    mgos_set_timer(PID_LOOP_TIMER_PERIOD_MS,  MGOS_TIMER_REPEAT, pid_loop_cb,  NULL);
#endif

    mgos_set_timer(WARNING_TONE_PERIOD_MS,  MGOS_TIMER_REPEAT, warning_tone_cb,  NULL);
}
