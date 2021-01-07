EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "ASM0219"
Date ""
Rev "001"
Comp "CWS"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L display-rescue:display_2.8inch_ili9341-display-rescue DS1
U 1 1 5C28F20B
P 3200 5900
F 0 "DS1" H 4250 6350 60  0000 C CNN
F 1 "display_2.8inch_ili9341" H 4300 5500 60  0000 C CNN
F 2 "PJA_Modules:LCD_2.8_INCH_MSP2807" H 3250 5800 60  0001 C CNN
F 3 "" H 3250 5800 60  0001 C CNN
	1    3200 5900
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:R-display-rescue R1
U 1 1 5C28F4B8
P 6500 2400
F 0 "R1" V 6580 2400 50  0000 C CNN
F 1 "1K" V 6500 2400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6430 2400 50  0001 C CNN
F 3 "" H 6500 2400 50  0001 C CNN
	1    6500 2400
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:R-display-rescue R2
U 1 1 5C28F527
P 6700 2400
F 0 "R2" V 6780 2400 50  0000 C CNN
F 1 "1K" V 6700 2400 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6630 2400 50  0001 C CNN
F 3 "" H 6700 2400 50  0001 C CNN
	1    6700 2400
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:ESP32-WROOM-display-rescue U1
U 1 1 5C2900D1
P 4300 3050
F 0 "U1" H 3600 4300 60  0000 C CNN
F 1 "ESP32-WROOM" H 4800 4300 60  0000 C CNN
F 2 "PJA_Modules:ESP32-WROOM" H 4650 4400 60  0001 C CNN
F 3 "" H 3850 3500 60  0001 C CNN
	1    4300 3050
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:LM1117-3V3-display-rescue U2
U 1 1 5C2A3F84
P 5750 950
F 0 "U2" H 5850 700 50  0000 C CNN
F 1 "LM1117-3V3" H 5750 1200 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-223-3_TabPin2" H 5750 950 50  0001 C CNN
F 3 "" H 5750 950 50  0001 C CNN
	1    5750 950 
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:C-display-rescue C1
U 1 1 5C2A4283
P 5300 1150
F 0 "C1" H 5325 1250 50  0000 L CNN
F 1 "DNF" H 5325 1050 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-6032-20_AVX-F_Pad2.25x2.35mm_HandSolder" H 5338 1000 50  0001 C CNN
F 3 "" H 5300 1150 50  0001 C CNN
	1    5300 1150
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:C-display-rescue C2
U 1 1 5C2A42CE
P 6250 1150
F 0 "C2" H 6275 1250 50  0000 L CNN
F 1 "DNF" H 6275 1050 50  0000 L CNN
F 2 "Capacitor_Tantalum_SMD:CP_EIA-6032-20_AVX-F_Pad2.25x2.35mm_HandSolder" H 6288 1000 50  0001 C CNN
F 3 "" H 6250 1150 50  0001 C CNN
	1    6250 1150
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:C-display-rescue C5
U 1 1 5C2A4397
P 6500 1150
F 0 "C5" H 6525 1250 50  0000 L CNN
F 1 "DNF" H 6525 1050 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 6538 1000 50  0001 C CNN
F 3 "" H 6500 1150 50  0001 C CNN
	1    6500 1150
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:C-display-rescue C3
U 1 1 5C2B0A30
P 1100 5200
F 0 "C3" H 1125 5300 50  0000 L CNN
F 1 "0.1uf" H 1125 5100 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 1138 5050 50  0001 C CNN
F 3 "" H 1100 5200 50  0001 C CNN
	1    1100 5200
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:DTSM-6-display-rescue S2
U 1 1 5C2D0915
P 1000 3300
F 0 "S2" V 715 3200 45  0000 L BNN
F 1 "Reset" V 815 3425 45  0000 L BNN
F 2 "switch-tact:switch-tact-DTS-6" H 1030 3450 20  0001 C CNN
F 3 "" H 1000 3300 60  0001 C CNN
	1    1000 3300
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:DTSM-6-display-rescue S1
U 1 1 5C2D0BA9
P 1000 2350
F 0 "S1" V 715 2250 45  0000 L BNN
F 1 "Setup WiFi" V 815 2475 45  0000 L BNN
F 2 "switch-tact:switch-tact-DTS-6" H 1030 2500 20  0001 C CNN
F 3 "" H 1000 2350 60  0001 C CNN
	1    1000 2350
	1    0    0    -1  
