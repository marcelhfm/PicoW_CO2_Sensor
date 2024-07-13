#ifndef LOGGING_H
#define LOGGING_H

#define LOG_QUEUE_LENGTH 25
#define LOG_MESSAGE_MAX_LENGTH 256

typedef enum { L_INFO, L_WARN, L_ERROR, L_DEBUG } log_level_t;

void init_udp();

void udp_log_task(void *pvParameters);

void send_udp_message(char *message);

void u_log(log_level_t level, const char *tag, const char *format, ...);

#endif // !LOGGING_H
