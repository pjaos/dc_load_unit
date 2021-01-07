#ifndef _FAN
#define _FAN

enum fan_id_list{FAN1=1, FAN2, FAN3, FAN4, FAN5};

void update_fan_state(uint8_t fan_id, float temp);
void init_fans(void);

#endif
