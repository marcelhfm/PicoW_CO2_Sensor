#include "main.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <task.h>

#include "hardware/watchdog.h"
#include "i2c/i2c.h"
#include "pico/stdio.h"
#include "projdefs.h"
#include "ssd1306/display.h"
#include "ssd1306/ssd1306.h"
#include "tasks/network_task.h"
#include "tasks/read_data_task.h"
#include "tasks/update_display_task.h"

volatile QueueHandle_t display_queue = NULL;
volatile QueueHandle_t network_queue = NULL;
TaskHandle_t read_data_handle = NULL;
TaskHandle_t update_display_handle = NULL;
TaskHandle_t network_task_handle = NULL;

int main() {
  stdio_init_all();

  if (watchdog_caused_reboot()) {
    printf("Rebooted by Watchdog!\n");
  } else {
    printf("Clean boot\n");
  }

  sleep_ms(2000); // Wait for serial_port to be initialized

  // Init display
  init_i2c();
  i2c_scan();

  oled_init();

  FrameBuffer fb;
  if (fb_init(&fb) != 0) {
    printf("update_display_task: Init failed, fb init failed.\n");
    return -1;
  }
  fb_clear(&fb);

  enum WriteMode wm = ADD;
  enum Rotation rot = deg0;
  update_display_params ud_params;
  ud_params.fb = &fb;
  ud_params.wm = wm;
  ud_params.rot = rot;
  printf("main: Creating Tasks\n");
  BaseType_t read_data_status = xTaskCreate(read_data_task, "READ_DATA_TASK",
                                            2056, NULL, 2, &read_data_handle);

  BaseType_t update_display_status =
      xTaskCreate(update_display_task, "UPDATE_DISPLAY_TASK", 4112,
                  (void *)&ud_params, 1, &update_display_handle);

  BaseType_t network_task_status = xTaskCreate(
      network_task, "NETWORK_TASK", 2056, NULL, 1, &network_task_handle);

  printf("main: Creating queues\n");
  display_queue = xQueueCreate(5, sizeof(int));
  network_queue = xQueueCreate(5, sizeof(measurements_t));

  if (read_data_status == pdPASS && update_display_status == pdPASS &&
      network_task_status == pdPASS) {
    printf("main: Starting scheduler!\n");
    vTaskStartScheduler();
  } else {
    printf("main: Unable to start scheduler! RD: %ld UD: %ld", read_data_status,
           update_display_status);
    return -1;
  }
  // should never be reached
  while (1)
    ;
}
