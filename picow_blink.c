#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "ssd1306.h"

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  // Setup Display
  printf("Main: Waiting for serial port to be ready...\n");
  sleep_ms(5000);

  printf("Test!\n");

  init_i2c();
  i2c_scan();

  oled_init();

  uint8_t invert = 0;
  while (1) {
    printf("Flashing display \n");
    flash_display(invert);
    if (invert == 0) {
      invert = 1;
    } else {
      invert = 0;
    }

    sleep_ms(500);
  }

  while (1) {
    printf("Updating display...\n");

    oled_string("Hello, World!");

    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(250);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(4750);
  }
}
