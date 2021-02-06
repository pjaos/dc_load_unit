#include <mgos.h>
#include <mgos_rpc.h>
#include <mgos_syslog.h>

#include "wifi_setup.h"
#include "ayt_tx_handler.h"
#include "timer.h"
#include "rpc_handler.h"
#include "log.h"
#include "fan.h"

extern char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];

/*
 * @brief Callback handler to reboot the unit.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_reboot(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
        LOG(LL_INFO, ("Reboot unit.") );
        mg_rpc_send_responsef(ri, NULL);
        LOG(LL_INFO, ("Reboot now.") );
        mgos_system_restart_after(250);

        (void) ri;
        (void) cb_arg;
        (void) fi;
        (void) args;
}

/*
 * @brief Callback handler to set the device to factory defaults. This will
 *        cause the device to reboot.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_factorydefault(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
        LOG(LL_INFO, ("Reset to factory default configuration and reboot.") );
        mgos_config_reset(MGOS_CONFIG_LEVEL_DEFAULTS);
        mg_rpc_send_responsef(ri, NULL);
        LOG(LL_INFO, ("Reboot now.") );
        mgos_system_restart_after(250);

        (void) ri;
        (void) cb_arg;
        (void) fi;
        (void) args;
}

/*
 * @brief Callback handler to set update the syslog state (enabled/disabled).
 *        If enabled syslog data is sent to the ICONS_GW host address.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_update_syslog(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
        LOG(LL_INFO, ("mgos_rpc_ydev_update_syslog()") );
        char *icons_gw_ip_addr = get_icons_gw_ip_address();

        //If syslog is enabled and we have the ICONS GW IP address setup syslog
        if( mgos_sys_config_get_ydev_enable_syslog() && icons_gw_ip_addr ) {
          char *hostname = (char *)mgos_sys_config_get_ydev_unit_name();
          reinit_syslog(icons_gw_ip_addr, hostname);
        }
        //disable syslog.
        else {
          reinit_syslog("", "");
        }

        mg_rpc_send_responsef(ri, NULL);

        (void) ri;
        (void) cb_arg;
        (void) fi;
        (void) args;
}

/*
 * @brief Callback handler to set set the PWM output value.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_set_pwm(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {

    float pwm;

    if (json_scanf(args.p, args.len, ri->args_fmt, &pwm) == 1) {
      mg_rpc_send_responsef(ri, "%f", pwm);
      set_load(pwm);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"pwm\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the target amps value.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_set_amps(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {

    float amps;

    if (json_scanf(args.p, args.len, ri->args_fmt, &amps) == 1) {
      mg_rpc_send_responsef(ri, "%f", amps);
      set_target_amps(amps);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"amps\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the target watts value.
 *        This is not used at the moment as the amps value can vary as the voltage changes.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_set_watts(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {

    float watts;

    if (json_scanf(args.p, args.len, ri->args_fmt, &watts) == 1) {
      mg_rpc_send_responsef(ri, "%f", watts);
      set_target_watts(watts);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"watts\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the debug level.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_sys_set_debug_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    int level = _LL_MIN;
    if (json_scanf(args.p, args.len, ri->args_fmt, &level) == 1) {
        if (level > _LL_MIN && level < _LL_MAX) {
            mg_rpc_send_responsef(ri, "%d", level);
            snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "level=%d", level);
            log_msg(LL_INFO, syslog_msg_buf);
            cs_log_set_level((enum cs_log_level) level);
        }
        else {
            mg_rpc_send_errorf(ri, -1, "Invalid debug level (-1 to 4 are valid).");
        }
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"level\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the load off voltage.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_set_load_off_voltage(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    float load_off_voltage = 0.0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &load_off_voltage) == 1) {
        mg_rpc_send_responsef(ri, "%f", load_off_voltage);
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Load off voltage =%f volts", load_off_voltage);
        log_msg(LL_INFO, syslog_msg_buf);
        mgos_sys_config_set_ydev_load_off_voltage(load_off_voltage);
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"set_load_off_voltage\":volts}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to get the config from the device.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_get_config(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {
    char *unit_name = (char *)mgos_sys_config_get_ydev_unit_name();
    char *group_name = (char *)mgos_sys_config_get_ydev_group_name();
    uint8_t syslog_enabled = mgos_sys_config_get_ydev_enable_syslog();
    uint32_t max_pp_count = mgos_sys_config_get_ydev_max_pp_count();
    float load_off_voltage = mgos_sys_config_get_ydev_load_off_voltage();

    if( unit_name == NULL ) {
        unit_name = "";
    }

    if( group_name == NULL ) {
        group_name = "";
    }

    mg_rpc_send_responsef(ri, "{"
                              "unit_name:%Q,"
                              "group_name:%Q,"
                              "syslog_enabled:%d,"
                              "max_pp_count:%d,"
                              "target_amps:%.001f,"
                              "target_watts:%.1f,"
                              "load_off_voltage:%f"
                              "}"
                              ,
                              unit_name,
                              group_name,
                              syslog_enabled,
                              max_pp_count,
                              get_target_amps(),
                              get_target_watts(),
                              load_off_voltage
                              );

    (void) cb_arg;
    (void) fi;
    (void) args;
}

#define SET_CONFIG_RPC_JSON_STRING "{dev_name:%Q,"\
"group_name:%Q,"\
"enable_syslog:%d,"\
"max_pp_count:%d,"\
"load_off_voltage:%f}"

#define SET_CONFIG_JSON_SCANF_ARGS &dev_name,\
&group_name,\
&enable_syslog,\
&max_pp_count,\
&load_off_voltage

/*
 * @brief Callback handler to allow the user parameters be set from the web page.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_set_config(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {
    char *dev_name=NULL;
    char *group_name=NULL;
    int  enable_syslog=0;
    int  max_pp_count=0;
    float load_off_voltage=0.0;
    log_mg_str(LL_INFO, &args);

    if(  json_scanf(args.p, args.len, ri->args_fmt, SET_CONFIG_JSON_SCANF_ARGS) == 5 ) {
        mg_rpc_send_responsef(ri, "%s, %s, %d, %d, %f",dev_name, group_name, enable_syslog, max_pp_count, load_off_voltage);
        mgos_sys_config_set_ydev_unit_name(dev_name);
        mgos_sys_config_set_ydev_group_name(group_name);
        mgos_sys_config_set_ydev_enable_syslog(enable_syslog);
        mgos_sys_config_set_ydev_max_pp_count(max_pp_count);
        mgos_sys_config_set_ydev_load_off_voltage(load_off_voltage);
        saveConfig();
    }

    if (dev_name) {
        free(dev_name);
    }

    if (group_name) {
        free(group_name);
    }

    (void)cb_arg;
    (void)fi;
    (void)args;
}

/*
 * @brief Callback handler to get the stats from the device.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_get_stats(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {
    float tempC = get_temp_C();
    float amps = get_amps();
    float volts = get_volts();
    float watts = get_watts();
    int temp_alarm = get_temp_alarm();
    int fan_on_count = get_fan_on_count();
    double amp_hours = get_amp_hours();
    double watt_hours = get_watt_hours();
    double previous_amp_hours = get_previous_amp_hours();
    double previous_watt_hours = get_previous_watt_hours();
    double load_on_secs = get_load_on_secs();
    double previous_load_on_secs = get_previous_load_on_secs();
    int min_load_voltage_alarm = get_min_load_voltage_alarm();
    int max_load_voltage_alarm = get_max_load_voltage_alarm();

    mg_rpc_send_responsef(ri, "{"
                              "amps:%f,"
                              "volts:%f,"
                              "watts:%f,"
                              "temp_c:%f,"
                              "temp_alarm:%d,"
                              "fan_on_count:%d,"
                              "amp_hours:%f,"
                              "watt_hours:%f,"
                              "previous_amp_hours:%f,"
                              "previous_watt_hours:%f,"
                              "load_on_secs:%f,"
                              "previous_load_on_secs:%f,"
                              "min_load_voltage_alarm:%d,"
                              "max_load_voltage_alarm:%d"
                              "}"
                              ,
                              amps,
                              volts,
                              watts,
                              tempC,
                              temp_alarm,
                              fan_on_count,
                              amp_hours,
                              watt_hours,
                              previous_amp_hours,
                              previous_watt_hours,
                              load_on_secs,
                              previous_load_on_secs,
                              min_load_voltage_alarm,
                              max_load_voltage_alarm
                              );

    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the target P,I & D pid values value.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_ydev_set_pid_coeffs(struct mg_rpc_request_info *ri,
                                         void *cb_arg,
                                         struct mg_rpc_frame_info *fi,
                                         struct mg_str args) {
    float p=0.0,i=0.0,d=0.0;

//    if (json_scanf(args.p, args.len, ri->args_fmt, &p, &i, &d) == 1) {
    if (json_scanf(args.p, args.len, ri->args_fmt, &p, &i, &d) == 3) {
        mg_rpc_send_responsef(ri, "{P:%lf,I:%lf,D:%lf}", p, i, d);
        snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "PID COEFFS: P:%f I:%f D:%f", p, i, d);
        log_msg(LL_INFO, syslog_msg_buf);
        set_pid_coeffs(p, i, d);
    } else {
        mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"p\":value,\"i\":value,\"i\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the current calibration value.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_sys_set_current_cal_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    float cal_value = 1.0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &cal_value) == 1) {
            mg_rpc_send_responsef(ri, "%f", cal_value);
            snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Current cal value=%f", cal_value);
            log_msg(LL_INFO, syslog_msg_buf);
            mgos_sys_config_set_ydev_amps_cal_factor(cal_value);
            saveConfig();
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"cal\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to set set the voltage calibration value.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_sys_set_voltage_cal_handler(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    float cal_value = 1.0;
    if (json_scanf(args.p, args.len, ri->args_fmt, &cal_value) == 1) {
            mg_rpc_send_responsef(ri, "%f", cal_value);
            snprintf(syslog_msg_buf, SYSLOG_MSG_BUF_SIZE, "Voltage cal value=%f", cal_value);
            log_msg(LL_INFO, syslog_msg_buf);
            mgos_sys_config_set_ydev_volts_cal_factor(cal_value);
            saveConfig();
    } else {
      mg_rpc_send_errorf(ri, -1, "Bad request. Expected: {\"cal\":value}");
    }

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to reset a temp alarm.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_reset_temp_alarm(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    if ( get_temp_alarm() ) {
        reset_temp_alarm();
    }

    mg_rpc_send_responsef(ri, NULL);

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to reset a min load voltage alarm.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_reset_min_load_voltage_alarm(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    if ( get_min_load_voltage_alarm() ) {
        reset_min_load_voltage_alarm();
    }

    mg_rpc_send_responsef(ri, NULL);

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Callback handler to reset a max load voltage alarm.
 * @param ri
 * @param cb_arg
 * @param fi
 * @param args
 */
