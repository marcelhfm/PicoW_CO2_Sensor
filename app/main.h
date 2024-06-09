#ifndef MAIN_H
#define MAIN_H

typedef struct measurements_t {
  int co2;
  int temp;
  int humidity;
} measurements_t;

enum Commands { CMD_NONE = 0, CMD_DISPLAY_OFF = 1, CMD_DISPLAY_ON = 2 };

#ifdef ENABLE_DEBUG_LOGS
#define DEBUG_LOG(fmt, args...) printf("DEBUG: " fmt "\n", ##args)
#else
#define DEBUG_LOG(fmt, args...) // Do nothing
#endif

#endif // MAIN_H