$EndComp
Text Notes 7450 5850 0    60   ~ 0
CS0 (DISPLAY)
Text Notes 7450 5950 0    60   ~ 0
CS1 (TOUCH SCREEN)
Text Notes 7450 6050 0    60   ~ 0
CS2 (SD CARD)
$Comp
L display-rescue:R-display-rescue R3
U 1 1 5C38339F
P 1700 2000
F 0 "R3" V 1780 2000 50  0000 C CNN
F 1 "10K" V 1700 2000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1630 2000 50  0001 C CNN
F 3 "" H 1700 2000 50  0001 C CNN
	1    1700 2000
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R4
U 1 1 5C38341C
P 1700 2750
F 0 "R4" V 1780 2750 50  0000 C CNN
F 1 "470R" V 1700 2750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 1630 2750 50  0001 C CNN
F 3 "" H 1700 2750 50  0001 C CNN
	1    1700 2750
	1    0    0    -1  
$EndComp
Text Label 3700 1500 0    60   ~ 0
3V3
Text Label 5550 4250 0    60   ~ 0
GND
$Comp
L display-rescue:C-display-rescue C4
U 1 1 5C383FDC
P 2550 2900
F 0 "C4" H 2575 3000 50  0000 L CNN
F 1 "0.1uf" H 2575 2800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric_Pad1.05x0.95mm_HandSolder" H 2588 2750 50  0001 C CNN
F 3 "" H 2550 2900 50  0001 C CNN
	1    2550 2900
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:usb_bat_pwr_module-display-rescue M2
U 1 1 5C3DE854
P 9550 1500
F 0 "M2" H 9150 1400 60  0000 C CNN
F 1 "usb_bat_pwr_module" V 8850 1350 60  0000 C CNN
F 2 "PJA_Modules:USB_BAT_PWR_MODULE" H 9550 1500 60  0001 C CNN
F 3 "" H 9550 1500 60  0001 C CNN
	1    9550 1500
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:DTSM-6-display-rescue S3
U 1 1 5C3E3E5A
P 10750 1650
F 0 "S3" V 10465 1550 45  0000 L BNN
F 1 "Power On/Off" V 10565 1775 45  0000 L BNN
F 2 "switch-tact:switch-tact-DTS-6" H 10780 1800 20  0001 C CNN
F 3 "" H 10750 1650 60  0001 C CNN
	1    10750 1650
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:R-display-rescue R7
U 1 1 5C3E4652
P 6200 600
F 0 "R7" V 6280 600 50  0000 C CNN
F 1 "DNF" V 6200 600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 6130 600 50  0001 C CNN
F 3 "" H 6200 600 50  0001 C CNN
	1    6200 600 
	0    1    1    0   
$EndComp
$Comp
L display-rescue:Conn_01x02-display-rescue J3
U 1 1 5C3E4D3C
P 10800 2350
F 0 "J3" H 10800 2450 50  0000 C CNN
F 1 "Lion Bat" H 10800 2150 50  0000 C CNN
F 2 "Connector_JST:JST_EH_B2B-EH-A_1x02_P2.50mm_Vertical" H 10800 2350 50  0001 C CNN
F 3 "" H 10800 2350 50  0001 C CNN
	1    10800 2350
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:LED-display-rescue D2
U 1 1 5C3E53D0
P 10250 3100
F 0 "D2" H 10250 3200 50  0000 C CNN
F 1 "LED" H 10250 3000 50  0000 C CNN
F 2 "LED_SMD:LED_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 10250 3100 50  0001 C CNN
F 3 "" H 10250 3100 50  0001 C CNN
	1    10250 3100
	0    -1   -1   0   
$EndComp
$Comp
L display-rescue:R-display-rescue R8
U 1 1 5C3E5625
P 10250 2700
F 0 "R8" V 10330 2700 50  0000 C CNN
F 1 "470R" V 10250 2700 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 10180 2700 50  0001 C CNN
F 3 "" H 10250 2700 50  0001 C CNN
	1    10250 2700
	-1   0    0    1   
