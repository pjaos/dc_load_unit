EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 2
Title ""
Date ""
Rev "1.3"
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2050 2500 0    50   Input ~ 0
Shunt
Text HLabel 2050 2950 0    50   Input ~ 0
GateDrive
Text HLabel 2050 3850 0    50   Input ~ 0
GND
$Comp
L pja_1:IRFP250N Q6
U 1 1 6059C924
P 4900 2950
F 0 "Q6" H 5150 2992 45  0000 L CNN
F 1 "IXFH80N25X3" H 5150 2908 45  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-247-3_Vertical" H 4930 3100 20  0001 C CNN
F 3 "" H 4900 2950 60  0001 C CNN
	1    4900 2950
	1    0    0    -1  
$EndComp
$Comp
L pja_1:IRFP250N Q9
U 1 1 6059D76F
P 6500 2950
F 0 "Q9" H 6750 2992 45  0000 L CNN
F 1 "IXFH80N25X3" H 6750 2908 45  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-247-3_Vertical" H 6530 3100 20  0001 C CNN
F 3 "" H 6500 2950 60  0001 C CNN
	1    6500 2950
	1    0    0    -1  
$EndComp
Wire Wire Line
	2750 3850 2050 3850
Wire Wire Line
	6050 3050 6100 3050
Wire Wire Line
	4450 3050 4500 3050
Wire Wire Line
	4900 3750 5450 3750
Wire Wire Line
	5450 3750 5450 2500
Connection ~ 4900 3750
Wire Wire Line
	4900 3750 4900 3350
$Comp
L Device:R R37
U 1 1 605B6FCB
P 2850 2950
F 0 "R37" V 2643 2950 50  0000 C CNN
F 1 "2k2" V 2734 2950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2780 2950 50  0001 C CNN
F 3 "~" H 2850 2950 50  0001 C CNN
	1    2850 2950
	0    1    1    0   
$EndComp
$Comp
L Device:R R38
U 1 1 605B7580
P 2850 3250
F 0 "R38" V 2643 3250 50  0000 C CNN
F 1 "2k2" V 2734 3250 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2780 3250 50  0001 C CNN
F 3 "~" H 2850 3250 50  0001 C CNN
	1    2850 3250
	0    1    1    0   
$EndComp
Wire Wire Line
	2050 2950 2600 2950
Wire Wire Line
	2700 3250 2600 3250
Wire Wire Line
	2600 3250 2600 2950
Connection ~ 2600 2950
Wire Wire Line
	2600 2950 2700 2950
Wire Wire Line
	3000 2950 3900 2950
Wire Wire Line
	3000 3250 5550 3250
Wire Wire Line
	5550 3250 5550 3050
Wire Wire Line
	2050 2500 5450 2500
$Comp
L Connector:Conn_01x01_Male J7
U 1 1 605C467A
P 9750 5150
F 0 "J7" H 9722 5082 50  0000 R CNN
F 1 "-Ve Sense" H 9722 5173 50  0000 R CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad_Via" H 9750 5150 50  0001 C CNN
F 3 "~" H 9750 5150 50  0001 C CNN
	1    9750 5150
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Male J8
U 1 1 605CA6B9
P 9800 1450
F 0 "J8" H 9772 1382 50  0000 R CNN
F 1 "+Ve Sense" H 9772 1473 50  0000 R CNN
F 2 "MountingHole:MountingHole_2.2mm_M2_Pad_Via" H 9800 1450 50  0001 C CNN
F 3 "~" H 9800 1450 50  0001 C CNN
	1    9800 1450
	-1   0    0    1   
$EndComp
$Comp
L Connector:Conn_01x01_Male J9
U 1 1 605CB072
P 9800 1850
F 0 "J9" H 9772 1782 50  0000 R CNN
F 1 "To Fuse 1" H 9772 1873 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 9800 1850 50  0001 C CNN
F 3 "~" H 9800 1850 50  0001 C CNN
	1    9800 1850
	-1   0    0    1   
$EndComp
Wire Wire Line
	4900 1850 4900 2550
Wire Wire Line
	2750 3850 2750 5150
Text HLabel 2050 1450 0    50   Input ~ 0
+veSense
Wire Wire Line
	9600 1450 2050 1450
