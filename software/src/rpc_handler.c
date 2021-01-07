#include <mgos.h>
#include <mgos_rpc.h>
#include <mgos_syslog.h>

#include "ayt_tx_handler.h"
#include "timer.h"
#include "rpc_handler.h"


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
 * @brief Init all the RPC handlers.
 */
void rpc_init(void) {

        struct mg_rpc *con = mgos_rpc_get_global();

        mg_rpc_add_handler(con, "ydev.factorydefault", NULL, mgos_rpc_ydev_factorydefault, NULL);
        mg_rpc_add_handler(con, "ydev.update_syslog", NULL, mgos_rpc_ydev_update_syslog, NULL);
        mg_rpc_add_handler(con, "ydev.pwm", "{pwm: %f}", mgos_rpc_ydev_set_pwm, NULL);
        mg_rpc_add_handler(con, "ydev.target_amps", "{amps: %f}", mgos_rpc_ydev_set_amps, NULL);
        mg_rpc_add_handler(con, "ydev.target_power", "{watts: %f}", mgos_rpc_ydev_set_watts, NULL);

}
