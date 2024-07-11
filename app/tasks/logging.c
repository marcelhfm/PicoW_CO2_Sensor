#include "./logging.h"
#include "../main.h"
#include "./logging.h"
#include "FreeRTOS.h"
#include "lwip/ip4_addr.h"
#include "lwip/ip_addr.h"
#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "pico/cyw43_arch.h"
#include "portmacro.h"
#include "projdefs.h"
#include "queue.h"
#include "task.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define UDP_IP "192.168.11.30"
#define UDP_PORT 12345

const char *U_TAG = "[UDP]";
extern QueueHandle_t log_queue;

struct udp_pcb *upcb;

void u_log(log_level_t level, const char *tag, const char *format, ...) {
  char log_message[LOG_MESSAGE_MAX_LENGTH];
  va_list args;
  const char *color_code;

  // Determine the color code based on log level
  switch (level) {
  case L_INFO:
    color_code = "\033[0;32m"; // Green
    break;
  case L_WARN:
    color_code = "\033[0;33m"; // Yellow
    break;
  case L_ERROR:
    color_code = "\033[0;31m"; // Red
    break;
  case L_DEBUG:
    color_code = "\033[0;34m"; // Blue
    break;
  default:
    color_code = "\033[0m"; // Reset
    break;
  }

  // Create the log message
  snprintf(log_message, LOG_MESSAGE_MAX_LENGTH, "%s[%s] ", color_code, tag);
  va_start(args, format);
  vsnprintf(log_message + strlen(log_message),
            LOG_MESSAGE_MAX_LENGTH - strlen(log_message), format, args);
  va_end(args);
  snprintf(log_message + strlen(log_message),
           LOG_MESSAGE_MAX_LENGTH - strlen(log_message),
           "\033[0m"); // Reset color

  DEBUG_LOG("%s", log_message);

  // Send the message to the queue
  if (xQueueSendToBack(log_queue, &log_message, portMAX_DELAY) != pdPASS) {
    DEBUG_LOG("%s Failed to send log to queue\n", U_TAG);
  }
}

void udp_log_task(void *pvParameters) {
  DEBUG_LOG("%s Hello from udp_log_task\n", U_TAG);
  ip_addr_t destAddr;
  if (ip4addr_aton(UDP_IP, &destAddr) == 0) {
    DEBUG_LOG("%s Invalid destination IP address.\n", U_TAG);
    return;
  }

  char log_message[LOG_MESSAGE_MAX_LENGTH];

  while (1) {
    if (xQueueReceive(log_queue, &log_message, portMAX_DELAY) == pdPASS) {
      const char *prefix = "1;";
      int original_len = strlen(log_message);
      int prefix_len = strlen(prefix);
      int len = original_len + prefix_len;

      char message_with_ds_id[len + 1]; // +1 for the null terminator
      snprintf(message_with_ds_id, len + 1, "%s%s", prefix, log_message);

      struct pbuf *p = pbuf_alloc(PBUF_TRANSPORT, len, PBUF_RAM);
      if (p == NULL) {
        DEBUG_LOG("%s Failed to allocate pbuf.\n", U_TAG);
        return;
      }

      memcpy(p->payload, message_with_ds_id, len);
      cyw43_arch_lwip_begin();
      err_t err = udp_sendto(upcb, p, &destAddr, UDP_PORT);
      cyw43_arch_lwip_end();

      pbuf_free(p);

      if (err != ERR_OK) {
        DEBUG_LOG("%s Error sending message: %d\n", U_TAG, err);
        return;
      }
    }
  }
}

void send_udp_message(char *message) {
  // Allocate pbuf
}

void init_udp() {
  upcb = udp_new();
  if (upcb == NULL) {
    DEBUG_LOG("%s Failed to create new UDP control block\n", U_TAG);
    return;
  }
  err_t err = udp_bind(upcb, IP_ADDR_ANY, UDP_PORT);
  if (err < 0) {
    DEBUG_LOG("%s Error binding to udp: %hhd\n", U_TAG, err);
  }
}
