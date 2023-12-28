#include "display.h"

#include "font8x8_basic.h"

#define WIDTH 128
#define HEIGHT 64

void display_set_pixel(FrameBuffer* fb, int16_t x, int16_t y,
                       enum WriteMode mode) {
  if ((x < 0) || (x >= WIDTH) || (y < 0) || (y >= HEIGHT)) {
    printf("display_set_pixel: Coordinates out of bounds.\n");
    return;
  }

  uint8_t byte = 1 << (y & 7);

  if (mode == ADD) {
    fb_byte_or(fb, x + (y / 8) * WIDTH, byte);
  } else if (mode == SUBTRACT) {
    fb_byte_and(fb, x + (y / 8) * WIDTH, byte);
  } else if (mode == INVERT) {
    fb_byte_xor(fb, x + (y / 8) * WIDTH, byte);
  }
}

void display_send_buffer(FrameBuffer* fb) {
  oled_command(CMD_PAGE_ADDR);
  oled_command(0x00);  // From min
  oled_command(0x07);  // To max
  oled_command(CMD_COL_ADDR);
  oled_command(0x00);  // From min
  oled_command(127);   // To max

  send_data(fb->buffer, FB_SIZE);
}

void display_set_orientation(bool orientation) {
  if (orientation) {
    oled_command(CMD_COL_REMAP_OFF);
    oled_command(CMD_COM_REMAP_OFF);
  } else {
    oled_command(CMD_COL_REMAP_ON);
    oled_command(CMD_COM_REMAP_OFF);
  }
}

void display_draw_text(FrameBuffer* fb, const char* text, uint8_t x, uint8_t y,
                       enum WriteMode mode, enum Rotation rot) {
  if (!text) {
    printf("display_draw_text: No text provided.\n");
    return;
  }

  uint8_t font_width = font_8x8[0];

  uint16_t n = 0;

  while (text[n] != '\0') {
    switch (rot) {
      case deg0:
        display_draw_char(fb, text[n], x + (n * font_width), y, mode, rot);
        break;
      case deg90:
        display_draw_char(fb, text[n], x, y + (n * font_width), mode, rot);
        break;
    }

    n++;
  }
}

void display_draw_char(FrameBuffer* fb, char c, uint8_t anchor_x,
                       uint8_t anchor_y, enum WriteMode mode,
                       enum Rotation rot) {
  if (c < 32) {
    printf("display_draw_char: Invalid char %c", c);
    return;
  }

  uint8_t font_width = font_8x8[0];
  uint8_t font_height = font_8x8[1];

  uint16_t seek = (c - 32) * (font_width * font_height) / 8 + 2;

  uint8_t b_seek = 0;

  for (uint8_t x = 0; x < font_width; x++) {
    for (uint8_t y = 0; y < font_height; y++) {
      if (font_8x8[seek] >> b_seek & 0b00000001) {
        switch (rot) {
          case deg0:
            display_set_pixel(fb, x + anchor_x, y + anchor_y, mode);
            break;
          case deg90:
            display_set_pixel(fb, -y + anchor_x + font_height, x + anchor_y,
                              mode);
            break;
        }
      }
      b_seek++;
      if (b_seek == 8) {
        b_seek = 0;
        seek++;
      }
    }
  }
}
