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
bool get_temp_alarm(void);
void set_pid_coeffs(float p, float i, float d);
void reset_temp_alarm(void);
double get_amp_hours(void);
double get_watt_hours(void);
double get_previous_amp_hours(void);
double get_previous_watt_hours(void);
double get_load_on_secs(void);
double get_previous_load_on_secs(void);
bool get_min_load_voltage_alarm(void);
void reset_min_load_voltage_alarm(void);
bool get_max_load_voltage_alarm(void);
void reset_max_load_voltage_alarm(void);
void set_max_pwm(float pwm);
bool get_audio_alarm(void);
void set_audio_alarm(bool on);

#endif /* SRC_INCLUDE_TIMERS_H_ */
