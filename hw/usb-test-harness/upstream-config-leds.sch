EESchema Schematic File Version 5
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 2 3
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
Connection ~ 3500 5500
$Comp
L Device:R_Small R?
U 1 1 5E1AE612
P 7100 2800
F 0 "R?" V 6904 2800 50  0000 C CNN
F 1 "100k" V 6995 2800 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 7100 2800 50  0001 C CNN
F 3 "~" H 7100 2800 50  0001 C CNN
	1    7100 2800
	0    1    1    0   
$EndComp
Text Notes 2800 3800 0    50   ~ 0
sheeet, could even use on of those new \nusb-c with only usb2 pins on it here\noption ALLLLL the things
Wire Wire Line
	2400 5900 2400 6000
$Comp
L Device:R_Small R?
U 1 1 5E1AE630
P 6700 2500
F 0 "R?" V 6504 2500 50  0000 C CNN
F 1 "100k" V 6595 2500 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6700 2500 50  0001 C CNN
F 3 "~" H 6700 2500 50  0001 C CNN
	1    6700 2500
	0    -1   -1   0   
$EndComp
$Comp
L Connector:USB_B_Micro J?
U 1 1 5E1AE64C
P 3400 3100
F 0 "J?" H 3457 3567 50  0000 C CNN
F 1 "USB_B_Micro (up)" H 3457 3476 50  0000 C CNN
F 2 "Connector_USB:USB_Micro-B_Amphenol_10103594-0001LF_Horizontal" H 3550 3050 50  0001 C CNN
F 3 "~" H 3550 3050 50  0001 C CNN
	1    3400 3100
	1    0    0    -1  
$EndComp
Text Notes 2600 1400 0    50   ~ 0
usb A MALE for plugging directly into an upstream\n+\nusb b micro for cable connecting as top level
$Comp
L power:+3V3 #PWR?
U 1 1 5E1AE6A5
P 10900 3400
F 0 "#PWR?" H 10900 3250 50  0001 C CNN
F 1 "+3V3" H 10915 3573 50  0000 C CNN
F 2 "" H 10900 3400 50  0001 C CNN
F 3 "" H 10900 3400 50  0001 C CNN
	1    10900 3400
	0    1    1    0   
$EndComp
Connection ~ 7200 2800
Connection ~ 2800 8700
$Comp
L power:GND #PWR?
U 1 1 5E1AE6C0
P 6900 2800
F 0 "#PWR?" H 6900 2550 50  0001 C CNN
F 1 "GND" H 6905 2627 50  0000 C CNN
F 2 "" H 6900 2800 50  0001 C CNN
F 3 "" H 6900 2800 50  0001 C CNN
	1    6900 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	7300 2800 7200 2800
Wire Wire Line
	7000 2800 6900 2800
$Comp
L Device:Crystal_GND23_Small Y?
U 1 1 5E1AE6D7
P 6500 4800
F 0 "Y?" V 6454 4944 50  0000 L CNN
F 1 "Crystal_GND23_Small" V 6545 4944 50  0000 L CNN
F 2 "Crystal:Crystal_SMD_3225-4Pin_3.2x2.5mm" H 6500 4800 50  0001 C CNN
F 3 "~" H 6500 4800 50  0001 C CNN
	1    6500 4800
	0    1    1    0   
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5E1AE73C
P 2400 8800
F 0 "J?" H 2480 8792 50  0000 L CNN
F 1 "5V_RAW" H 2480 8701 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 2400 8800 50  0001 C CNN
F 3 "~" H 2400 8800 50  0001 C CNN
	1    2400 8800
	-1   0    0    1   
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5E1AE764
P 2500 8300
F 0 "#PWR?" H 2500 8150 50  0001 C CNN
F 1 "+5V" H 2515 8473 50  0000 C CNN
F 2 "" H 2500 8300 50  0001 C CNN
F 3 "" H 2500 8300 50  0001 C CNN
	1    2500 8300
	1    0    0    -1  
