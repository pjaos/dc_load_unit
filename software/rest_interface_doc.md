# REST interface documentation.
The DC Load unit has a REST interface. The following documentation details
this interface. The examples here show the DC load unit having an IP address
of 192.168.0.245.

#Calling rest IF

curl http://192.168.0.245/rpc/reboot

curl http://192.168.0.245/rpc/factorydefault

curl http://192.168.0.245/rpc/pwm -d '{"pwm":0.1}'
0.500000

curl http://192.168.0.245/rpc/target_amps -d '{"amps":1.1}'
1.100000

curl http://192.168.0.245/rpc/target_power -d '{"watts":10}'
10.000000

curl http://192.168.0.245/rpc/reset_temp_alarm


curl http://192.168.0.245/rpc/debug -d '{"level":2}'
2

curl http://192.168.0.245/rpc/get_config
{"unit_name":"DC Load Unit","group_name":"","syslog_enabled":0,"max_pp_count":60,"target_amps":0.0,"max_watts":150.0,"load_off_voltage":0.000000}

curl http://192.168.0.245/rpc/set_config -d '{"dev_name": "DC Load Unit\n","group_name": "","enable_syslog": 1,"max_pp_count": 61,"target_amps": 1.0,"target_watts": 0.0,"load_off_voltage": 2.000000,"max_watts": 55.000000}'

curl http://192.168.0.245/rpc/get_stats
{"amps":0.003405,"volts":0.000000,"watts":0.000000,"temp_c":20.465612,"temp_alarm":0,"fan_on_count":0,"amp_hours":0.000000,"watt_hours":0.000000,"previous_amp_hours":0.000000,"previous_watt_hours":0.000000,"load_on_secs":0.000000,"previous_load_on_secs":0.000000,"min_load_voltage_alarm":0,"max_load_voltage_alarm":0,"audio_alarm":0}

curl http://192.168.0.245/rpc/pid_coeffs -d '{"P":0.5, "I":0.1, "D":0.1}'
{"P":0.500000,"I":0.100000,"D":0.100000}

curl http://192.168.0.245/rpc/set_current_cal -d '{"cal":1.0}'
1.000000

curl http://192.168.0.245/rpc/set_voltage_cal -d '{"cal":1.0}'
1.000000

curl http://192.168.0.245/rpc/set_load_off_voltage -d '{"volts":10.0}'
10.000000

curl http://192.168.0.245/rpc/reset_temp_alarm

curl http://192.168.0.245/rpc/reset_min_load_voltage_alarm

curl http://192.168.0.245/rpc/reset_max_load_voltage_alarm

curl http://192.168.0.245/rpc/set_max_pwm -d '{"value":0.36}'

curl http://192.168.0.245/rpc/set_audio_alarm -d '{"on":1}'

### This causes the syslog output to be sent to the server the YView AYT message was received from
curl http://192.168.0.245/rpc/update_syslog
null
