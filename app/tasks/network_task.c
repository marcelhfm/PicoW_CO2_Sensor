#include "cyw43_ll.h"
#include "hardware/watchdog.h"
#include "lwip/err.h"
#include "lwip/ip4_addr.h"
#include "lwip/tcpbase.h"
#include "pico/cyw43_arch.h"

#include "../main.h"
#include "logging.h"
#include "lwip/tcp.h"
#include "network_task.h"
#include "projdefs.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <task.h>

#define WATCHDOG_TIMEOUT_MS 40000 // 10s

const char *N_TAG = "NETWORK";

extern QueueHandle_t network_queue;
extern TaskHandle_t update_display_handle;
extern bool display_on;

static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len);
static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb, err_t err);
static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
                               err_t err);
static void tcp_error_callback(void *arg, err_t err);
static void try_tcp_send(struct tcp_pcb *tpcb, const char *data);

enum Commands parse_command(const char *message) {
  int command = atoi(message);
  switch (command) {
  case CMD_DISPLAY_OFF:
    return CMD_DISPLAY_OFF;
  case CMD_DISPLAY_ON:
    return CMD_DISPLAY_ON;
  default:
    return CMD_NONE;
  }
}

static struct tcp_pcb *tcp_pcb;

static void tcp_client_init() {
  ip_addr_t server_ip;
  tcp_pcb = tcp_new();
  if (!tcp_pcb) {
    u_log(L_ERROR, N_TAG, "Error creating PCB.\n");
    return;
  }

  IP4_ADDR(&server_ip, 192, 168, 11, 30);
  u_log(L_INFO, N_TAG, "Attempting to bind and connect to server at %s:%d\n",
        ip4addr_ntoa(&server_ip), SERVER_PORT);

  if (tcp_bind(tcp_pcb, IP_ADDR_ANY, 0) != ERR_OK) {
    u_log(L_ERROR, N_TAG, "Error binding TCP.\n");
    tcp_abort(tcp_pcb);
    return;
  }

  tcp_connect(tcp_pcb, &server_ip, SERVER_PORT, tcp_connected_callback);
}

static err_t tcp_connected_callback(void *arg, struct tcp_pcb *tpcb,
                                    err_t err) {
  if (err == ERR_OK) {
    tcp_sent(tpcb, tcp_sent_callback);
    tcp_recv(tpcb, tcp_recv_callback);
    tcp_err(tpcb, tcp_error_callback);

    return ERR_OK;
  } else {
    u_log(L_ERROR, N_TAG, "Connection failed with error code %d\n", err);
    return err;
  }
}

static err_t tcp_sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
  u_log(L_INFO, N_TAG, "Data sent Successfully.\n");
  return ERR_OK;
}

static void tcp_error_callback(void *arg, err_t err) {
  u_log(L_WARN, N_TAG,
        "TCP connection error %d encountered. Restoring "
        "connection\n",
        err);

  struct tcp_pcb *tpcb = (struct tcp_pcb *)arg;
  if (tpcb) {
    tcp_pcb = NULL;
    tcp_close(tpcb);
  }

  vTaskDelay(pdMS_TO_TICKS(5000));
  tcp_client_init();
}

static err_t tcp_recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p,
                               err_t err) {
  if (p == NULL) {
    u_log(L_WARN, N_TAG, "Connection was closed.\n");
    tcp_close(tpcb);
    return ERR_ABRT;
  } else if (err == ERR_OK) {
    char *received_data = (char *)p->payload;

    received_data[p->len] = '\0';
    enum Commands cmd = parse_command(received_data);

    switch (cmd) {
    case CMD_DISPLAY_OFF:
      u_log(L_INFO, N_TAG, "Received CMD_DISPLAY_OFF\n");
      xTaskNotify(update_display_handle, cmd, eSetValueWithOverwrite);
      break;
    case CMD_DISPLAY_ON:
      u_log(L_INFO, N_TAG, "Received CMD_DISPLAY_ON\n");
      xTaskNotify(update_display_handle, cmd, eSetValueWithOverwrite);
      break;
    default:
      u_log(L_WARN, N_TAG, "Received unknown command\n");
      break;
    }

    tcp_recved(tpcb, p->tot_len);
    pbuf_free(p);
  }

  return ERR_OK;
}

static void try_tcp_send(struct tcp_pcb *tpcb, const char *data) {
  if (tpcb && tpcb->state == ESTABLISHED) {
    err_t err = tcp_write(tpcb, data, strlen(data), TCP_WRITE_FLAG_COPY);
    if (err == ERR_OK) {
      tcp_output(tpcb);
    } else {
      u_log(L_ERROR, N_TAG, "Failed to send data: %d\n", err);
    }
  } else {
    u_log(L_WARN, N_TAG, "Connection lost, attempting reconnect...\n");
    tcp_client_init();

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void network_task() {
  u_log(L_INFO, N_TAG, "Hello from network_task!\n");

  tcp_client_init();
  watchdog_enable(WATCHDOG_TIMEOUT_MS, 1);

  measurements_t data;
  char buffer[100];

  while (1) {
    watchdog_update();
    if (xQueueReceive(network_queue, &data, portMAX_DELAY) == pdPASS) {
      watchdog_update();

      if (snprintf(buffer, sizeof(buffer), "1,%d,%d,%d,%d\n", data.co2,
                   data.temp, data.humidity, display_on) >= sizeof(buffer)) {
        u_log(L_ERROR, N_TAG, "Buffer overflow detected!\n");
      }

      cyw43_arch_lwip_begin();
      try_tcp_send(tcp_pcb, buffer);
      cyw43_arch_lwip_end();
    }
  }

  vTaskDelete(NULL);
}