$EndComp
Text Notes 3650 700  0    60   ~ 0
LM1117 and 3 caps not fitted
$Comp
L display-rescue:R-display-rescue R5
U 1 1 5C3E7861
P 2700 3850
F 0 "R5" V 2780 3850 50  0000 C CNN
F 1 "DNF" V 2700 3850 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2630 3850 50  0001 C CNN
F 3 "" H 2700 3850 50  0001 C CNN
	1    2700 3850
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:LED-display-rescue D1
U 1 1 5C3E8A0F
P 5750 4000
F 0 "D1" H 5750 4100 50  0000 C CNN
F 1 "LED" H 5750 3900 50  0000 C CNN
F 2 "LED_SMD:LED_1210_3225Metric_Pad1.42x2.65mm_HandSolder" H 5750 4000 50  0001 C CNN
F 3 "" H 5750 4000 50  0001 C CNN
	1    5750 4000
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R6
U 1 1 5C3E8C16
P 5400 4000
F 0 "R6" V 5480 4000 50  0000 C CNN
F 1 "470R" V 5400 4000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 5330 4000 50  0001 C CNN
F 3 "" H 5400 4000 50  0001 C CNN
	1    5400 4000
	0    -1   -1   0   
$EndComp
NoConn ~ 9750 1900
NoConn ~ 9750 2000
NoConn ~ 9750 2100
NoConn ~ 3350 2750
NoConn ~ 3350 2850
NoConn ~ 3350 2950
NoConn ~ 3350 3050
NoConn ~ 3950 4100
NoConn ~ 4050 4100
NoConn ~ 4150 4100
NoConn ~ 4250 4100
NoConn ~ 4350 4100
NoConn ~ 4450 4100
NoConn ~ 4550 4100
NoConn ~ 4650 4100
NoConn ~ 10250 2750
$Comp
L display-rescue:R-display-rescue R9
U 1 1 5C4159BB
P 9950 3050
F 0 "R9" V 10030 3050 50  0000 C CNN
F 1 "68K" V 9950 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9880 3050 50  0001 C CNN
F 3 "" H 9950 3050 50  0001 C CNN
	1    9950 3050
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R10
U 1 1 5C415AF9
P 9950 3550
F 0 "R10" V 10030 3550 50  0000 C CNN
F 1 "100K" V 9950 3550 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9880 3550 50  0001 C CNN
F 3 "" H 9950 3550 50  0001 C CNN
	1    9950 3550
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R11
U 1 1 5C419651
P 9750 3050
F 0 "R11" V 9830 3050 50  0000 C CNN
F 1 "68K" V 9750 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9680 3050 50  0001 C CNN
F 3 "" H 9750 3050 50  0001 C CNN
	1    9750 3050
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R12
U 1 1 5C4196F9
P 9650 3750
F 0 "R12" V 9730 3750 50  0000 C CNN
F 1 "100K" V 9650 3750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9580 3750 50  0001 C CNN
F 3 "" H 9650 3750 50  0001 C CNN
	1    9650 3750
	-1   0    0    1   
$EndComp
$Comp
L display-rescue:R-display-rescue R13
U 1 1 5C41D097
P 2100 2850
F 0 "R13" V 2180 2850 50  0000 C CNN
F 1 "47K" V 2100 2850 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 2030 2850 50  0001 C CNN
F 3 "" H 2100 2850 50  0001 C CNN
	1    2100 2850
	1    0    0    -1  
$EndComp
$Comp
L display-rescue:R-display-rescue R14
U 1 1 5C41DA26
P 5650 3450
F 0 "R14" V 5550 3450 50  0000 C CNN
F 1 "0R" V 5650 3450 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 5580 3450 50  0001 C CNN
F 3 "" H 5650 3450 50  0001 C CNN
	1    5650 3450
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6050 1400 6050 4000
Wire Wire Line
	1100 4250 2550 4250
Wire Wire Line
	6050 4700 2300 4700
Wire Wire Line
	2300 4700 2300 5350
Wire Wire Line
	1100 5350 2300 5350
Connection ~ 6050 4250
Wire Wire Line
	2400 5250 3000 5250
