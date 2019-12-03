EESchema Schematic File Version 5
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
$Comp
L Interface_USB:USB2514B_Bi U1
U 1 1 5DE66101
P 3300 3100
F 0 "U1" H 3300 1511 50  0000 C CNN
F 1 "USB2514B_Bi" H 3300 1420 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-36-1EP_6x6mm_P0.5mm_EP3.7x3.7mm" H 4600 1600 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/00001692C.pdf" H 4900 1500 50  0001 C CNN
	1    3300 3100
	1    0    0    -1  
$EndComp
$Comp
L Interface_USB:USB2514B_Bi U2
U 1 1 5DE90D5E
P 8700 5800
F 0 "U2" H 8700 4211 50  0000 C CNN
F 1 "USB2514B_Bi" H 8700 4120 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-36-1EP_6x6mm_P0.5mm_EP3.7x3.7mm" H 10000 4300 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/00001692C.pdf" H 10300 4200 50  0001 C CNN
	1    8700 5800
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_A J3
U 1 1 5DE6A44E
P 5400 5600
F 0 "J3" H 5170 5589 50  0000 R CNN
F 1 "USB_A (male down)" H 5170 5498 50  0000 R CNN
F 2 "Connector_USB:USB_A_Stewart_SS-52100-001_Horizontal" H 5550 5550 50  0001 C CNN
F 3 " ~" H 5550 5550 50  0001 C CNN
	1    5400 5600
	-1   0    0    -1  
$EndComp
Wire Wire Line
	5100 5700 4700 5700
Text Notes 4000 7000 0    50   ~ 0
Male A plug for stacking sideways, paralleled with normal A female for downstream
Wire Wire Line
	5100 6500 4800 6500
Wire Wire Line
	4700 5700 4700 6600
Wire Wire Line
	4700 6600 5100 6600
$Comp
L Connector:USB_A J1
U 1 1 5DE70389
P 900 2100
F 0 "J1" H 957 2567 50  0000 C CNN
F 1 "USB_A (female up)" H 957 2476 50  0000 C CNN
F 2 "Connector_USB:USB_A_Receptacle_GENERIC" H 1050 2050 50  0001 C CNN
F 3 " ~" H 1050 2050 50  0001 C CNN
	1    900  2100
	1    0    0    -1  
$EndComp
$Comp
L Connector:USB_B_Micro J2
U 1 1 5DE72467
P 900 3100
F 0 "J2" H 957 3567 50  0000 C CNN
F 1 "USB_B_Micro (up)" H 957 3476 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Amphenol_10103594-0001LF_Horizontal" H 1050 3050 50  0001 C CNN
F 3 "~" H 1050 3050 50  0001 C CNN
	1    900  3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 2500 1600 2500
Wire Wire Line
	1600 2500 1600 2100
Wire Wire Line
	1600 2100 1200 2100
Wire Wire Line
	1600 2500 1600 3100
Wire Wire Line
	1600 3100 1200 3100
Connection ~ 1600 2500
Wire Wire Line
	1200 3200 1700 3200
Wire Wire Line
	1700 3200 1700 2400
Wire Wire Line
	1700 2400 2200 2400
Wire Wire Line
	1700 2400 1700 2200
Wire Wire Line
	1700 2200 1200 2200
Connection ~ 1700 2400
Text Notes 600  1500 0    50   ~ 0
usb A female for plug stacking\n+\nusb b micro for cable connecting
$Comp
L Connector:USB_A J4
U 1 1 5DE73E78
P 5400 6500
F 0 "J4" H 5170 6489 50  0000 R CNN
F 1 "USB_A (female down)" H 5170 6398 50  0000 R CNN
F 2 "Connector_USB:USB_A_Receptacle_GENERIC" H 5550 6450 50  0001 C CNN
F 3 " ~" H 5550 6450 50  0001 C CNN
	1    5400 6500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	4800 6500 4800 5600
Wire Wire Line
	4800 5600 5100 5600
Connection ~ 4800 5600
Text Notes 8200 1600 0    50   ~ 0
Each pair a Double stack USB port\nC12049 on lcsc or similar
Text Notes 1400 7200 0    50   ~ 0
Screw terminals for 5-12V input?\nEven allow bus powered operation?\nif these are powering DUTs, we could never rely\non enough upstream power for this surely.
$Comp
L Connector:USB_A_DUAL J5
U 1 1 5DE8C1BA
P 7000 2400
F 0 "J5" H 6770 2139 50  0000 R CNN
F 1 "USB_A_DUAL" H 6770 2048 50  0000 R CNN
F 2 "Connector_USB:USB_A_Wuerth_61400826021_Horizontal_Stacked" H 7150 2350 50  0001 C CNN
F 3 " ~" H 7150 2350 50  0001 C CNN
	1    7000 2400
	-1   0    0    -1  
$EndComp
$Comp
L Connector:USB_A_DUAL J6
U 1 1 5DE90D5F
P 11700 5000
F 0 "J6" H 11470 4739 50  0000 R CNN
F 1 "USB_A_DUAL" H 11470 4648 50  0000 R CNN
F 2 "Connector_USB:USB_A_Wuerth_61400826021_Horizontal_Stacked" H 11850 4950 50  0001 C CNN
F 3 " ~" H 11850 4950 50  0001 C CNN
	1    11700 5000
	-1   0    0    -1  
