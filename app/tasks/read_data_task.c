#include "read_data_task.h"

#include "../main.h"
#include "logging.h"
#include <FreeRTOS.h>
#include <queue.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>

#include "../scd40/scd40.h"

extern QueueHandle_t display_queue;
extern QueueHandle_t network_queue;

const TickType_t read_data_delay = 5000 / portTICK_PERIOD_MS;

const char *R_TAG = "READ_DATA_TASK";

void read_data_task() {
  u_log(L_INFO, R_TAG, "Hello from read_data_task!\n");

  measurements_t data;

  int16_t retval = scd40_init();

  if (retval != 0) {
    u_log(L_ERROR, R_TAG, "Error initializing task: %i\n", retval);
    vTaskDelete(NULL);
    return;
  }

  uint16_t co2;
  int32_t temp_mc;
  int16_t humidity_m_percent_rh;

  while (1) {
    bool data_ready_flag = true;

    retval = scd40_get_data_ready_flag(&data_ready_flag);

    if (retval != 0) {
      u_log(L_ERROR, R_TAG, "Error executing get_data_ready_flag: %i\n",
            retval);
      vTaskDelay(read_data_delay / 5); // 1000ms
      continue;
    }
    if (!data_ready_flag) {
      u_log(L_WARN, R_TAG, "Data was not ready...\n");
      vTaskDelay(read_data_delay / 5); // 1000ms
      continue;
    }

    retval = scd40_read_measurement(&co2, &temp_mc, &humidity_m_percent_rh);
    if (retval != 0) {
      u_log(L_ERROR, R_TAG, "error executing read_measurement: %i\n", retval);
    } else if (co2 == 0) {
      u_log(L_WARN, R_TAG, "invalid sample detected, skipping.\n");
    } else {
      u_log(L_INFO, R_TAG, "C02: %u; Temp: %i; Humidity Percent: %i\n", co2,
            temp_mc, humidity_m_percent_rh);

      data.co2 = co2;
      data.temp = temp_mc;
      data.humidity = humidity_m_percent_rh;

      xQueueSendToBack(display_queue, &co2, 0);
      xQueueSendToBack(network_queue, &data, 0);
    }

    vTaskDelay(read_data_delay);
  }
}
