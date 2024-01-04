#include "read_data_task.h"

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

#define ADC_VREF 3.3  // Reference voltage in millivolts
#define ADC_RESOLUTION 4096

// Constants for temperature calculation
#define TEMP_SCALE_FACTOR 1
#define VOLTAGE_OFFSET 0.706  // Offset in millivolts
#define TEMP_COEFFICIENT \
  0.001721  // Coefficient in microvolts per degree Celsius

extern QueueHandle_t queue;

const TickType_t read_data_delay = 5000 / portTICK_PERIOD_MS;

int read_data() {
  uint16_t raw = adc_read();

  // Convert the raw reading to millivolts without using floating point
  float voltage_mV = raw * (ADC_VREF / ADC_RESOLUTION);

  // Calculate the temperature in degrees Celsius scaled by TEMP_SCALE_FACTOR
  float temp_c =
      27 - (voltage_mV - VOLTAGE_OFFSET) * TEMP_SCALE_FACTOR / TEMP_COEFFICIENT;

  printf("read_data: temperature is: %f°C\n", temp_c);

  // Return the temperature in milli-Celsius
  return temp_c * 1000;
}

void read_data_task() {
  printf("Hello from read_data_task!\n");
  adc_init();
  adc_set_temp_sensor_enabled(true);
  adc_select_input(4);  // onboard temp sensor

  int temp_mC = 0;  // Temperature in milli-Celsius

  while (1) {
    temp_mC = read_data();
    xQueueSendToBack(queue, &temp_mC, 0);
    vTaskDelay(read_data_delay);
  }
}