Wire Wire Line
	2400 1500 2400 2450
Wire Wire Line
	2400 2450 2550 2450
Wire Wire Line
	6900 950  6900 1500
Wire Wire Line
	1700 1500 2400 1500
Connection ~ 2400 2450
Wire Wire Line
	6100 2550 6100 2650
Wire Wire Line
	5200 2550 6100 2550
Wire Wire Line
	5200 2850 6700 2850
Wire Wire Line
	6500 1500 6500 2250
Connection ~ 6500 1500
Wire Wire Line
	6700 1500 6700 2250
Connection ~ 6700 1500
Wire Wire Line
	6500 2550 6500 2650
Connection ~ 6500 2650
Wire Wire Line
	6700 2550 6700 2850
Connection ~ 6700 2850
Wire Wire Line
	2100 5550 3000 5550
Wire Wire Line
	2100 3000 2100 4800
Wire Wire Line
	2100 4800 6150 4800
Wire Wire Line
	6150 4800 6150 3450
Wire Wire Line
	2000 3150 3350 3150
Wire Wire Line
	2000 3150 2000 5650
Wire Wire Line
	1700 5950 1700 4950
Wire Wire Line
	1700 4950 6300 4950
Wire Wire Line
	6300 4950 6300 3350
Wire Wire Line
	6300 3350 5200 3350
Wire Wire Line
	3850 4250 3850 4100
Wire Wire Line
	1450 6550 3000 6550
Wire Wire Line
	1450 3250 1450 6550
Wire Wire Line
	2000 5650 3000 5650
Wire Wire Line
	3000 5950 1700 5950
Wire Wire Line
	3000 5450 2200 5450
Wire Wire Line
	2200 5450 2200 4750
Wire Wire Line
	2200 4750 6100 4750
Wire Wire Line
	6100 3250 6100 4750
Wire Wire Line
	6100 3250 5200 3250
Wire Wire Line
	1900 5750 2550 5750
Wire Wire Line
	1900 5750 1900 4850
Wire Wire Line
	1900 4850 6200 4850
Wire Wire Line
	6200 4850 6200 2450
Wire Wire Line
	6200 2450 5200 2450
Wire Wire Line
	1800 5850 2500 5850
Wire Wire Line
	1800 5850 1800 4900
Wire Wire Line
	1800 4900 6250 4900
Wire Wire Line
	6250 4900 6250 3150
Wire Wire Line
	6250 3150 5200 3150
Wire Wire Line
	1600 6050 2450 6050
Wire Wire Line
	1600 6050 1600 5000
Wire Wire Line
	1600 5000 6350 5000
Wire Wire Line
	6350 5000 6350 3050
Wire Wire Line
	6350 3050 5200 3050
Wire Wire Line
	5150 600  5150 950 
Wire Wire Line
	5150 950  5300 950 
Wire Wire Line
	6050 950  6250 950 
Wire Wire Line
	5300 1400 5750 1400
Wire Wire Line
	5750 1400 5750 1250
Wire Wire Line
	5300 1300 5300 1400
Connection ~ 5750 1400
Wire Wire Line
	6250 1400 6250 1300
Connection ~ 6050 1400
Wire Wire Line
	6500 1400 6500 1300
Connection ~ 6250 1400
Wire Wire Line
	6250 1000 6250 950 
Connection ~ 6250 950 
Wire Wire Line
	6500 950  6500 1000
Connection ~ 6500 950 
Wire Wire Line
	5300 1000 5300 950 
Connection ~ 5300 950 
Connection ~ 2300 5350
Wire Wire Line
	1100 5050 1100 4550
Wire Wire Line
	1100 4550 2400 4550
Connection ~ 2400 4550
Wire Wire Line
	1100 4250 1100 3500
Connection ~ 3850 4250
Wire Wire Line
	1000 3500 1100 3500
Wire Wire Line
	1000 2550 1100 2550
Wire Wire Line
	1200 2550 1200 3500
Connection ~ 1100 3500
Connection ~ 1100 2550
Wire Wire Line
	1000 2150 1100 2150
Wire Wire Line
	1250 2150 1250 4150
Wire Wire Line
	1250 4150 2700 4150
Wire Wire Line
	5200 4150 5200 3650