$EndComp
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5E1AE77D
P 2400 9300
F 0 "J?" H 2480 9292 50  0000 L CNN
F 1 "GND" H 2480 9201 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 2400 9300 50  0001 C CNN
F 3 "~" H 2400 9300 50  0001 C CNN
	1    2400 9300
	-1   0    0    1   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1AE7A5
P 2800 9400
F 0 "#PWR?" H 2800 9150 50  0001 C CNN
F 1 "GND" H 2805 9227 50  0000 C CNN
F 2 "" H 2800 9400 50  0001 C CNN
F 3 "" H 2800 9400 50  0001 C CNN
	1    2800 9400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2800 8600 2800 8700
Wire Wire Line
	2800 8700 2600 8700
Wire Wire Line
	2600 8800 2800 8800
Wire Wire Line
	2800 8800 2800 8700
Wire Wire Line
	2600 9200 2800 9200
Wire Wire Line
	2800 9200 2800 9300
Wire Wire Line
	2600 9300 2800 9300
Connection ~ 2800 9300
Wire Wire Line
	2800 9300 2800 9400
$Comp
L Connector:Screw_Terminal_01x02 J?
U 1 1 5E1AE7C3
P 2100 7700
F 0 "J?" H 2180 7692 50  0000 L CNN
F 1 "Vext" H 2180 7601 50  0000 L CNN
F 2 "TerminalBlock:TerminalBlock_Altech_AK300-2_P5.00mm" H 2100 7700 50  0001 C CNN
F 3 "~" H 2100 7700 50  0001 C CNN
	1    2100 7700
	-1   0    0    1   
$EndComp
$Comp
L Regulator_Linear:MIC5504-3.3YM5 U?
U 1 1 5E1AE7EB
P 4100 9400
F 0 "U?" H 4100 9767 50  0000 C CNN
F 1 "MIC5504-3.3YM5" H 4100 9676 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 4100 9000 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/MIC550X.pdf" H 3850 9650 50  0001 C CNN
	1    4100 9400
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR?
U 1 1 5E1AE822
P 3600 9200
F 0 "#PWR?" H 3600 9050 50  0001 C CNN
F 1 "+5V" H 3615 9373 50  0000 C CNN
F 2 "" H 3600 9200 50  0001 C CNN
F 3 "" H 3600 9200 50  0001 C CNN
	1    3600 9200
	1    0    0    -1  
$EndComp
Wire Wire Line
	3600 9200 3600 9300
Wire Wire Line
	3600 9300 3700 9300
Wire Wire Line
	3700 9500 3600 9500
Wire Wire Line
	3600 9500 3600 9300
Connection ~ 3600 9300
$Comp
L power:GND #PWR?
U 1 1 5E1AE840
P 4100 9900
F 0 "#PWR?" H 4100 9650 50  0001 C CNN
F 1 "GND" H 4105 9727 50  0000 C CNN
F 2 "" H 4100 9900 50  0001 C CNN
F 3 "" H 4100 9900 50  0001 C CNN
	1    4100 9900
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 9700 4100 9900
$Comp
L power:+3V3 #PWR?
U 1 1 5E1AE856
P 4800 9200
F 0 "#PWR?" H 4800 9050 50  0001 C CNN
F 1 "+3V3" H 4815 9373 50  0000 C CNN
F 2 "" H 4800 9200 50  0001 C CNN
F 3 "" H 4800 9200 50  0001 C CNN
	1    4800 9200
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 9300 4800 9300
Wire Wire Line
	4800 9300 4800 9200
$Comp
L power:+3V3 #PWR?
U 1 1 5E1AE871
P 8600 2000
F 0 "#PWR?" H 8600 1850 50  0001 C CNN
F 1 "+3V3" H 8615 2173 50  0000 C CNN
F 2 "" H 8600 2000 50  0001 C CNN
F 3 "" H 8600 2000 50  0001 C CNN
	1    8600 2000
	1    0    0    -1  
$EndComp
Wire Wire Line
	8300 2200 8300 2100
