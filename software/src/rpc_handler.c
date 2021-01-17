#include <mgos.h>
#include <mgos_rpc.h>
#include <mgos_syslog.h>

#include "wifi_setup.h"
#include "ayt_tx_handler.h"
#include "timer.h"
#include "rpc_handler.h"
#include "log.h"

extern char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];

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
                              "target_watts:%.1f"
                              "}"
                              ,
                              unit_name,
                              group_name,
                              syslog_enabled,
                              max_pp_count,
                              get_target_amps(),
                              get_target_watts()
                              );

    (void) cb_arg;
    (void) fi;
    (void) args;
}

#define SET_CONFIG_RPC_JSON_STRING "{dev_name:%Q,"\
"group_name:%Q,"\
"enable_syslog:%d,"\
"max_pp_count:%d}"

#define SET_CONFIG_JSON_SCANF_ARGS &dev_name,\
&group_name,\
&enable_syslog,\
&max_pp_count

/**
 * @brief useful for logging the text of the received messages.
 */
static void log_mg_str(struct mg_str *args) {

    char *buf = (char *) malloc(args->len+1);
    if( buf ) {
        memset(buf, 0, args->len+1);
        memcpy(buf, args->p, args->len);
        LOG(LL_INFO, ("RPC RX: %s", buf));
        free(buf);
    }
}

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

    log_mg_str(&args);

    json_scanf(args.p, args.len, ri->args_fmt, SET_CONFIG_JSON_SCANF_ARGS);

    mgos_sys_config_set_ydev_unit_name(dev_name);
    mgos_sys_config_set_ydev_group_name(group_name);
    mgos_sys_config_set_ydev_enable_syslog(enable_syslog);
    mgos_sys_config_set_ydev_max_pp_count(max_pp_count);
    saveConfig();

    mg_rpc_send_responsef(ri, NULL);

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

    mg_rpc_send_responsef(ri, "{"
                              "amps:%.001f,"
                              "volts:%.1f,"
                              "watts:%.1f,"
                              "temp_c:%.1f,"
                              "temp_alarm:%d"
                              "}"
                              ,
                              amps,
                              volts,
                              watts,
                              tempC,
                              temp_alarm
                              );

    (void) cb_arg;
    (void) fi;
    (void) args;
}

/*
 * @brief Init all the RPC handlers.
 */
void rpc_init(void) {

        struct mg_rpc *con = mgos_rpc_get_global();

        mg_rpc_add_handler(con, "factorydefault", NULL, mgos_rpc_ydev_factorydefault, NULL);
        mg_rpc_add_handler(con, "update_syslog", NULL, mgos_rpc_ydev_update_syslog, NULL);

        mg_rpc_add_handler(con, "get_config", NULL, mgos_rpc_get_config, NULL);
        mg_rpc_add_handler(con, "get_stats", NULL, mgos_rpc_get_stats, NULL);
        mg_rpc_add_handler(con, "set_config", SET_CONFIG_RPC_JSON_STRING, mgos_rpc_set_config, NULL);

        mg_rpc_add_handler(con, "pwm", "{pwm: %f}", mgos_rpc_ydev_set_pwm, NULL);
        mg_rpc_add_handler(con, "target_amps", "{amps: %f}", mgos_rpc_ydev_set_amps, NULL);
        mg_rpc_add_handler(con, "target_power", "{watts: %f}", mgos_rpc_ydev_set_watts, NULL);
        mg_rpc_add_handler(con, "debug", "{level: %d}", mgos_sys_set_debug_handler, NULL);

}