Connection ~ 1100 2150
Wire Wire Line
	1450 3250 2700 3250
Wire Wire Line
	1000 3100 1100 3100
Connection ~ 1100 3100
Wire Wire Line
	1500 6250 2300 6250
Wire Wire Line
	1500 6250 1500 3450
Wire Wire Line
	1500 3450 3350 3450
Wire Wire Line
	5500 5750 5750 5750
Wire Wire Line
	5750 5750 5750 4600
Wire Wire Line
	3050 4600 5750 4600
Wire Wire Line
	3050 4600 3050 3550
Wire Wire Line
	3050 3550 3350 3550
Wire Wire Line
	2550 5100 2550 5750
Wire Wire Line
	2550 5100 5850 5100
Wire Wire Line
	5850 5100 5850 5500
Wire Wire Line
	5850 5850 5500 5850
Connection ~ 2550 5750
Wire Wire Line
	5900 5950 5500 5950
Wire Wire Line
	5900 5050 5900 5600
Wire Wire Line
	5900 5050 2450 5050
Wire Wire Line
	2450 5050 2450 6050
Connection ~ 2450 6050
Wire Wire Line
	2500 4650 2500 5850
Wire Wire Line
	2500 4650 5950 4650
Wire Wire Line
	5950 4650 5950 5700
Wire Wire Line
	5950 6050 5500 6050
Connection ~ 2500 5850
Wire Wire Line
	7200 5500 5850 5500
Connection ~ 5850 5500
Wire Wire Line
	7200 5600 5900 5600
Connection ~ 5900 5600
Wire Wire Line
	7200 5700 5950 5700
Connection ~ 5950 5700
Wire Wire Line
	6100 5800 7200 5800
Connection ~ 6100 4750
Wire Wire Line
	2500 6150 3000 6150
Wire Wire Line
	2450 6450 3000 6450
Wire Wire Line
	2550 6350 3000 6350
Wire Wire Line
	2300 6250 2300 6750
Wire Wire Line
	2300 6750 6100 6750
Wire Wire Line
	6100 6750 6100 5900
Wire Wire Line
	6100 5900 7200 5900
Connection ~ 2300 6250
Wire Wire Line
	6400 4600 6400 6000
Wire Wire Line
	6400 6000 7200 6000
Connection ~ 5750 4600
Wire Wire Line
	1700 1500 1700 1850
Connection ~ 2400 1500
Wire Wire Line
	1700 2150 1700 2550
Wire Wire Line
	3350 2550 1700 2550
Connection ~ 1700 2550
Wire Wire Line
	1700 3100 1700 2900
Wire Wire Line
	2550 2750 2550 2450
Connection ~ 2550 2450
Wire Wire Line
	2550 3050 2550 4250
Connection ~ 2550 4250
Wire Wire Line
	10050 600  10050 1200
Wire Wire Line
	10050 1200 9750 1200
Wire Wire Line
	9750 1500 10150 1500
Wire Wire Line
	10350 1850 10750 1850
Wire Wire Line
	10350 1850 10350 1700
Wire Wire Line
	10350 1700 9750 1700
Connection ~ 10750 1850
Wire Wire Line
	10350 1450 10750 1450
Wire Wire Line
	10350 1450 10350 1600
Wire Wire Line
	10350 1600 9750 1600
Connection ~ 10750 1450
Wire Wire Line
	10100 1400 10100 750 
Wire Wire Line
	10100 750  8000 750 
Wire Wire Line
	8000 750  8000 950 
Connection ~ 6900 950 
Wire Wire Line
	10300 1500 10300 2350
Connection ~ 10150 1500
Wire Wire Line
	9750 2450 10250 2450
Wire Wire Line
	10250 1300 10250 2450
Wire Wire Line
	9750 1400 10100 1400
Wire Wire Line
	9750 1300 10250 1300
Connection ~ 10250 2450
Wire Wire Line
	10250 2850 10250 2950
Wire Wire Line
	10100 3250 10250 3250
Wire Wire Line
	10100 1800 10100 2800
Wire Wire Line
	10100 1800 9750 1800
Wire Wire Line
	8400 1400 8400 950 
Wire Wire Line
	8400 950  10150 950 
