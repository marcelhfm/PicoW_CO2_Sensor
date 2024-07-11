#include "framebuffer.h"

#include "../tasks/logging.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *F_TAG = "FRAMEBUFFER";

int fb_init(FrameBuffer *fb) {
  if (fb == 0) {
    u_log(L_ERROR, F_TAG, "fb is null!\n");
    return -1;
  }

  fb->buffer = (unsigned char *)malloc(FB_SIZE);
  if (fb->buffer == 0) {
    u_log(L_ERROR, F_TAG, "Memory allocation failure!\n");
  }

  return 0;
}

void fb_byte_or(FrameBuffer *fb, int n, unsigned char byte) {
  if (n > (FB_SIZE - 1)) {
    u_log(L_ERROR, F_TAG, "Offset out of range!\n");
  };
  fb->buffer[n] |= byte;
}

void fb_byte_and(FrameBuffer *fb, int n, unsigned char byte) {
  if (n > (FB_SIZE - 1))
    return;
  fb->buffer[n] &= byte;
}

void fb_byte_xor(FrameBuffer *fb, int n, unsigned char byte) {
  if (n > (FB_SIZE - 1))
    return;
  fb->buffer[n] ^= byte;
}

void fb_clear(FrameBuffer *fb) { memset(fb->buffer, 0, FB_SIZE); }

void fb_destroy(FrameBuffer *fb) {
  if (fb != 0 && fb->buffer != 0) {
    free(fb->buffer);
    fb->buffer = 0;
  }
}