Wire Wire Line
	8300 2100 8400 2100
Wire Wire Line
	8600 2100 8600 2000
Wire Wire Line
	8600 2200 8600 2100
Connection ~ 8600 2100
Wire Wire Line
	8500 2200 8500 2100
Connection ~ 8500 2100
Wire Wire Line
	8500 2100 8600 2100
Wire Wire Line
	8400 2200 8400 2100
Connection ~ 8400 2100
Wire Wire Line
	8400 2100 8500 2100
Wire Wire Line
	8800 2200 8800 2100
Wire Wire Line
	8800 2100 8600 2100
Wire Wire Line
	3900 1900 3700 1900
Wire Wire Line
	3900 2900 3900 1900
Wire Wire Line
	3700 2900 3900 2900
Wire Wire Line
	4100 2100 3700 2100
Wire Wire Line
	3700 3200 4200 3200
Wire Wire Line
	4100 3100 3700 3100
Wire Wire Line
	4200 2200 3700 2200
$Comp
L Connector:USB_A J?
U 1 1 5E1AEA41
P 3400 2100
F 0 "J?" H 3457 2567 50  0000 C CNN
F 1 "USB_A (male up)" H 3457 2476 50  0000 C CNN
F 2 "Connector_USB:USB_A_Receptacle_GENERIC" H 3550 2050 50  0001 C CNN
F 3 " ~" H 3550 2050 50  0001 C CNN
	1    3400 2100
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 2100 4100 3100
Wire Wire Line
	4200 2200 4200 3200
Wire Wire Line
	6800 2500 7200 2500
Wire Wire Line
	7200 2500 7200 2800
$Comp
L Device:C_Small C?
U 1 1 5E1AEA96
P 6400 2600
F 0 "C?" H 6492 2645 50  0000 L CNN
F 1 "2.2uF" H 6491 2555 50  0000 L CNN
F 2 "" H 6400 2600 50  0001 C CNN
F 3 "~" H 6400 2600 50  0001 C CNN
	1    6400 2600
	1    0    0    -1  
$EndComp
Wire Wire Line
	6400 2500 6600 2500
Wire Wire Line
	6400 2700 6400 2800
Wire Wire Line
	6400 2800 6900 2800
Connection ~ 6900 2800
Wire Wire Line
	3900 1900 6400 1900
Wire Wire Line
	6400 1900 6400 2500
Connection ~ 3900 1900
Connection ~ 6400 2500
Text Label 7300 3000 2    50   ~ 0
UP-
Text Label 7300 3100 2    50   ~ 0
UP+
Wire Wire Line
	4200 2200 4400 2200
Connection ~ 4200 2200
Wire Wire Line
	4100 2100 4400 2100
Connection ~ 4100 2100
Text Label 4400 2200 0    50   ~ 0
UP-
Text Label 4400 2100 0    50   ~ 0
UP+
Wire Wire Line
	6500 4700 7300 4700
Wire Wire Line
	6500 4900 6900 4900
Wire Wire Line
	6900 4900 6900 4800
Wire Wire Line
	6900 4800 7300 4800
$Comp
L Memory_EEPROM:24LC02 U?
U 1 1 5E1AEAC8
P 2900 6000
F 0 "U?" H 2600 6300 50  0000 C CNN
F 1 "FT24C02A" H 3100 5700 50  0000 C CNN
F 2 "" H 2900 6000 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/21709c.pdf" H 2900 6000 50  0001 C CNN
	1    2900 6000
	1    0    0    -1  
$EndComp
Text Notes 3700 4700 0    50   ~ 0
eeprom with some method of factory programing is only sane method?\nCan we do everything we want with strapping?\nIf so, use solder jumpers for the cfg-sel bits
Wire Wire Line
	2400 6000 2400 6100
Wire Wire Line
	7000 3500 7300 3500
Wire Wire Line
	3000 7200 3200 7200
Wire Wire Line
	5900 5300 6300 5300