Connection ~ 6500 1400
Wire Wire Line
	10150 950  10150 1500
Wire Wire Line
	3350 2350 3250 2350
Wire Wire Line
	3250 2350 3250 1650
Wire Wire Line
	3250 1650 5300 1650
Connection ~ 5300 1400
Wire Wire Line
	5300 1900 5200 1900
Connection ~ 5300 1650
Wire Wire Line
	5300 2350 5200 2350
Connection ~ 5300 1900
Wire Wire Line
	2700 3700 2700 3250
Connection ~ 2700 3250
Wire Wire Line
	2700 4000 2700 4150
Connection ~ 2700 4150
Wire Wire Line
	4750 4100 5000 4100
Wire Wire Line
	5000 4100 5000 4000
Wire Wire Line
	5000 4000 5250 4000
Wire Wire Line
	6050 600  5150 600 
Wire Wire Line
	5200 2950 5900 2950
Wire Wire Line
	5900 2950 5900 4350
Wire Wire Line
	5900 4350 9350 4350
Wire Wire Line
	9350 4350 9350 3300
Wire Wire Line
	9350 3300 9950 3300
Wire Wire Line
	9950 3200 9950 3300
Connection ~ 9950 3300
Wire Wire Line
	9950 2900 9950 2800
Wire Wire Line
	9950 2800 10100 2800
Connection ~ 10100 2800
Wire Wire Line
	9950 4250 9950 3700
Wire Wire Line
	9500 3200 9650 3200
Wire Wire Line
	9650 3200 9650 3600
Wire Wire Line
	9650 3900 9650 4250
Connection ~ 9650 4250
Wire Wire Line
	9750 2450 9750 2900
Wire Wire Line
	9500 3200 9500 4400
Wire Wire Line
	9500 4400 2900 4400
Wire Wire Line
	2900 4400 2900 2650
Wire Wire Line
	2900 2650 3350 2650
Connection ~ 9650 3200
Connection ~ 2100 4800
Wire Wire Line
	2100 2700 2100 2650
Wire Wire Line
	2100 2650 2400 2650
Connection ~ 2400 2650
Wire Wire Line
	6150 3450 5800 3450
Wire Wire Line
	5500 3450 5200 3450
Wire Wire Line
	6100 2650 6500 2650
Wire Wire Line
	5550 4000 5600 4000
Wire Wire Line
	5900 4000 6050 4000
Connection ~ 6050 4000
Wire Wire Line
	10300 2350 10600 2350
Text Label 10450 2450 0    60   ~ 0
BAT+
Text Label 10450 2350 0    60   ~ 0
GND
Text Label 9750 1200 0    60   ~ 0
5VBAT
Text Notes 7250 6850 0    60   ~ 0
ASM0219V001 assembly
Wire Wire Line
	6050 4250 6050 4700
Wire Wire Line
	2400 2450 2400 2650
Wire Wire Line
	6500 1500 6700 1500
Wire Wire Line
	6700 1500 6900 1500
Wire Wire Line
	6500 2650 7000 2650
Wire Wire Line
	6700 2850 7000 2850
Wire Wire Line
	5750 1400 6050 1400
Wire Wire Line
	6050 1400 6250 1400
Wire Wire Line
	6250 1400 6500 1400
Wire Wire Line
	6250 950  6500 950 
Wire Wire Line
	6500 950  6900 950 
Wire Wire Line
	5300 950  5450 950 
Wire Wire Line
	2300 5350 3000 5350
Wire Wire Line
	2400 4550 2400 5250
Wire Wire Line
	3850 4250 6050 4250
Wire Wire Line
	1100 3500 1200 3500
Wire Wire Line
	1100 2550 1200 2550
Wire Wire Line
	1100 2150 1250 2150
Wire Wire Line
	1100 3100 1700 3100
Wire Wire Line
	2550 5750 3000 5750
Wire Wire Line
	2550 5750 2550 6350
Wire Wire Line
	2450 6050 3000 6050
Wire Wire Line
	2450 6050 2450 6450
Wire Wire Line
	2500 5850 3000 5850
Wire Wire Line
	2500 5850 2500 6150
Wire Wire Line
	5850 5500 5850 5850
