#ifndef _LOG
#define _LOG

#define SYSLOG_MSG_BUF_SIZE 132                   //The maximum size +1 (null character) of syslog messages.

void log_msg(char *msg);

#endif