Text Notes 3500 8900 0    50   ~ 0
Need as much as 460mA at 3.3V for 7xHS ports enabled!
Wire Wire Line
	10200 3100 9200 3100
Wire Wire Line
	2400 6400 2900 6400
Wire Wire Line
	3700 5600 3700 5500
Wire Wire Line
	10500 3300 10400 3300
Wire Wire Line
	10800 3400 10800 3300
Connection ~ 10800 3200
Text Label 7000 3600 2    50   ~ 0
EE_SCL
Wire Wire Line
	10800 3400 10700 3400
Wire Wire Line
	10800 3300 10700 3300
Wire Wire Line
	6100 5000 6300 5000
$Comp
L power:+3V3 #PWR?
U 1 1 5E1AEB1E
P 5900 4800
F 0 "#PWR?" H 5900 4650 50  0001 C CNN
F 1 "+3V3" H 5915 4973 50  0000 C CNN
F 2 "" H 5900 4800 50  0001 C CNN
F 3 "" H 5900 4800 50  0001 C CNN
	1    5900 4800
	1    0    0    -1  
$EndComp
Text Label 3200 7100 0    50   ~ 0
EE_SCL
Connection ~ 2400 6400
Wire Wire Line
	10200 3000 10000 3000
Wire Wire Line
	2000 6400 2400 6400
Wire Wire Line
	2400 6100 2400 6400
Connection ~ 3700 6000
Wire Wire Line
	9700 5000 9200 5000
$Comp
L power:GND #PWR?
U 1 1 5E1AEB3E
P 6300 5300
F 0 "#PWR?" H 6300 5050 50  0001 C CNN
F 1 "GND" H 6305 5127 50  0000 C CNN
F 2 "" H 6300 5300 50  0001 C CNN
F 3 "" H 6300 5300 50  0001 C CNN
	1    6300 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	10900 3400 10800 3400
Wire Wire Line
	3700 6000 3800 6000
Wire Wire Line
	10800 3300 10800 3200
Wire Wire Line
	10200 3400 9200 3400
Wire Wire Line
	3700 5800 3700 6000
Text Notes 1900 7100 0    50   ~ 0
Programming header\nfor EEPROM
Wire Wire Line
	10500 3400 10400 3400
$Comp
L Device:C_Small C?
U 1 1 5E1AEB5A
P 2000 6000
F 0 "C?" H 2092 6045 50  0000 L CNN
F 1 "100nF" H 2091 5955 50  0000 L CNN
F 2 "" H 2000 6000 50  0001 C CNN
F 3 "~" H 2000 6000 50  0001 C CNN
	1    2000 6000
	1    0    0    -1  
$EndComp
Wire Wire Line
	10700 2900 10800 2900
Connection ~ 10800 3000
Wire Wire Line
	10800 2900 10800 3000
Wire Wire Line
	3700 5500 3500 5500
Connection ~ 2900 5500
Connection ~ 3500 5900
Wire Wire Line
	10500 3100 10400 3100
Connection ~ 10000 3000
$Comp
L power:+3V3 #PWR?
U 1 1 5E1AEB80
P 2900 5500
F 0 "#PWR?" H 2900 5350 50  0001 C CNN
F 1 "+3V3" H 2915 5673 50  0000 C CNN
F 2 "" H 2900 5500 50  0001 C CNN
F 3 "" H 2900 5500 50  0001 C CNN
	1    2900 5500
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Small D?
U 1 1 5E1AEB99
P 10600 3100
F 0 "D?" H 10700 3100 50  0000 C CNN
F 1 "LED_Small" H 10600 3243 50  0001 C CNN
F 2 "" V 10600 3100 50  0001 C CNN
F 3 "~" V 10600 3100 50  0001 C CNN
	1    10600 3100
	1    0    0    -1  