$Comp
L Connector:Conn_01x01_Male J15
U 1 1 608FDD3D
P 9800 2050
F 0 "J15" H 9772 1982 50  0000 R CNN
F 1 "To Fuse 2" H 9772 2073 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 9800 2050 50  0001 C CNN
F 3 "~" H 9800 2050 50  0001 C CNN
	1    9800 2050
	-1   0    0    1   
$EndComp
Wire Wire Line
	6500 2050 6500 2550
$Comp
L Connector:Conn_01x01_Male J16
U 1 1 60915833
P 9800 4650
F 0 "J16" H 9772 4582 50  0000 R CNN
F 1 "0.22R Shunt" H 9772 4673 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 9800 4650 50  0001 C CNN
F 3 "~" H 9800 4650 50  0001 C CNN
	1    9800 4650
	-1   0    0    1   
$EndComp
Wire Wire Line
	4900 4650 4900 3750
$Comp
L pja_1:IRFP250N Q11
U 1 1 60B4E104
P 8400 2950
F 0 "Q11" H 8650 2992 45  0000 L CNN
F 1 "IXFH80N25X3" H 8650 2908 45  0000 L CNN
F 2 "Package_TO_SOT_THT:TO-247-3_Vertical" H 8430 3100 20  0001 C CNN
F 3 "" H 8400 2950 60  0001 C CNN
	1    8400 2950
	1    0    0    -1  
$EndComp
$Comp
L Connector:Conn_01x01_Male J17
U 1 1 60B53BAB
P 9800 2300
F 0 "J17" H 9772 2232 50  0000 R CNN
F 1 "To Fuse 3" H 9772 2323 50  0000 R CNN
F 2 "MountingHole:MountingHole_3.2mm_M3_Pad_Via" H 9800 2300 50  0001 C CNN
F 3 "~" H 9800 2300 50  0001 C CNN
	1    9800 2300
	-1   0    0    1   
$EndComp
Wire Wire Line
	9600 2300 8400 2300
Wire Wire Line
	8400 2300 8400 2550
Wire Wire Line
	7500 3050 7500 3200
$Comp
L Device:R R43
U 1 1 60B69CFF
P 3200 3500
F 0 "R43" V 2993 3500 50  0000 C CNN
F 1 "2k2" V 3084 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 3130 3500 50  0001 C CNN
F 3 "~" H 3200 3500 50  0001 C CNN
	1    3200 3500
	0    1    1    0   
$EndComp
Wire Wire Line
	3350 3500 7250 3500
Wire Wire Line
	7250 3500 7250 3200
Wire Wire Line
	7250 3200 7500 3200
Connection ~ 2600 3250
Wire Wire Line
	2600 3500 3050 3500
Wire Wire Line
	2600 3250 2600 3500
$Comp
L pspice:INDUCTOR L3
U 1 1 605DE203
P 7750 3050
F 0 "L3" H 7750 3265 50  0000 C CNN
F 1 "2.2uh" H 7750 3174 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 7750 3050 50  0001 C CNN
F 3 "~" H 7750 3050 50  0001 C CNN
	1    7750 3050
	1    0    0    -1  
$EndComp
$Comp
L pspice:INDUCTOR L2
U 1 1 605E5588
P 5800 3050
F 0 "L2" H 5800 3265 50  0000 C CNN
F 1 "2.2uh" H 5800 3174 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 5800 3050 50  0001 C CNN
F 3 "~" H 5800 3050 50  0001 C CNN
	1    5800 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2950 3900 3050
$Comp
L pspice:INDUCTOR L1
U 1 1 605E6032
P 4200 3050
F 0 "L1" H 4200 3265 50  0000 C CNN
F 1 "2.2uh" H 4200 3174 50  0000 C CNN
F 2 "Inductor_SMD:L_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 4200 3050 50  0001 C CNN
F 3 "~" H 4200 3050 50  0001 C CNN
	1    4200 3050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 3050 3950 3050
Connection ~ 6500 4650
Wire Wire Line
	2750 5150 9550 5150
Connection ~ 8400 4650
Wire Wire Line
	6500 3350 6500 4650
Wire Wire Line
	8400 3350 8400 4650
Wire Wire Line
	9600 2050 6500 2050
Wire Wire Line
	4900 1850 9600 1850
Wire Wire Line
	9600 4650 8400 4650
Wire Wire Line
	4900 4650 6500 4650
Wire Wire Line
	6500 4650 8400 4650
$EndSCHEMATC
