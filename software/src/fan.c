#include <mgos.h>
#include <mgos_gpio.h>

#include "log.h"
#include "fan.h"

#define FAN1_GPIO 27
#define FAN2_GPIO 26
#define FAN3_GPIO 25
#define FAN4_GPIO 33
#define FAN5_GPIO 32

#define FAN1_THRESHOLD_TEMP 60
#define FAN2_THRESHOLD_TEMP 40
#define FAN3_THRESHOLD_TEMP 30
#define FAN4_THRESHOLD_TEMP 35
#define FAN5_THRESHOLD_TEMP 50
#define FAN_TEMP_HYSTERESIS  1

extern char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];

/**
 * @brief Set the fan on/off state.
 * @param fan ID The ID of the fan to be set.
 * @return void.
 */
void set_fan_state(uint8_t fan_id, bool on) {
    if( fan_id == FAN1 ) {
        mgos_gpio_write(FAN1_GPIO, on);
    }
    else if( fan_id == FAN2 ) {
        mgos_gpio_write(FAN2_GPIO, on);
    }
    else if( fan_id == FAN3 ) {
        mgos_gpio_write(FAN3_GPIO, on);
    }
    else if( fan_id == FAN4 ) {
        mgos_gpio_write(FAN4_GPIO, on);
    }
    else if( fan_id == FAN5 ) {
        mgos_gpio_write(FAN5_GPIO, on);
    }
    snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "%s: FAN: %d, state: %d", __FUNCTION__, fan_id, on);
    log_msg(LL_INFO, syslog_msg_buf);

}

/**
 * @brief Set a fan on/off.
 * @param fan_id The fan identifier number.
 * @param temp The heat sink temperature.
 * @return void.
 */
void update_fan_state(uint8_t fan_id, float temp) {
    float threshold_temp = 0.0;

    if( fan_id == FAN1 ) {
        threshold_temp = FAN1_THRESHOLD_TEMP;
    }
    else if( fan_id == FAN2 ) {
        threshold_temp = FAN2_THRESHOLD_TEMP;
    }
    else if( fan_id == FAN3 ) {
        threshold_temp = FAN3_THRESHOLD_TEMP;
    }
    else if( fan_id == FAN4 ) {
        threshold_temp = FAN4_THRESHOLD_TEMP;
    }
    else if( fan_id == FAN5 ) {
        threshold_temp = FAN5_THRESHOLD_TEMP;
    }

    //Turn on the fan if required
    if( temp >=  threshold_temp ) {
        set_fan_state(fan_id, true);
    }

    //Turn fan off if required
    if( temp <= threshold_temp-FAN_TEMP_HYSTERESIS ) {
        set_fan_state(fan_id, false);
    }
}

/**
 * @Init all the fan outputs and turn off fans.
 */
void init_fans(void) {
    mgos_gpio_set_mode(FAN1_GPIO, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_set_mode(FAN2_GPIO, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_set_mode(FAN3_GPIO, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_set_mode(FAN4_GPIO, MGOS_GPIO_MODE_OUTPUT);
    mgos_gpio_set_mode(FAN5_GPIO, MGOS_GPIO_MODE_OUTPUT);

    mgos_gpio_write(FAN1_GPIO, false);
    mgos_gpio_write(FAN2_GPIO, false);
    mgos_gpio_write(FAN3_GPIO, false);
    mgos_gpio_write(FAN4_GPIO, false);
    mgos_gpio_write(FAN5_GPIO, false);

}
