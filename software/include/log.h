#ifndef _LOG
#define _LOG

#define SYSLOG_MSG_BUF_SIZE 256                   //The maximum size +1 (null character) of syslog messages.

void log_msg(uint8_t log_level, char *msg);
void log_mg_str(uint8_t log_level, struct mg_str *args);

#endif
