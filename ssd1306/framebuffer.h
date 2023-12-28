
#ifndef framebuffer_H
#define framebuffer_H

#define FB_SIZE 1024

typedef struct {
  unsigned char* buffer;
} FrameBuffer;

int fb_init(FrameBuffer* fb);

void fb_byte_or(FrameBuffer* fb, int n, unsigned char byte);

void fb_byte_and(FrameBuffer* fb, int n, unsigned char byte);

void fb_byte_xor(FrameBuffer* fb, int n, unsigned char byte);

void fb_clear(FrameBuffer* fb);

void fb_destroy(FrameBuffer* fb);

#endif