Wire Wire Line
	5900 5600 5900 5950
Wire Wire Line
	5950 5700 5950 6050
Wire Wire Line
	6100 4750 6100 5800
Wire Wire Line
	2300 6250 3000 6250
Wire Wire Line
	5750 4600 6400 4600
Wire Wire Line
	2400 1500 6500 1500
Wire Wire Line
	1700 2550 1700 2600
Wire Wire Line
	2550 2450 3350 2450
Wire Wire Line
	2550 4250 3850 4250
Wire Wire Line
	10750 1850 10850 1850
Wire Wire Line
	10750 1450 10850 1450
Wire Wire Line
	6900 950  8000 950 
Wire Wire Line
	10150 1500 10300 1500
Wire Wire Line
	10250 2450 10600 2450
Wire Wire Line
	10250 2450 10250 2550
Wire Wire Line
	5300 1400 5300 1650
Wire Wire Line
	5300 1650 5300 1900
Wire Wire Line
	5300 1900 5300 2350
Wire Wire Line
	2700 3250 3350 3250
Wire Wire Line
	2700 4150 5200 4150
Wire Wire Line
	9950 3300 9950 3400
Wire Wire Line
	10100 2800 10100 3250
Wire Wire Line
	9650 4250 9950 4250
Wire Wire Line
	9650 3200 9750 3200
Wire Wire Line
	2100 4800 2100 5550
Wire Wire Line
	2400 2650 2400 4550
$Comp
L Connector:Conn_01x04_Male J6
U 1 1 5FCABF5A
P 10750 4450
F 0 "J6" H 10722 4378 50  0000 R CNN
F 1 "I2C" H 10858 4640 50  0001 C CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 10750 4450 50  0001 C CNN
F 3 "~" H 10750 4450 50  0001 C CNN
	1    10750 4450
	-1   0    0    1   
$EndComp
Text GLabel 1550 1500 0    60   Input ~ 0
3V3
Text GLabel 10550 4250 0    60   Input ~ 0
3V3
Text GLabel 900  4250 0    60   Input ~ 0
GND
Text GLabel 10550 4550 0    60   Input ~ 0
GND
Text GLabel 7000 2650 2    60   Input ~ 0
SDA
Text GLabel 7000 2850 2    60   Input ~ 0
SCL
Text GLabel 10550 4450 0    60   Input ~ 0
SDA
Text GLabel 10550 4350 0    60   Input ~ 0
SCL
Text Notes 10500 4750 0    60   ~ 0
I2C
Wire Wire Line
	6350 600  10050 600 
Wire Wire Line
	6500 1400 8400 1400
Text GLabel 5200 2650 2    60   Input ~ 0
TXD
Text GLabel 5200 2750 2    60   Input ~ 0
RXD
$Comp
L Connector:Conn_01x03_Male J5
U 1 1 5FCF10AC
P 10700 3650
F 0 "J5" H 10672 3628 50  0000 R CNN
F 1 "Serial" H 10850 3850 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B3B-EH-A_1x03_P2.50mm_Vertical" H 10700 3650 50  0001 C CNN
F 3 "~" H 10700 3650 50  0001 C CNN
	1    10700 3650
	-1   0    0    1   
$EndComp
Text GLabel 10500 3750 0    60   Input ~ 0
GND
Text GLabel 10500 3650 0    60   Input ~ 0
TXD
Text GLabel 10500 3550 0    60   Input ~ 0
RXD
$Comp
L Connector:Conn_01x04_Male J7
U 1 1 5FCF8471
P 10800 5150
F 0 "J7" H 10772 5078 50  0000 R CNN
F 1 "SEDI" H 11000 5350 50  0000 R CNN
F 2 "Connector_JST:JST_EH_B4B-EH-A_1x04_P2.50mm_Vertical" H 10800 5150 50  0001 C CNN
F 3 "~" H 10800 5150 50  0001 C CNN
	1    10800 5150
	-1   0    0    1   
$EndComp
Text Notes 10450 5500 0    60   ~ 0
DISPLAY
Wire Wire Line
	6050 4000 6050 4250