$EndComp
Text Label 3800 5900 0    50   ~ 0
EE_SDA
$Comp
L Device:LED_Small D?
U 1 1 5E1AEBBE
P 10600 3200
F 0 "D?" H 10700 3200 50  0000 C CNN
F 1 "LED_Small" H 10600 3343 50  0001 C CNN
F 2 "" V 10600 3200 50  0001 C CNN
F 3 "~" V 10600 3200 50  0001 C CNN
	1    10600 3200
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Small D?
U 1 1 5E1AEBE2
P 10600 3400
F 0 "D?" H 10700 3400 50  0000 C CNN
F 1 "LED_Small" H 10600 3543 50  0001 C CNN
F 2 "" V 10600 3400 50  0001 C CNN
F 3 "~" V 10600 3400 50  0001 C CNN
	1    10600 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5E1AEC06
P 11000 3000
F 0 "#PWR?" H 11000 2750 50  0001 C CNN
F 1 "GND" H 11005 2827 50  0000 C CNN
F 2 "" H 11000 3000 50  0001 C CNN
F 3 "" H 11000 3000 50  0001 C CNN
	1    11000 3000
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5E1AEC1B
P 10300 3200
F 0 "R?" V 10300 3000 50  0000 L CNN
F 1 "330" V 10300 3300 50  0000 L CNN
F 2 "" H 10300 3200 50  0001 C CNN
F 3 "~" H 10300 3200 50  0001 C CNN
	1    10300 3200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5E1AEC37
P 3700 5700
F 0 "R?" H 3759 5745 50  0000 L CNN
F 1 "10k" H 3759 5655 50  0000 L CNN
F 2 "" H 3700 5700 50  0001 C CNN
F 3 "~" H 3700 5700 50  0001 C CNN
	1    3700 5700
	1    0    0    -1  
$EndComp
$Comp
L Device:R_Small R?
U 1 1 5E1AEC53
P 3500 5700
F 0 "R?" H 3559 5745 50  0000 L CNN
F 1 "10k" H 3559 5655 50  0000 L CNN
F 2 "" H 3500 5700 50  0001 C CNN
F 3 "~" H 3500 5700 50  0001 C CNN
	1    3500 5700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5800 3500 5900
Wire Wire Line
	3300 6000 3700 6000
Connection ~ 6300 5000
$Comp
L Device:R_Small R?
U 1 1 5E1AEC72
P 10300 3100
F 0 "R?" V 10300 2900 50  0000 L CNN
F 1 "330" V 10300 3200 50  0000 L CNN
F 2 "" H 10300 3100 50  0001 C CNN
F 3 "~" H 10300 3100 50  0001 C CNN
	1    10300 3100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3000 7100 3200 7100
Wire Wire Line
	2000 5500 2900 5500
Text Notes 4700 5300 0    50   ~ 0
Connect for EEP programming\n(holds hub in reset)
$Comp
L Device:R_Small R?
U 1 1 5E1AEC91
P 10300 3400
F 0 "R?" V 10300 3200 50  0000 L CNN
F 1 "330" V 10300 3500 50  0000 L CNN
F 2 "" H 10300 3400 50  0001 C CNN
F 3 "~" H 10300 3400 50  0001 C CNN
	1    10300 3400
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3000 7000 3200 7000
Wire Wire Line
	10800 3100 10700 3100
Wire Wire Line
	10500 3000 10400 3000
Wire Wire Line
	10000 3000 9200 3000
Wire Wire Line
	3300 6100 3300 6400
Wire Wire Line
	10800 3200 10800 3100
Connection ~ 2400 6100
$Comp
L power:GND #PWR?
U 1 1 5E1AECB4
P 9700 5000
F 0 "#PWR?" H 9700 4750 50  0001 C CNN
F 1 "GND" H 9705 4827 50  0000 C CNN
F 2 "" H 9700 5000 50  0001 C CNN
F 3 "" H 9700 5000 50  0001 C CNN
	1    9700 5000
	-1   0    0    -1  
$EndComp
Wire Wire Line
	11000 3000 10800 3000
