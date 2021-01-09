#include <mgos.h>
#include <mgos_syslog.h>

#include "log.h"

char syslog_msg_buf[SYSLOG_MSG_BUF_SIZE];  //The buffer to load syslog messages into ready for transmission.

/**
 * @brief Send a log message to the serial port and syslog (if enabled).
 * @param log_level The log level as defined in cs_log_level.
 * @param msg The log message.
 * @return void
 */
void log_msg(uint8_t log_level, char *msg) {
    LOG(log_level, (msg) );
    if( mgos_sys_config_get_ydev_enable_syslog() ) {
      if( log_level == LL_ERROR ) {
          mgos_syslog_log_error(__FUNCTION__, syslog_msg_buf);
      }
      else if( log_level == LL_WARN ) {
          mgos_syslog_log_warn(__FUNCTION__, syslog_msg_buf);
      }
      else if( log_level == LL_INFO ) {
          mgos_syslog_log_info(__FUNCTION__, syslog_msg_buf);
      }
      else if( log_level == LL_DEBUG ) {
          mgos_syslog_log_debug(__FUNCTION__, syslog_msg_buf);
      }
      //This mapping is not intuative but probably won't be used often.
      else if( log_level == LL_VERBOSE_DEBUG ) {
          mgos_syslog_log_notice(__FUNCTION__, syslog_msg_buf);
      }
    }
}