static void mgos_rpc_reset_max_load_voltage_alarm(struct mg_rpc_request_info *ri,
                                       void *cb_arg,
                                       struct mg_rpc_frame_info *fi,
                                       struct mg_str args) {

    if ( get_max_load_voltage_alarm() ) {
        reset_max_load_voltage_alarm();
    }

    mg_rpc_send_responsef(ri, NULL);

    (void) ri;
    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Init all the RPC handlers.
 */
void rpc_init(void) {

        struct mg_rpc *con = mgos_rpc_get_global();

        mg_rpc_add_handler(con, "reboot", NULL, mgos_rpc_ydev_reboot, NULL);
        mg_rpc_add_handler(con, "factorydefault", NULL, mgos_rpc_ydev_factorydefault, NULL);
        mg_rpc_add_handler(con, "update_syslog", NULL, mgos_rpc_ydev_update_syslog, NULL);
        mg_rpc_add_handler(con, "get_config", NULL, mgos_rpc_get_config, NULL);
        mg_rpc_add_handler(con, "get_stats", NULL, mgos_rpc_get_stats, NULL);
        mg_rpc_add_handler(con, "set_config", SET_CONFIG_RPC_JSON_STRING, mgos_rpc_set_config, NULL);
        mg_rpc_add_handler(con, "pwm", "{pwm: %f}",             mgos_rpc_ydev_set_pwm, NULL);
        mg_rpc_add_handler(con, "target_amps", "{amps: %f}",    mgos_rpc_ydev_set_amps, NULL);
        mg_rpc_add_handler(con, "target_power", "{watts: %f}",  mgos_rpc_ydev_set_watts, NULL);
        mg_rpc_add_handler(con, "debug", "{level: %d}",         mgos_sys_set_debug_handler, NULL);
        mg_rpc_add_handler(con, "pid_coeffs", "{P:%f,I:%f,D:%f}",      mgos_rpc_ydev_set_pid_coeffs, NULL);
        mg_rpc_add_handler(con, "set_current_cal", "{cal: %f}",         mgos_sys_set_current_cal_handler, NULL);
        mg_rpc_add_handler(con, "set_voltage_cal", "{cal: %f}",           mgos_sys_set_voltage_cal_handler, NULL);
        mg_rpc_add_handler(con, "set_load_off_voltage", "{volts: %f}", mgos_rpc_set_load_off_voltage, NULL);
        mg_rpc_add_handler(con, "reset_temp_alarm", NULL, mgos_rpc_reset_temp_alarm, NULL);
        mg_rpc_add_handler(con, "reset_min_load_voltage_alarm", NULL, mgos_rpc_reset_min_load_voltage_alarm, NULL);
        mg_rpc_add_handler(con, "reset_max_load_voltage_alarm", NULL, mgos_rpc_reset_max_load_voltage_alarm, NULL);

}