$Comp
L Device:LED_Small D?
U 1 1 5E1AECCA
P 10600 3300
F 0 "D?" H 10700 3300 50  0000 C CNN
F 1 "LED_Small" H 10600 3443 50  0001 C CNN
F 2 "" V 10600 3300 50  0001 C CNN
F 3 "~" V 10600 3300 50  0001 C CNN
	1    10600 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 5900 3800 5900
Wire Wire Line
	2000 5500 2000 5900
$Comp
L Device:C_Small C?
U 1 1 5E1AECF0
P 6300 5100
F 0 "C?" H 6392 5145 50  0000 L CNN
F 1 "100nF" H 6391 5055 50  0000 L CNN
F 2 "" H 6300 5100 50  0001 C CNN
F 3 "~" H 6300 5100 50  0001 C CNN
	1    6300 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	10200 3300 9200 3300
$Comp
L Device:R_Small R?
U 1 1 5E1AED0F
P 10300 3000
F 0 "R?" V 10300 2800 50  0000 L CNN
F 1 "330" V 10300 3100 50  0000 L CNN
F 2 "" H 10300 3000 50  0001 C CNN
F 3 "~" H 10300 3000 50  0001 C CNN
	1    10300 3000
	0    -1   -1   0   
$EndComp
$Comp
L Connector:Conn_01x03_Male J?
U 1 1 5E1AED2B
P 2800 7100
F 0 "J?" H 2908 7379 50  0000 C CNN
F 1 "Conn_01x03_Male" H 2908 7288 50  0000 C CNN
F 2 "" H 2800 7100 50  0001 C CNN
F 3 "~" H 2800 7100 50  0001 C CNN
	1    2800 7100
	1    0    0    -1  
$EndComp
Wire Wire Line
	10000 2900 10000 3000
Wire Wire Line
	2900 6300 2900 6400
Wire Wire Line
	10500 2900 10000 2900
Connection ~ 6300 5300
Wire Wire Line
	7000 3600 7300 3600
Wire Wire Line
	2900 5500 2900 5700
$Comp
L power:GND #PWR?
U 1 1 5E1AED60
P 3200 7200
F 0 "#PWR?" H 3200 6950 50  0001 C CNN
F 1 "GND" H 3205 7027 50  0000 C CNN
F 2 "" H 3200 7200 50  0001 C CNN
F 3 "" H 3200 7200 50  0001 C CNN
	1    3200 7200
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 5900 3500 5900
Text Label 3200 7000 0    50   ~ 0
EE_SDA
Wire Wire Line
	2400 6100 2500 6100
Wire Wire Line
	3300 6400 2900 6400
$Comp
L Device:R_Small R?
U 1 1 5E1AED79
P 6100 4900
F 0 "R?" V 5904 4900 50  0000 C CNN
F 1 "100k" V 5995 4900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6100 4900 50  0001 C CNN
F 3 "~" H 6100 4900 50  0001 C CNN
	1    6100 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5900 4800 6100 4800
$Comp
L Device:LED_Small D?
U 1 1 5E1AED96
P 10600 3000
F 0 "D?" H 10500 3000 50  0000 C CNN
F 1 "LED_Small" H 10600 3143 50  0001 C CNN
F 2 "" V 10600 3000 50  0001 C CNN
F 3 "~" V 10600 3000 50  0001 C CNN
	1    10600 3000
	-1   0    0    1   
$EndComp
Connection ~ 10800 3300
Wire Wire Line
	10200 3200 9200 3200
Wire Wire Line
	2400 6000 2500 6000
Wire Wire Line
	10800 3000 10700 3000
Text Label 7000 3500 2    50   ~ 0
EE_SDA
$Comp
L Device:R_Small R?
U 1 1 5E1AEDBF
P 10600 2900
F 0 "R?" H 10659 2945 50  0000 L CNN
F 1 "100k" H 10659 2855 50  0000 L CNN
F 2 "" H 10600 2900 50  0001 C CNN
F 3 "~" H 10600 2900 50  0001 C CNN
	1    10600 2900
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10500 3200 10400 3200
Connection ~ 6100 5000
Wire Wire Line
	5900 5000 6100 5000
