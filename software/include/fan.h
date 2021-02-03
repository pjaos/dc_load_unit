#ifndef _FAN
#define _FAN

#define MAX_HEATSINK_TEMP 79.0

void set_cooling(float temp);
void init_fans(void);
uint8_t get_fan_on_count(void);

#endif
