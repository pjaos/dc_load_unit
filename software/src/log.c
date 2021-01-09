#include <mgos.h>
#include <mgos_syslog.h>

#include "log.h"

char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];  //The buffer to load syslog messages into ready for transmission.

/**
 * @brief Send a log message to the serial port and syslog (if enabled)
 */
void log_msg(char *msg) {
    LOG(LL_INFO, (msg) );
    if( mgos_sys_config_get_ydev_enable_syslog() ) {
      mgos_syslog_log_info(__FUNCTION__, syslog_msg_buf);
    }
}
