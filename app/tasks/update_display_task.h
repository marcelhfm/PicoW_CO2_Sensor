#ifndef UPDATE_DISPLAY_TASK_H
#define UPDATE_DISPLAY_TASK_H

#include "../main.h"
#include "../ssd1306/display.h"
#include "../ssd1306/framebuffer.h"

typedef struct {
  FrameBuffer* fb;
  enum WriteMode wm;
  enum Rotation rot;
} update_display_params;

enum STATUS {
  STATUS_GOOD,
  STATUS_BAD,
};

typedef struct {
  float co2_measurement;
  enum STATUS wifi_status;
  enum STATUS sensor_status;
  bool flash;
} DisplayInfo;

void update_display_task(void* task_params);

#endif
