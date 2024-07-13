#include "main.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>

#include "hardware/watchdog.h"
#include "i2c/i2c.h"
#include "pico/cyw43_arch.h"
#include "pico/stdio.h"
#include "portmacro.h"
#include "projdefs.h"
#include "ssd1306/display.h"
#include "ssd1306/ssd1306.h"
#include "tasks/logging.h"
#include "tasks/network_task.h"
#include "tasks/read_data_task.h"
#include "tasks/update_display_task.h"

volatile QueueHandle_t log_queue = NULL;
volatile QueueHandle_t display_queue = NULL;
volatile QueueHandle_t network_queue = NULL;
TaskHandle_t read_data_handle = NULL;
TaskHandle_t update_display_handle = NULL;
TaskHandle_t network_task_handle = NULL;

char *TAG = "MAIN";

bool display_on;

void init_wifi() {
  if (cyw43_arch_init()) {
    DEBUG_LOG("[MAIN] Failed to initialize CYW43\n");
    watchdog_reboot(0, 0, 0);
  }

  cyw43_arch_enable_sta_mode();

  DEBUG_LOG("[MAIN] Connecting to WiFi %s and pwd %s\n", WIFI_SSID,
            WIFI_PASSWORD);

  int err = cyw43_arch_wifi_connect_blocking(WIFI_SSID, WIFI_PASSWORD,
                                             CYW43_AUTH_WPA2_AES_PSK);
  if (err != 0) {
    DEBUG_LOG("[MAIN] Error connecting to wifi: %d", err);
    watchdog_reboot(0, 0, 0);
  }
  DEBUG_LOG("[MAIN] Successfully connected to WiFi!\n");
}

int main() {
  stdio_init_all();

  sleep_ms(2000); // Wait for serial_port to be initialized

  if (watchdog_caused_reboot()) {
    DEBUG_LOG("[MAIN] Rebooted by Watchdog!\n");
  } else {
    DEBUG_LOG("[MAIN] Clean boot\n");
  }

  init_wifi();

  DEBUG_LOG("[%s] Creating queues\n", TAG);
  display_queue = xQueueCreate(5, sizeof(int));
  network_queue = xQueueCreate(5, sizeof(measurements_t));
  log_queue = xQueueCreate(LOG_QUEUE_LENGTH, LOG_MESSAGE_MAX_LENGTH);

  // Init display
  init_i2c();
  i2c_scan();

  oled_init();

  FrameBuffer fb;
  if (fb_init(&fb) != 0) {
    u_log(L_ERROR, TAG, "FrameBuffer init failed.");
    return -1;
  }
  fb_clear(&fb);

  init_udp();

  enum WriteMode wm = ADD;
  enum Rotation rot = deg0;
  update_display_params ud_params;
  ud_params.fb = &fb;
  ud_params.wm = wm;
  ud_params.rot = rot;

  u_log(L_INFO, TAG, "Creating Tasks\n");
  BaseType_t udp_log_status = xTaskCreate(udp_log_task, "UDP Log Task", 1024,
                                          NULL, tskIDLE_PRIORITY, NULL);

  BaseType_t read_data_status = xTaskCreate(read_data_task, "READ_DATA_TASK",
                                            2056, NULL, 2, &read_data_handle);

  BaseType_t update_display_status =
      xTaskCreate(update_display_task, "UPDATE_DISPLAY_TASK", 4112,
                  (void *)&ud_params, 1, &update_display_handle);

  BaseType_t network_task_status = xTaskCreate(
      network_task, "NETWORK_TASK", 2056, NULL, 1, &network_task_handle);
  if (read_data_status == pdPASS && update_display_status == pdPASS &&
      udp_log_status == pdPASS) {
    // if (read_data_status == pdPASS && update_display_status == pdPASS &&
    //     network_task_status == pdPASS) {
    u_log(L_DEBUG, TAG, "Starting scheduler!\n");
    vTaskStartScheduler();
  } else {
    u_log(L_ERROR, TAG, "Unable to start scheduler! RD: %ld UD: %ld",
          read_data_status, update_display_status);
    return -1;
  }
  // should never be reached
  while (1)
    ;
}
