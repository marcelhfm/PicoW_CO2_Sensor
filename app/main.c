#include <stdbool.h>
#include <stdint.h>

#include "pico/cyw43_arch.h"
#include "pico/stdlib.h"
#include "ssd1306/display.h"
#include "ssd1306/framebuffer.h"

enum STATUS {
  STATUS_GOOD,
  STATUS_BAD,
};

typedef struct {
  uint16_t co2_measurement;
  enum STATUS wifi_status;
  enum STATUS sensor_status;
  bool flash;
} DisplayInfo;

void draw_checkmark(FrameBuffer* fb, int x, int y, enum WriteMode wm) {
  display_set_pixel(fb, x + 0, y + 5, wm);
  display_set_pixel(fb, x + 1, y + 6, wm);
  display_set_pixel(fb, x + 2, y + 7, wm);
  display_set_pixel(fb, x + 3, y + 6, wm);
  display_set_pixel(fb, x + 4, y + 5, wm);
  display_set_pixel(fb, x + 5, y + 4, wm);
  display_set_pixel(fb, x + 6, y + 3, wm);
  display_set_pixel(fb, x + 7, y + 2, wm);
}

void draw_cross(FrameBuffer* fb, int x, int y, enum WriteMode wm) {
  for (int i = 0; i < 8; ++i) {
    display_set_pixel(fb, x + i, y + i, wm);
    display_set_pixel(fb, x + 7 - i, y + i, wm);
  }
}

void update_display(DisplayInfo* display_info, FrameBuffer* fb,
                    enum WriteMode wm, enum Rotation rot) {
  fb_clear(fb);

  // Draw status bar
  display_draw_text(fb, "S:", 0, 0, wm, rot);
  display_draw_text(fb, "W:", 128 - 8 - 8 - 8, 0, wm, rot);

  if (display_info->sensor_status == STATUS_GOOD) {
    draw_checkmark(fb, 16, 0, wm);
  } else if (display_info->sensor_status == STATUS_BAD) {
    draw_cross(fb, 16, 0, wm);
  }

  if (display_info->wifi_status == STATUS_GOOD) {
    draw_checkmark(fb, 120, 0, wm);
  } else if (display_info->wifi_status == STATUS_BAD) {
    draw_cross(fb, 120, 0, wm);
  }

  for (int i = 0; i < 128; i++) {
    display_set_pixel(fb, i, 10, wm);
  }

  // Draw co2 levels
  char str[12];
  sprintf(str, "%d", display_info->co2_measurement);
  strcat(str, " ppm");

  display_draw_text(fb, str, 128 / 3, 20, wm, rot);
  // Draw co2 levels bar + warning
  // Calculate the fill level based on co2_measurement (range: 0 - 2500)
  uint8_t fillLevel =
      (uint8_t)((display_info->co2_measurement * (128 - 15 - 14)) / 2500);

  // Adjust fill level if it exceeds the bar's width
  if (fillLevel > (128 - 15 - 14)) {
    fillLevel = (128 - 15 - 14);
  }

  // Draw CO2 levels bar
  // Bar outline
  for (uint8_t i = 15; i < 128 - 14; i++) {
    display_set_pixel(fb, i, 32, wm);
    display_set_pixel(fb, i, 36, wm);
  }
  for (uint8_t i = 32; i <= 36; i++) {
    display_set_pixel(fb, 15, i, wm);
    display_set_pixel(fb, 128 - 14, i, wm);
  }

  // Fill in the bar based on the CO2 measurement
  for (uint8_t i = 16; i < 16 + fillLevel; i++) {
    for (uint8_t j = 33; j <= 35; j++) {
      display_set_pixel(fb, i, j, wm);
    }
  }

  // Status text
  if (display_info->co2_measurement < 400) {
    display_draw_text(fb, "Good", 128 / 3 + 8, 42, wm, rot);
  } else if (display_info->co2_measurement > 2000) {
    if (display_info->flash) {
      display_draw_text(fb, "!!Critical!!", 128 / 3 - 24, 42, wm, rot);
    }
  } else if (display_info->co2_measurement > 1000) {
    if (display_info->flash) {
      display_draw_text(fb, "!Warning!", 128 / 3 - 10, 42, wm, rot);
    }
  } else if (display_info->co2_measurement > 400) {
    display_draw_text(fb, "Bad", 128 / 3 + 12, 42, wm, rot);
  }

  display_info->flash = !display_info->flash;
  display_send_buffer(fb);
}

int main() {
  stdio_init_all();
  if (cyw43_arch_init()) {
    printf("Wi-Fi init failed\n");
    return -1;
  }

  // Waiting a bit for serial port to be initialized
  sleep_ms(3000);

  init_i2c();
  i2c_scan();

  oled_init();

  FrameBuffer fb;
  if (fb_init(&fb) != 0) {
    printf("main: fb init failed!\n");
    return -1;
  };
  fb_clear(&fb);

  enum WriteMode wm = ADD;
  enum Rotation rot = deg0;

  uint8_t invert = 0;

  DisplayInfo display_info;
  display_info.wifi_status = STATUS_GOOD;
  display_info.sensor_status = STATUS_BAD;
  display_info.co2_measurement = 1500;
  display_info.flash = false;

  while (1) {
    update_display(&display_info, &fb, wm, rot);
    sleep_ms(1000);
  }
}