Text GLabel 10600 5250 0    60   Input ~ 0
GND
Text GLabel 3350 3350 0    60   Input ~ 0
DTX
Text GLabel 5200 3550 2    60   Input ~ 0
DRX
Text GLabel 10600 5150 0    60   Input ~ 0
DTX
Text GLabel 10600 5050 0    60   Input ~ 0
DRX
Wire Wire Line
	10600 4950 10000 4950
Text GLabel 9600 4950 0    60   Input ~ 0
3V3
Text GLabel 4950 950  0    60   Input ~ 0
5V
Wire Wire Line
	4950 950  5150 950 
Connection ~ 5150 950 
Wire Wire Line
	1550 1500 1700 1500
Connection ~ 1700 1500
Wire Wire Line
	900  4250 1100 4250
Connection ~ 1100 4250
$Comp
L Device:R R16
U 1 1 5FD8811C
P 9750 4950
F 0 "R16" V 9543 4950 50  0000 C CNN
F 1 "0R" V 9634 4950 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9680 4950 50  0001 C CNN
F 3 "~" H 9750 4950 50  0001 C CNN
	1    9750 4950
	0    1    1    0   
$EndComp
Text GLabel 9600 4600 0    60   Input ~ 0
5V
Wire Wire Line
	9900 4600 10000 4600
Wire Wire Line
	10000 4600 10000 4950
Connection ~ 10000 4950
Wire Wire Line
	10000 4950 9900 4950
$Comp
L Device:R R15
U 1 1 5FD88BE9
P 9750 4600
F 0 "R15" V 9543 4600 50  0000 C CNN
F 1 "0R" V 9634 4600 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric_Pad1.05x0.95mm_HandSolder" V 9680 4600 50  0001 C CNN
F 3 "~" H 9750 4600 50  0001 C CNN
	1    9750 4600
	0    1    1    0   
$EndComp
Text GLabel 7200 5500 2    60   Input ~ 0
MOSI
Text GLabel 7200 5600 2    60   Input ~ 0
MISO
Text GLabel 7200 5700 2    60   Input ~ 0
SCK
Text GLabel 7200 6000 2    60   Input ~ 0
CS2
Wire Wire Line
	6050 4250 9650 4250
$Comp
L Connector_Generic:Conn_02x04_Odd_Even J4
U 1 1 5FDAB679
P 10450 5850
F 0 "J4" H 10500 6075 50  0000 C CNN
F 1 "SPI" H 10500 5500 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x04_P2.54mm_Vertical" H 10450 5850 50  0001 C CNN
F 3 "~" H 10450 5850 50  0001 C CNN
	1    10450 5850
	1    0    0    -1  
$EndComp
Text GLabel 10250 6050 0    60   Input ~ 0
SCK
Text GLabel 10250 5950 0    60   Input ~ 0
MOSI
Text GLabel 10250 5850 0    60   Input ~ 0
MISO
Text GLabel 10250 5750 0    60   Input ~ 0
3V3
Text GLabel 10750 5750 2    60   Input ~ 0
5V
Text GLabel 10750 5850 2    60   Input ~ 0
CS2
Text GLabel 3350 3650 0    60   Input ~ 0
INT
Text GLabel 10750 5950 2    60   Input ~ 0
INT
Text GLabel 10750 6050 2    60   Input ~ 0
GND
$Comp
L Connector:Conn_01x04_Male J1
U 1 1 5FE707D4
P 9400 5950
F 0 "J1" H 9372 5878 50  0000 R CNN
F 1 "Conn_01x04_Male" H 9372 5923 50  0001 R CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x04_P2.54mm_Vertical" H 9400 5950 50  0001 C CNN
F 3 "~" H 9400 5950 50  0001 C CNN
	1    9400 5950
	-1   0    0    1   
$EndComp
Text GLabel 9200 5850 0    60   Input ~ 0
MOSI
Text GLabel 9200 5950 0    60   Input ~ 0
MISO
Text GLabel 9200 5750 0    60   Input ~ 0
CS2
Text GLabel 9200 6050 0    60   Input ~ 0
SCK
Text GLabel 7200 5900 2    60   Input ~ 0
CS1
Text GLabel 7200 5800 2    60   Input ~ 0
CS0
Text Notes 8850 6300 0    60   ~ 0
3.5" Display\nSDCard
$EndSCHEMATC
