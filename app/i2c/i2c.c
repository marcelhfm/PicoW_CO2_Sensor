#include "i2c.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include "pico/stdlib.h"

#define I2C_PORT i2c0

// For SSD1306
#define I2C_SDA_PIN 0
#define I2C_SCL_PIN 1

// For SCD40
#define I2C_SDA_PIN2 4
#define I2C_SCL_PIN2 5

void init_i2c() {
  printf("init_i2c: Initializing i2c...\n");
  i2c_init(I2C_PORT, 100 * 1000);  // 100 kHz

  // SSD1306
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);

  // SCD40
  gpio_set_function(I2C_SDA_PIN2, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SCL_PIN2, GPIO_FUNC_I2C);

  i2c_set_slave_mode(I2C_PORT, false, 0);
  printf("init_i2c: Done\n");
}

void i2c_scan() {
  printf("Scanning I2C bus...\n");
  uint8_t address;
  uint8_t data;

  for (address = 1; address < 128;
       address++) {  // 7-bit addresses from 0x01 to 0x7F
    int result = i2c_read_blocking(I2C_PORT, address, &data, 1, false);

    if (result !=
        PICO_ERROR_GENERIC) {  // Check if there is a response from the address
      printf("I2C device detected at address 0x%02X\n", address);
    }
  }
}