$EndComp
$Comp
L Connector:USB_A_DUAL J7
U 1 1 5DE8FE6F
P 11700 6600
F 0 "J7" H 11470 6339 50  0000 R CNN
F 1 "USB_A_DUAL" H 11470 6248 50  0000 R CNN
F 2 "Connector_USB:USB_A_Wuerth_61400826021_Horizontal_Stacked" H 11850 6550 50  0001 C CNN
F 3 " ~" H 11850 6550 50  0001 C CNN
	1    11700 6600
	-1   0    0    -1  
$EndComp
Text Notes 9400 3000 0    50   ~ 0
Dual Leds, power port controllers for each one too please\nAP2142/52  or 46/56 for duals\n(duals are cheaper than two singles)\n
$Comp
L Device:R_Small R1
U 1 1 5DE96E4C
P 2000 2000
F 0 "R1" V 1804 2000 50  0000 C CNN
F 1 "100k" V 1895 2000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2000 2000 50  0001 C CNN
F 3 "~" H 2000 2000 50  0001 C CNN
	1    2000 2000
	0    1    1    0   
$EndComp
$Comp
L Device:R_Small R2
U 1 1 5DE9B0E4
P 2000 2200
F 0 "R2" V 1804 2200 50  0000 C CNN
F 1 "100k" V 1895 2200 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2000 2200 50  0001 C CNN
F 3 "~" H 2000 2200 50  0001 C CNN
	1    2000 2200
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR01
U 1 1 5DE98444
P 1800 2200
F 0 "#PWR01" H 1800 1950 50  0001 C CNN
F 1 "GND" H 1805 2027 50  0000 C CNN
F 2 "" H 1800 2200 50  0001 C CNN
F 3 "" H 1800 2200 50  0001 C CNN
	1    1800 2200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2200 2200 2100 2200
Wire Wire Line
	2100 2200 2100 2000
Connection ~ 2100 2200
Wire Wire Line
	1900 2200 1800 2200
Wire Wire Line
	1900 2000 1800 2000
Wire Wire Line
	1800 2000 1800 1900
Wire Wire Line
	1800 1900 1400 1900
Wire Wire Line
	1200 2900 1400 2900
Wire Wire Line
	1400 2900 1400 1900
Connection ~ 1400 1900
Wire Wire Line
	1400 1900 1200 1900
$Comp
L Device:Crystal_GND23_Small Y1
U 1 1 5DE98EB7
P 1600 3500
F 0 "Y1" V 1554 3644 50  0000 L CNN
F 1 "Crystal_GND23_Small" V 1645 3644 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm" H 1600 3500 50  0001 C CNN
F 3 "~" H 1600 3500 50  0001 C CNN
	1    1600 3500
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR02
U 1 1 5DE999FC
P 1800 3000
F 0 "#PWR02" H 1800 2750 50  0001 C CNN
F 1 "GND" H 1805 2827 50  0000 C CNN
F 2 "" H 1800 3000 50  0001 C CNN
F 3 "" H 1800 3000 50  0001 C CNN
	1    1800 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R3
U 1 1 5DE99A0A
P 2000 3000
F 0 "R3" V 1804 3000 50  0000 C CNN
F 1 "12k" V 1895 3000 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 2000 3000 50  0001 C CNN
F 3 "~" H 2000 3000 50  0001 C CNN
	1    2000 3000
	0    1    1    0   
$EndComp
Wire Wire Line
	2100 3000 2200 3000
Wire Wire Line
	1900 3000 1800 3000
Wire Wire Line
	2200 3200 1900 3200
Wire Wire Line
	1900 3200 1900 3400
Wire Wire Line
	1900 3400 1600 3400
Wire Wire Line
	1600 3600 2000 3600
Wire Wire Line
	2000 3600 2000 3300
Wire Wire Line
	2000 3300 2200 3300
$Comp
L Connector:Screw_Terminal_01x02 J8
U 1 1 5DE9BF5A
P 1700 7400
F 0 "J8" H 1780 7392 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 1780 7301 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 1700 7400 50  0001 C CNN
F 3 "~" H 1700 7400 50  0001 C CNN
	1    1700 7400
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J9
U 1 1 5DE9F396
P 1700 7900
F 0 "J9" H 1780 7892 50  0000 L CNN
F 1 "Screw_Terminal_01x02" H 1780 7801 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 1700 7900 50  0001 C CNN
F 3 "~" H 1700 7900 50  0001 C CNN
	1    1700 7900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4800 3800 4400 3800
Wire Wire Line
	4800 3800 4800 5600
Wire Wire Line
	4400 3700 4700 3700
Wire Wire Line
	4700 3700 4700 5700
Connection ~ 4700 5700
Wire Wire Line
	4400 3300 6000 3300
Wire Wire Line
	6000 3300 6000 5200
Wire Wire Line
	6000 5200 7600 5200
Wire Wire Line
	7600 5100 6100 5100
Wire Wire Line
	6100 5100 6100 3200
Wire Wire Line
	6100 3200 4400 3200
$EndSCHEMATC