$Comp
L Device:R_Small R?
U 1 1 5E1AEDDE
P 10300 3300
F 0 "R?" V 10300 3100 50  0000 L CNN
F 1 "330" V 10300 3400 50  0000 L CNN
F 2 "" H 10300 3300 50  0001 C CNN
F 3 "~" H 10300 3300 50  0001 C CNN
	1    10300 3300
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2000 6100 2000 6400
Wire Wire Line
	3500 5500 2900 5500
Wire Wire Line
	3500 5600 3500 5500
Connection ~ 2900 6400
Wire Wire Line
	6300 5200 6300 5300
$Comp
L power:GND #PWR?
U 1 1 5E1AEDFF
P 2900 6400
F 0 "#PWR?" H 2900 6150 50  0001 C CNN
F 1 "GND" H 2905 6227 50  0000 C CNN
F 2 "" H 2900 6400 50  0001 C CNN
F 3 "" H 2900 6400 50  0001 C CNN
	1    2900 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NO_Small JP?
U 1 1 5E1AEE14
P 5900 5200
F 0 "JP?" V 5855 5248 50  0000 L CNN
F 1 "Jumper_NO_Small" V 5945 5248 50  0000 L CNN
F 2 "" H 5900 5200 50  0001 C CNN
F 3 "~" H 5900 5200 50  0001 C CNN
	1    5900 5200
	0    1    1    0   
$EndComp
Connection ~ 10800 3400
Wire Wire Line
	5900 5100 5900 5000
Wire Wire Line
	10800 3200 10700 3200
Text Label 3800 6000 0    50   ~ 0
EE_SCL
Wire Wire Line
	2500 5900 2400 5900
Wire Wire Line
	6300 5000 7300 5000
Connection ~ 2400 6000
Text Notes 700  7500 0    50   ~ 0
Screw terminals for 5-12V input?\nJumper for whether to use a regulator\nor just expect perfect 5V input?
Text Notes 700  4200 0    50   ~ 0
Optional, pin straps cover everything but usb device serial!
$Comp
L Device:R_Small R?
U 1 1 5E1AE71E
P 9500 4900
F 0 "R?" V 9400 4900 50  0000 C CNN
F 1 "12k" V 9500 4900 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 9500 4900 50  0001 C CNN
F 3 "~" H 9500 4900 50  0001 C CNN
	1    9500 4900
	0    -1   1    0   
$EndComp
Wire Wire Line
	9600 4900 9700 4900
Wire Wire Line
	9700 4900 9700 5000
Connection ~ 9700 5000
Wire Wire Line
	9200 4900 9400 4900
$Comp
L Interface_USB:USB2517 U?
U 1 1 5E19C20E
P 8200 3700
F 0 "U?" H 8250 2012 50  0000 C CNN
F 1 "USB2517" H 8250 1921 50  0000 C CNN
F 2 "Package_DFN_QFN:QFN-64-1EP_9x9mm_P0.5mm_EP4.7x4.7mm_ThermalVias" H 8500 3300 50  0001 C CNN
F 3 "http://ww1.microchip.com/downloads/en/DeviceDoc/USB2517-USB2517i-Data-Sheet-00001598C.pdf" H 8500 3300 50  0001 C CNN
	1    8200 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8200 5300 8200 5500
$Comp
L power:GND #PWR?
U 1 1 5E1AE897
P 8200 5500
F 0 "#PWR?" H 8200 5250 50  0001 C CNN
F 1 "GND" H 8205 5327 50  0000 C CNN
F 2 "" H 8200 5500 50  0001 C CNN
F 3 "" H 8200 5500 50  0001 C CNN
	1    8200 5500
	1    0    0    -1  
$EndComp
Text Notes 7200 10000 0    236  ~ 0
KARRLLL\n* just focus on pin strapped\n* eeprom  is ONLY to save us if we screw up\n* we don't normally care about _hub_ serial\njust make the hub work!\n	
$EndSCHEMATC
