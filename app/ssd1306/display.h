#ifndef display_H
#define display_H

#include <stdbool.h>
#include <stdint.h>

#include "framebuffer.h"

enum WriteMode : const unsigned char {
  // Set pixel on, regardless of state
  ADD = 0,
  // Set pixel off, regardless of state
  SUBTRACT = 1,
  // inverst pixel
  INVERT = 2,
};

enum Rotation {
  deg0,
  deg90,
};

// Set a single pixel
void display_set_pixel(FrameBuffer* fb, int16_t x, int16_t y,
                       enum WriteMode mode);

// Sends framebuffer to display, to update display
void display_send_buffer(FrameBuffer* fb);

// Set orientation of display -> 0 = not flipped, 1 = flipped
void display_set_orientation(bool orientation);

// Draw some text
void display_draw_text(FrameBuffer* fb, const char* text, uint8_t x, uint8_t y,
                       enum WriteMode mode, enum Rotation rot);

// Draw single char
void display_draw_char(FrameBuffer* fb, const char c, uint8_t x, uint8_t y,
                       enum WriteMode mode, enum Rotation rot);

#endif
