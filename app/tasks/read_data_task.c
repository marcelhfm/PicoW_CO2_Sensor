#include "read_data_task.h"

#include <FreeRTOS.h>
#include <queue.h>
#include <stdint.h>
#include <stdio.h>
#include <task.h>

#include "hardware/adc.h"
#include "pico/stdlib.h"

#define ADC_VREF 3300  // Reference voltage in millivolts
#define ADC_RESOLUTION 4096

// Constants for temperature calculation
#define TEMP_SCALE_FACTOR 1000
#define VOLATAGE_OFFSET 706    // Offset in millivolts
#define TEMP_COEFFICIENT 1721  // Coefficient in microvolts per degree Celsius

extern QueueHandle_t queue;

const TickType_t read_data_delay = 500 / portTICK_PERIOD_MS;

int read_data() {
  uint16_t raw = adc_read();

  // Convert the raw reading to millivolts without using floating point
  int voltage_mV = (raw * ADC_VREF) / ADC_RESOLUTION;

  // Calculate the temperature in degrees Celsius scaled by TEMP_SCALE_FACTOR
  int temp_mC = 27000 - (voltage_mV - VOLATAGE_OFFSET) * TEMP_SCALE_FACTOR /
                            TEMP_COEFFICIENT;

  printf("Temperature is: %d.%03d°C\n", temp_mC / TEMP_SCALE_FACTOR,
         temp_mC % TEMP_SCALE_FACTOR);

  // Return the temperature in milli-Celsius
  return temp_mC;
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
