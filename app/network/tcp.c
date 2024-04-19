#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "pico/stdlib.h"

/*
 * Required by CoreMqtt
 */
uint16_t tcp_get_curr_time() { return to_ms_since_boot(get_absolute_time()); }

int32_t tcp_trans_send() {}
