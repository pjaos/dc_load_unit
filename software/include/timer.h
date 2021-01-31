#ifndef SRC_INCLUDE_TIMERS_H_
#define SRC_INCLUDE_TIMERS_H_

void set_load(float factor);
void set_target_amps(float amps);
void set_target_watts(float watts);
void init_timers(void);
void set_target_watts(float watts);
float get_target_amps(void);
float get_target_watts(void);
float get_amps(void);
float get_volts(void);
float get_watts(void);
float get_temp_C(void);
float get_temp_alarm(void);
void set_pid_coeffs(float p, float i, float d);
void set_load_off_voltage(float load_off_voltage);

#endif /* SRC_INCLUDE_TIMERS_H_ */
