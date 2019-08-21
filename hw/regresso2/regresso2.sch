EESchema Schematic File Version 5
LIBS:regresso2-cache
EELAYER 30 0
EELAYER END
$Descr A3 16535 11693
encoding utf-8
Sheet 1 1
Title "test host"
Date "Autumn 2017"
Rev ""
Comp "libopencm3"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text Label 12450 2600 2    60   ~ 0
DAC1_OUT
Text Label 12450 2700 2    60   ~ 0
DAC2_OUT
Text Label 13450 1400 0    60   ~ 0
I2C_SDA
Text Label 13400 2900 0    60   ~ 0
UART_TX_OUT
Text Label 13500 2200 0    60   ~ 0
UART_RX_OUT
Text Label 12450 2800 2    60   ~ 0
SPI_CS
Text Label 13450 1900 0    60   ~ 0
SPI_MOSI
Text Label 13450 1800 0    60   ~ 0
SPI_MISO
Text Label 13450 1700 0    60   ~ 0
SPI_SCK
NoConn ~ 12500 1700
Text Notes 11650 850  0    60   ~ 0
Arduino UNOr3 pins\nAs used on Nucleo64 boards\n
$Comp
L power:GND #PWR02
U 1 1 56D70CC2
P 11950 2200
F 0 "#PWR02" H 11950 1950 50  0001 C CNN
F 1 "GND" H 11950 2050 50  0000 C CNN
F 2 "" H 11950 2200 50  0000 C CNN
F 3 "" H 11950 2200 50  0000 C CNN
	1    11950 2200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR03
U 1 1 56D70CFF
P 14050 1600
F 0 "#PWR03" H 14050 1350 50  0001 C CNN
F 1 "GND" H 14050 1450 50  0000 C CNN
F 2 "" H 14050 1600 50  0000 C CNN
F 3 "" H 14050 1600 50  0000 C CNN
	1    14050 1600
	1    0    0    -1  
$EndComp
Text Notes 12800 1700 0    60   ~ 0
1
$Comp
L MCU_ST_STM32L1:STM32L151C6TxA U1
U 1 1 58CEFE92
P 3200 5800
F 0 "U1" H 2700 7350 50  0000 L BNN
F 1 "STM32L151C6TxA" H 4100 7350 50  0000 R BNN
F 2 "Housings_QFP:TQFP-48_7x7mm_Pitch0.5mm" H 5150 4250 50  0000 R TNN
F 3 "" H 3200 5800 50  0000 C CNN
	1    3200 5800
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x05_Odd_Even P9
U 1 1 58CF048F
P 1800 1400
F 0 "P9" H 1800 1700 50  0000 C CNN
F 1 "FX2LA" H 1800 1100 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Angled_2x05_Pitch2.54mm" H 1800 200 50  0001 C CNN
F 3 "" H 1800 200 50  0000 C CNN
F 4 "Wurth 613010243121" H 1800 1400 60  0001 C CNN "female-RA"
	1    1800 1400
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR05
U 1 1 58CF077D
P 3200 7550
F 0 "#PWR05" H 3200 7300 50  0001 C CNN
F 1 "GND" H 3200 7400 50  0000 C CNN
F 2 "" H 3200 7550 50  0000 C CNN
F 3 "" H 3200 7550 50  0000 C CNN
	1    3200 7550
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR06
U 1 1 58CF0E5B
P 2200 1700
F 0 "#PWR06" H 2200 1450 50  0001 C CNN
F 1 "GND" H 2200 1550 50  0000 C CNN
F 2 "" H 2200 1700 50  0000 C CNN
F 3 "" H 2200 1700 50  0000 C CNN
	1    2200 1700
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR07
U 1 1 58CF1156
P 5350 1500
F 0 "#PWR07" H 5350 1250 50  0001 C CNN
F 1 "GND" H 5350 1350 50  0000 C CNN
F 2 "" H 5350 1500 50  0000 C CNN
F 3 "" H 5350 1500 50  0000 C CNN
	1    5350 1500
	1    0    0    -1  
$EndComp
Text Label 3900 6800 0    60   ~ 0
USB_DM
Text Label 3900 6900 0    60   ~ 0
USB_DP
Text Label 5000 1750 3    60   ~ 0
USB_DM
Text Label 4900 1750 3    60   ~ 0
USB_DP
Text Label 3900 6100 0    60   ~ 0
DAC1_OUT
Text Label 3900 6200 0    60   ~ 0
DAC2_OUT
Text Label 2200 6500 0    60   ~ 0
I2C_SCL
Text Label 2200 6600 0    60   ~ 0
I2C_SDA
Text Notes 6450 1950 0    60   ~ 0
[x] debug via via 6pin\n[  ] debug via 10x5x1.27mm cortex SMD\n
Text Label 13450 1300 0    60   ~ 0
I2C_SCL
Text Notes 11600 2600 0    60   ~ 0
A0
Text Notes 11600 2700 0    60   ~ 0
A1
Text Notes 14250 2000 0    50   ~ 0
PA5 - spi1-sck (DACOUT2)\nPA6 - spi1-miso\nPA7 - spi1-mosi\nPB6 - usart1-tx on target
Text Notes 14250 2900 0    50   ~ 0
PA10 - usart1-rx on target
Wire Notes Line
	11625 925  13025 925 
Wire Notes Line
	13025 925  13025 575 
Wire Wire Line
	11950 2200 12400 2200
Connection ~ 12400 2200
Wire Wire Line
	12500 2600 12450 2600
Wire Wire Line
	12500 2700 12450 2700
Wire Wire Line
	12500 2800 12450 2800
Wire Wire Line
	13300 1900 13450 1900
Wire Wire Line
	13300 1800 13450 1800
Wire Wire Line
	13300 1700 13450 1700
Wire Wire Line
	13300 1600 14050 1600
Wire Notes Line
	11225 600  11225 3550
Wire Notes Line
	11600 3550 14300 3550
Wire Wire Line
	3100 7400 3100 7500
Wire Wire Line
	3100 7500 3200 7500
Wire Wire Line
	3200 7400 3200 7500
Connection ~ 3200 7500
Wire Wire Line
	3300 7500 3300 7400
Wire Wire Line
	3400 7500 3400 7400
Connection ~ 3300 7500
Wire Wire Line
	2100 1600 2200 1600
Wire Wire Line
	3800 6800 3900 6800
Wire Wire Line
	3800 6900 3900 6900
Wire Wire Line
	5000 1700 5000 2150
Wire Wire Line
	4900 1700 4900 2150
Wire Wire Line
	3800 6100 3900 6100
Wire Wire Line
	3800 6200 3900 6200
Wire Wire Line
	2200 6600 2600 6600
Wire Wire Line
	2600 6500 2200 6500
Text Label 1600 1200 2    60   ~ 0
I2C_SDA
Text Label 2100 1200 0    60   ~ 0
I2C_SCL
Text Label 1600 1500 2    60   ~ 0
UART_TX_OUT
Text Label 1600 1300 2    60   ~ 0
SPI_MOSI
Text Label 2100 1300 0    60   ~ 0
SPI_MISO
Text Label 1600 1400 2    60   ~ 0
SPI_SCK
Text Notes 850  2300 0    60   Italic 0
NOTE: this can be freely routed to do whatever's possible!\nPotentially:\nJumpers to select what you're connecting here?\n(but non-ideal)
$Comp
L Device:C C2
U 1 1 58CF43FF
P 2800 9900
F 0 "C2" H 2825 10000 50  0000 L CNN
F 1 "100n" H 2825 9800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 2838 9750 50  0000 C CNN
F 3 "" H 2800 9900 50  0000 C CNN
	1    2800 9900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 58CF46A1
P 3350 10250
F 0 "#PWR08" H 3350 10000 50  0001 C CNN
F 1 "GND" H 3350 10100 50  0000 C CNN
F 2 "" H 3350 10250 50  0000 C CNN
F 3 "" H 3350 10250 50  0000 C CNN
	1    3350 10250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR09
U 1 1 58CF46D6
P 3350 9550
F 0 "#PWR09" H 3350 9400 50  0001 C CNN
F 1 "+3V3" H 3350 9690 50  0000 C CNN
F 2 "" H 3350 9550 50  0000 C CNN
F 3 "" H 3350 9550 50  0000 C CNN
	1    3350 9550
	1    0    0    -1  
$EndComp
Wire Wire Line
	3350 9750 3350 9550
Wire Wire Line
	2800 9750 3150 9750
Connection ~ 3150 9750
Connection ~ 3350 9750
Connection ~ 3550 9750
Wire Wire Line
	2800 10050 3150 10050
Connection ~ 3550 10050
Connection ~ 3150 10050
Wire Wire Line
	3350 10250 3350 10050
Connection ~ 3350 10050
$Comp
L Device:C C1
U 1 1 58CF4B65
P 1250 4400
F 0 "C1" H 1275 4500 50  0000 L CNN
F 1 "100n" H 1275 4300 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1288 4250 50  0000 C CNN
F 3 "" H 1250 4400 50  0000 C CNN
	1    1250 4400
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR010
U 1 1 58CF4BEA
P 950 4400
F 0 "#PWR010" H 950 4150 50  0001 C CNN
F 1 "GND" H 950 4250 50  0000 C CNN
F 2 "" H 950 4400 50  0000 C CNN
F 3 "" H 950 4400 50  0000 C CNN
	1    950  4400
	1    0    0    -1  
$EndComp
Wire Wire Line
	2600 4400 1400 4400
Wire Wire Line
	1100 4400 950  4400
NoConn ~ 2600 5500
NoConn ~ 2600 5400
NoConn ~ 2600 5300
NoConn ~ 2600 6100
Wire Wire Line
	2600 6000 2200 6000
Text Label 2200 6000 0    60   Italic 0
SWO
Wire Wire Line
	3800 7000 3900 7000
Wire Wire Line
	3800 7100 3900 7100
Text Label 3900 7000 0    60   Italic 0
SWDIO
Text Label 3900 7100 0    60   Italic 0
SWCLK
Text Label 1400 4400 0    60   Italic 0
NRST
$Comp
L power:PWR_FLAG #FLG011
U 1 1 58CF5E41
P 6900 9950
F 0 "#FLG011" H 6900 10045 50  0001 C CNN
F 1 "PWR_FLAG" H 6900 10130 50  0000 C CNN
F 2 "" H 6900 9950 50  0000 C CNN
F 3 "" H 6900 9950 50  0000 C CNN
	1    6900 9950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR012
U 1 1 58CF5E8B
P 6900 10050
F 0 "#PWR012" H 6900 9800 50  0001 C CNN
F 1 "GND" H 6900 9900 50  0000 C CNN
F 2 "" H 6900 10050 50  0000 C CNN
F 3 "" H 6900 10050 50  0000 C CNN
	1    6900 10050
	1    0    0    -1  
$EndComp
Wire Wire Line
	6900 9950 6900 10050
Text Notes 3650 1050 0    60   Italic 12
ONLY GROUND BETWEEN BOARDS by default!\nIntended for both to have their own USB\nOption for debug only, with power FROM target
$Comp
L Device:C C3
U 1 1 58CF69A9
P 3150 9900
F 0 "C3" H 3175 10000 50  0000 L CNN
F 1 "100n" H 3175 9800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3188 9750 50  0000 C CNN
F 3 "" H 3150 9900 50  0000 C CNN
	1    3150 9900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C4
U 1 1 58CF69F1
P 3550 9900
F 0 "C4" H 3575 10000 50  0000 L CNN
F 1 "100n" H 3575 9800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3588 9750 50  0000 C CNN
F 3 "" H 3550 9900 50  0000 C CNN
	1    3550 9900
	1    0    0    -1  
$EndComp
$Comp
L Device:C C5
U 1 1 58CF6A3A
P 3900 9900
F 0 "C5" H 3925 10000 50  0000 L CNN
F 1 "100n" H 3925 9800 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 3938 9750 50  0000 C CNN
F 3 "" H 3900 9900 50  0000 C CNN
	1    3900 9900
	1    0    0    -1  
$EndComp
Text Notes 1350 1650 0    60   ~ 0
CLK
Text Label 4075 6600 0    60   ~ 0
UART_TX_OUT
Text Label 4075 6700 0    60   ~ 0
UART_RX_OUT
Wire Wire Line
	2600 6900 2200 6900
Wire Wire Line
	2600 7000 2200 7000
Wire Wire Line
	2600 7100 2200 7100
Wire Wire Line
	2600 7200 2200 7200
Text Label 2200 6900 0    60   ~ 0
SPI_CS
Text Label 2200 7000 0    60   ~ 0
SPI_SCK
Text Label 2200 7100 0    60   ~ 0
SPI_MISO
Text Label 2200 7200 0    60   ~ 0
SPI_MOSI
$Comp
L Regulator_Linear:MIC5504-3.3YM5 U2
U 1 1 59F7B611
P 8200 10050
F 0 "U2" H 7900 10325 50  0000 L CNN
F 1 "MIC550x-3.3YM5" H 8725 10325 50  0000 R CNN
F 2 "Package_TO_SOT_SMD:SOT-23-5" H 8850 9800 50  0001 C CIN
F 3 "" H 8200 10050 50  0000 C CNN
	1    8200 10050
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR013
U 1 1 59F7B6BD
P 8950 9950
F 0 "#PWR013" H 8950 9800 50  0001 C CNN
F 1 "+3V3" H 8950 10090 50  0000 C CNN
F 2 "" H 8950 9950 50  0000 C CNN
F 3 "" H 8950 9950 50  0000 C CNN
	1    8950 9950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR014
U 1 1 59F7B70C
P 8200 10500
F 0 "#PWR014" H 8200 10250 50  0001 C CNN
F 1 "GND" H 8200 10350 50  0000 C CNN
F 2 "" H 8200 10500 50  0000 C CNN
F 3 "" H 8200 10500 50  0000 C CNN
	1    8200 10500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8600 9950 8900 9950
Wire Wire Line
	7650 9950 7700 9950
Wire Wire Line
	4400 1750 4700 1750
Wire Wire Line
	4700 1750 4700 1700
Wire Wire Line
	4400 1750 4400 1700
Wire Wire Line
	8200 10350 8200 10450
$Comp
L power:+5V #PWR015
U 1 1 59F7C40C
P 4400 1700
F 0 "#PWR015" H 4400 1550 50  0001 C CNN
F 1 "+5V" H 4400 1840 50  0000 C CNN
F 2 "" H 4400 1700 50  0001 C CNN
F 3 "" H 4400 1700 50  0001 C CNN
	1    4400 1700
	1    0    0    -1  
$EndComp
$Comp
L power:+5V #PWR016
U 1 1 59F7C819
P 7650 9950
F 0 "#PWR016" H 7650 9800 50  0001 C CNN
F 1 "+5V" H 7650 10090 50  0000 C CNN
F 2 "" H 7650 9950 50  0001 C CNN
F 3 "" H 7650 9950 50  0001 C CNN
	1    7650 9950
	1    0    0    -1  
$EndComp
NoConn ~ 5300 1300
Wire Wire Line
	7800 10150 7700 10150
Wire Wire Line
	7700 10150 7700 9950
Connection ~ 7700 9950
$Comp
L Connector_Generic:Conn_01x06 P11
U 1 1 59F7E088
P 6850 1250
F 0 "P11" H 6850 1600 50  0000 C CNN
F 1 "CONN_01X06" V 6950 1250 50  0000 C CNN
F 2 "Socket_Strips:Socket_Strip_Straight_1x06_Pitch2.54mm" H 6850 1250 50  0001 C CNN
F 3 "" H 6850 1250 50  0001 C CNN
	1    6850 1250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	7050 1050 7600 1050
Wire Wire Line
	7050 1150 7100 1150
Wire Wire Line
	7050 1250 7600 1250
Wire Wire Line
	7050 1350 7100 1350
Wire Wire Line
	7050 1450 7100 1450
Wire Wire Line
	7050 1550 7100 1550
$Comp
L power:GND #PWR017
U 1 1 59F7E9AF
P 7600 1250
F 0 "#PWR017" H 7600 1000 50  0001 C CNN
F 1 "GND" H 7600 1100 50  0000 C CNN
F 2 "" H 7600 1250 50  0000 C CNN
F 3 "" H 7600 1250 50  0000 C CNN
	1    7600 1250
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR018
U 1 1 59F7EA7B
P 7600 1050
F 0 "#PWR018" H 7600 900 50  0001 C CNN
F 1 "+3V3" H 7600 1190 50  0000 C CNN
F 2 "" H 7600 1050 50  0000 C CNN
F 3 "" H 7600 1050 50  0000 C CNN
	1    7600 1050
	1    0    0    -1  
$EndComp
Text Label 7100 1450 0    60   ~ 0
NRST
Text Label 7100 1550 0    60   ~ 0
SWO
Text Notes 6700 850  0    60   ~ 0
STLink compatible debug
NoConn ~ 3800 7200
NoConn ~ 2600 6400
NoConn ~ 2600 6300
NoConn ~ 2600 6200
Text Notes -121300 438350 0    60   ~ 0
IN/OUT from perspective of HOST\nDUT Nucleo is TARGET
Text Notes 5350 7300 0    60   ~ 0
IN/OUT is from the perspective of HOST\nDUT Nucleo is TARGET
Text Label 2100 1400 0    60   ~ 0
SPI_CS
Text Label 7100 1150 0    60   ~ 0
SWCLK
Text Label 7100 1350 0    60   ~ 0
SWDIO
$Comp
L Device:R R1
U 1 1 5A0F8D14
P 10600 9750
F 0 "R1" V 10680 9750 50  0000 C CNN
F 1 "4k7" V 10600 9750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10530 9750 50  0001 C CNN
F 3 "" H 10600 9750 50  0001 C CNN
	1    10600 9750
	1    0    0    -1  
$EndComp
$Comp
L Device:R R2
U 1 1 5A0F8EF5
P 10900 9750
F 0 "R2" V 10980 9750 50  0000 C CNN
F 1 "4k7" V 10900 9750 50  0000 C CNN
F 2 "Resistor_SMD:R_0603_1608Metric" V 10830 9750 50  0001 C CNN
F 3 "" H 10900 9750 50  0001 C CNN
	1    10900 9750
	1    0    0    -1  
$EndComp
Wire Wire Line
	10900 9500 10900 9600
Wire Wire Line
	10600 9500 10750 9500
Wire Wire Line
	10600 9500 10600 9600
$Comp
L power:+3V3 #PWR019
U 1 1 5A0F90D0
P 10750 9500
F 0 "#PWR019" H 10750 9350 50  0001 C CNN
F 1 "+3V3" H 10750 9640 50  0000 C CNN
F 2 "" H 10750 9500 50  0000 C CNN
F 3 "" H 10750 9500 50  0000 C CNN
	1    10750 9500
	1    0    0    -1  
$EndComp
$Comp
L Device:Jumper_NC_Small JP1
U 1 1 5A0F91B9
P 10600 10050
F 0 "JP1" H 10600 10130 50  0000 C CNN
F 1 "Jumper_NC_Small" H 10610 9990 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 10600 10050 50  0001 C CNN
F 3 "" H 10600 10050 50  0001 C CNN
	1    10600 10050
	0    -1   -1   0   
$EndComp
$Comp
L Device:Jumper_NC_Small JP2
U 1 1 5A0F9269
P 10900 10050
F 0 "JP2" H 10900 10130 50  0000 C CNN
F 1 "Jumper_NC_Small" H 10910 9990 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 10900 10050 50  0001 C CNN
F 3 "" H 10900 10050 50  0001 C CNN
	1    10900 10050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10600 9900 10600 9950
Wire Wire Line
	10900 9900 10900 9950
Wire Wire Line
	10900 10150 10900 10300
Wire Wire Line
	10600 10150 10600 10300
Text Label 10600 10300 3    60   ~ 0
I2C_SDA
Text Label 10900 10300 3    60   ~ 0
I2C_SCL
Text Notes 950  1000 0    60   ~ 0
Designed for a cheap FX2LA\nto be just "plugged in" for sigrok
NoConn ~ 13300 1500
NoConn ~ 12500 1800
NoConn ~ 12500 1900
Wire Wire Line
	12500 2300 12400 2300
Wire Wire Line
	12400 2300 12400 2200
NoConn ~ 12500 2400
Text Notes 3000 4600 0    39   ~ 0
Bootloader wut?
$Comp
L Device:C C6
U 1 1 5A0FB867
P 1250 4800
F 0 "C6" H 1275 4900 50  0000 L CNN
F 1 "100n" H 1275 4700 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1288 4650 50  0000 C CNN
F 3 "" H 1250 4800 50  0000 C CNN
	1    1250 4800
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR020
U 1 1 5A0FB8D8
P 950 4800
F 0 "#PWR020" H 950 4550 50  0001 C CNN
F 1 "GND" H 950 4650 50  0000 C CNN
F 2 "" H 950 4800 50  0000 C CNN
F 3 "" H 950 4800 50  0000 C CNN
	1    950  4800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 4800 950  4800
Text Notes 3000 4800 0    60   ~ 0
Not needed on L1, but needed for L4/F3
Text Notes 12100 3400 0    60   ~ 0
TODO\n[ ] Route all spare pins to _something_?
$Comp
L Device:C C8
U 1 1 5A0FC660
P 8900 10300
F 0 "C8" H 8925 10400 50  0000 L CNN
F 1 "1u" H 8925 10200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 8938 10150 50  0000 C CNN
F 3 "" H 8900 10300 50  0000 C CNN
	1    8900 10300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C7
U 1 1 5A0FC76D
P 7700 10300
F 0 "C7" H 7725 10400 50  0000 L CNN
F 1 "1u" H 7725 10200 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 7738 10150 50  0000 C CNN
F 3 "" H 7700 10300 50  0000 C CNN
	1    7700 10300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7700 10450 8200 10450
Connection ~ 8200 10450
Wire Wire Line
	8900 10150 8900 9950
Connection ~ 8900 9950
Connection ~ 10750 9500
Text Label 2600 4800 2    60   ~ 0
VLCD
Text Notes 2450 3500 0    60   ~ 0
L1/F303/L4 in qfp48 are all compatible with this layout\n(but f3 needs external pullup on USB-DP)
$Comp
L Device:Crystal_GND24_Small Y1
U 1 1 5A137095
P 1650 5100
F 0 "Y1" H 1700 5275 50  0000 L CNN
F 1 "Crystal_GND24_Small" H 1700 5200 50  0000 L CNN
F 2 "Crystals:Crystal_SMD_SeikoEpson_FA238-4pin_3.2x2.5mm" H 1650 5100 50  0001 C CNN
F 3 "" H 1650 5100 50  0001 C CNN
	1    1650 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	1750 5100 1800 5100
$Comp
L Device:C C10
U 1 1 5A1373B8
P 1800 5450
F 0 "C10" H 1825 5550 50  0000 L CNN
F 1 "18pf" H 1825 5350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1838 5300 50  0000 C CNN
F 3 "" H 1800 5450 50  0000 C CNN
	1    1800 5450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 5A137455
P 1500 5450
F 0 "C9" H 1525 5550 50  0000 L CNN
F 1 "18pf" H 1525 5350 50  0000 L CNN
F 2 "Capacitor_SMD:C_0603_1608Metric" H 1538 5300 50  0000 C CNN
F 3 "" H 1500 5450 50  0000 C CNN
	1    1500 5450
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR021
U 1 1 5A137592
P 1650 5800
F 0 "#PWR021" H 1650 5550 50  0001 C CNN
F 1 "GND" H 1650 5650 50  0000 C CNN
F 2 "" H 1650 5800 50  0000 C CNN
F 3 "" H 1650 5800 50  0000 C CNN
	1    1650 5800
	1    0    0    -1  
$EndComp
Wire Wire Line
	1650 5800 1650 5600
Wire Wire Line
	1500 5600 1650 5600
Connection ~ 1650 5600
NoConn ~ 12500 2100
Text Notes 12300 2150 0    60   ~ 0
5V
$Comp
L Connector_Generic:Conn_01x02 J1
U 1 1 5A36EBE0
P 5000 2350
F 0 "J1" H 5000 2500 50  0000 C CNN
F 1 "CONN_01X02" V 5100 2350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5000 2350 50  0001 C CNN
F 3 "" H 5000 2350 50  0001 C CNN
	1    5000 2350
	0    1    1    0   
$EndComp
Text Notes 4550 2700 0    60   ~ 0
Probe points, for\nbus tracing USB
Wire Wire Line
	2600 4600 2250 4600
Wire Wire Line
	1500 4900 1500 5100
Wire Wire Line
	1500 5100 1550 5100
Wire Wire Line
	2600 5000 2400 5000
Wire Wire Line
	2400 5000 2400 4900
Wire Wire Line
	2400 4900 1500 4900
Connection ~ 1500 5100
Wire Wire Line
	1800 5300 1800 5100
Connection ~ 1800 5100
$Comp
L power:GND #PWR022
U 1 1 5A43A55C
P 2250 4600
F 0 "#PWR022" H 2250 4350 50  0001 C CNN
F 1 "GND" H 2250 4450 50  0000 C CNN
F 2 "" H 2250 4600 50  0000 C CNN
F 3 "" H 2250 4600 50  0000 C CNN
	1    2250 4600
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint PB0
U 1 1 5A43D04F
P 2550 5700
F 0 "PB0" H 2550 5970 50  0000 C CNN
F 1 "TEST_1P" H 2550 5900 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 2750 5700 50  0001 C CNN
F 3 "" H 2750 5700 50  0001 C CNN
	1    2550 5700
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint PB1
U 1 1 5A43D328
P 2350 5800
F 0 "PB1" H 2350 6070 50  0000 C CNN
F 1 "TEST_1P" H 2350 6000 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 2550 5800 50  0001 C CNN
F 3 "" H 2550 5800 50  0001 C CNN
	1    2350 5800
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint PB10
U 1 1 5A43D40B
P 2300 6700
F 0 "PB10" H 2300 6970 50  0000 C CNN
F 1 "TEST_1P" H 2300 6900 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 2500 6700 50  0001 C CNN
F 3 "" H 2500 6700 50  0001 C CNN
	1    2300 6700
	0    -1   -1   0   
$EndComp
$Comp
L Connector:TestPoint PB11
U 1 1 5A43D49D
P 2150 6800
F 0 "PB11" H 2150 7070 50  0000 C CNN
F 1 "TEST_1P" H 2150 7000 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 2350 6800 50  0001 C CNN
F 3 "" H 2350 6800 50  0001 C CNN
	1    2150 6800
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3800 6500 4075 6500
Wire Wire Line
	3800 6600 4075 6600
Wire Wire Line
	3800 6700 4075 6700
$Comp
L Connector:TestPoint PA0
U 1 1 5A43FB1F
P 4350 5700
F 0 "PA0" H 4350 5970 50  0000 C CNN
F 1 "TEST_1P" H 4350 5900 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 4550 5700 50  0001 C CNN
F 3 "" H 4550 5700 50  0001 C CNN
	1    4350 5700
	0    1    1    0   
$EndComp
$Comp
L Connector:TestPoint PA1
U 1 1 5A43FBC5
P 4550 5800
F 0 "PA1" H 4550 6070 50  0000 C CNN
F 1 "TEST_1P" H 4550 6000 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 4750 5800 50  0001 C CNN
F 3 "" H 4750 5800 50  0001 C CNN
	1    4550 5800
	0    1    1    0   
$EndComp
Text Notes 3000 5450 0    60   ~ 0
no need for rtc\nthis is the host board
Wire Wire Line
	2600 4800 1400 4800
NoConn ~ 2600 5900
Text Notes 8900 7100 0    60   ~ 0
TODO Jumper RTS/CTS over to I2C on fx2 port?
Wire Wire Line
	12400 2200 12500 2200
Wire Wire Line
	3200 7500 3300 7500
Wire Wire Line
	3200 7500 3200 7550
Wire Wire Line
	3300 7500 3400 7500
Wire Wire Line
	3150 9750 3350 9750
Wire Wire Line
	3350 9750 3550 9750
Wire Wire Line
	3550 9750 3900 9750
Wire Wire Line
	3550 10050 3900 10050
Wire Wire Line
	3150 10050 3350 10050
Wire Wire Line
	3350 10050 3550 10050
Wire Wire Line
	7700 9950 7800 9950
Wire Wire Line
	8200 10450 8200 10500
Wire Wire Line
	8200 10450 8900 10450
Wire Wire Line
	8900 9950 8950 9950
Wire Wire Line
	10750 9500 10900 9500
Wire Wire Line
	1650 5600 1800 5600
Wire Wire Line
	1500 5100 1500 5300
Wire Wire Line
	1800 5100 2600 5100
Connection ~ 7700 10150
Wire Wire Line
	3300 4100 3400 4100
Wire Wire Line
	3400 4100 3400 4200
Wire Wire Line
	3300 4100 3300 4200
Wire Wire Line
	3200 4200 3200 4100
Wire Wire Line
	3100 4100 3100 4200
Connection ~ 3250 4100
$Comp
L power:+3V3 #PWR04
U 1 1 58CF063D
P 3250 4000
F 0 "#PWR04" H 3250 3850 50  0001 C CNN
F 1 "+3V3" H 3250 4140 50  0000 C CNN
F 2 "" H 3250 4000 50  0000 C CNN
F 3 "" H 3250 4000 50  0000 C CNN
	1    3250 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	3200 4100 3250 4100
Connection ~ 3300 4100
Wire Wire Line
	3100 4100 3200 4100
Wire Wire Line
	3250 4100 3250 4000
Wire Wire Line
	3250 4100 3300 4100
Connection ~ 3200 4100
$Comp
L Connector:USB_OTG P10
U 1 1 58CF10AD
P 4900 1400
F 0 "P10" V 4650 1650 50  0000 C CNN
F 1 "CONTROL" V 4650 1300 50  0000 C CNN
F 2 "Connectors:USB_Micro-B_10103594-0001LF" V 4850 1300 50  0001 C CNN
F 3 "" V 4850 1300 50  0000 C CNN
	1    4900 1400
	0    -1   1    0   
$EndComp
NoConn ~ 5100 1700
Wire Wire Line
	5300 1400 5350 1400
Wire Wire Line
	5350 1400 5350 1500
$Comp
L Connector_Generic:Conn_01x10 J4
U 1 1 5D5815FB
P 13100 1800
F 0 "J4" H 13018 1075 50  0000 C CNN
F 1 "Conn_01x10" H 13018 1166 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x10_P2.54mm_Vertical" H 13100 1800 50  0001 C CNN
F 3 "~" H 13100 1800 50  0001 C CNN
	1    13100 1800
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J5
U 1 1 5D585E14
P 13100 2800
F 0 "J5" H 13018 2175 50  0000 C CNN
F 1 "Conn_01x08" H 13018 2266 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 13100 2800 50  0001 C CNN
F 3 "~" H 13100 2800 50  0001 C CNN
	1    13100 2800
	-1   0    0    1   
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J2
U 1 1 5D58A16D
P 12700 2000
F 0 "J2" H 12618 1375 50  0000 C CNN
F 1 "Conn_01x08" H 12618 1466 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x08_P2.54mm_Vertical" H 12700 2000 50  0001 C CNN
F 3 "~" H 12700 2000 50  0001 C CNN
	1    12700 2000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x06 J3
U 1 1 5D58C12A
P 12700 2800
F 0 "J3" H 12618 2275 50  0000 C CNN
F 1 "Conn_01x06" H 12618 2366 50  0000 C CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x06_P2.54mm_Vertical" H 12700 2800 50  0001 C CNN
F 3 "~" H 12700 2800 50  0001 C CNN
	1    12700 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	13300 1400 13450 1400
Wire Wire Line
	13300 1300 13450 1300
NoConn ~ 12500 3100
NoConn ~ 12500 3000
Wire Wire Line
	13300 2900 13400 2900
Text Notes 2050 7325 0    50   ~ 0
SPI2 on host
Text Notes 4850 5925 0    50   ~ 0
usart2 on host
Text Notes 11350 2800 0    50   ~ 0
PA4 - spi1-nss
Text Notes 575  7025 0    50   ~ 0
SPI_CS is also adc18, so fine for\ndac from target as well as CS
NoConn ~ 3800 6300
NoConn ~ 3800 6400
NoConn ~ 12500 2900
Text Notes 14250 1400 0    50   ~ 0
PB8 - i2c1-scl\nPB9 - i2c1-sda
Text Notes 13400 3125 0    61   ~ 0
PA2\nPA3
Text Notes 13400 2800 0    61   ~ 0
PB10 isc2-scl / tim2ch3 / usart3-tx\nPB4 spi1-miso / tim3ch1\nPB5 spi1-mosi / i2c1-smba / tim3ch2\nPB3 (unusable, SWO to stlink)
Text Notes 13600 3075 0    61   ~ 0
Unusable, jumpered to stlink by default
Text Label 4075 6500 0    61   ~ 0
UART_CK_OUT
Text Notes 4750 6600 0    61   ~ 0
usart1 on host
Wire Wire Line
	13300 2400 13425 2400
Text Label 13425 2400 0    61   ~ 0
USART_CK_OUT
Text Notes 14125 2375 0    61   ~ 0
PA8 - usart1-ck
NoConn ~ 13300 2500
NoConn ~ 13300 2600
NoConn ~ 13300 3000
NoConn ~ 13300 3100
NoConn ~ 13300 2100
NoConn ~ 13500 2000
Text Notes 14200 2200 0    61   ~ 0
PC7\nPA9
Wire Wire Line
	13300 2200 13500 2200
Wire Wire Line
	13300 2000 13500 2000
NoConn ~ 12500 2000
Text Notes 12200 2000 0    61   ~ 0
3.3v
$Comp
L power:GND #PWR0101
U 1 1 5D5EBF36
P 6300 6400
F 0 "#PWR0101" H 6300 6150 50  0001 C CNN
F 1 "GND" H 6300 6250 50  0000 C CNN
F 2 "" H 6300 6400 50  0000 C CNN
F 3 "" H 6300 6400 50  0000 C CNN
	1    6300 6400
	1    0    0    -1  
$EndComp
$Comp
L Device:LED_Small D1
U 1 1 5D5F1C52
P 6300 6200
F 0 "D1" V 6346 6132 50  0000 R CNN
F 1 "indicator" V 6255 6132 50  0000 R CNN
F 2 "LED_SMD:LED_0805_2012Metric" V 6300 6200 50  0001 C CNN
F 3 "~" V 6300 6200 50  0001 C CNN
	1    6300 6200
	0    -1   -1   0   
$EndComp
$Comp
L Device:R_Small R3
U 1 1 5D5F24EE
P 6300 5900
F 0 "R3" H 6359 5946 50  0000 L CNN
F 1 "1k" H 6359 5855 50  0000 L CNN
F 2 "Resistor_SMD:R_0603_1608Metric" H 6300 5900 50  0001 C CNN
F 3 "~" H 6300 5900 50  0001 C CNN
	1    6300 5900
	1    0    0    -1  
$EndComp
Wire Wire Line
	6300 6000 6300 6100
Wire Wire Line
	6300 6300 6300 6400
Wire Wire Line
	6300 5800 6300 5700
Wire Wire Line
	6300 5700 6100 5700
Text Label 6100 5700 2    61   ~ 0
LED_CONTROL
$Comp
L power:GND #PWR0102
U 1 1 5D5F306A
P 7900 6500
F 0 "#PWR0102" H 7900 6250 50  0001 C CNN
F 1 "GND" H 7900 6350 50  0000 C CNN
F 2 "" H 7900 6500 50  0000 C CNN
F 3 "" H 7900 6500 50  0000 C CNN
	1    7900 6500
	1    0    0    -1  
$EndComp
$Comp
L Connector:TestPoint PA2
U 1 1 5D5F3358
P 7900 6400
F 0 "PA2" H 7900 6670 50  0000 C CNN
F 1 "TEST_1P" H 7900 6600 50  0000 C CNN
F 2 "TestPoint:TestPoint_THTPad_1.5x1.5mm_Drill0.7mm" H 8100 6400 50  0001 C CNN
F 3 "" H 8100 6400 50  0001 C CNN
	1    7900 6400
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 6500 7900 6400
NoConn ~ 3800 5900
NoConn ~ 3800 6000
Text Notes 12200 1800 0    61   ~ 0
IOREF
Text Notes 12200 1900 0    61   ~ 0
RESET
Text Notes 12300 2400 0    61   ~ 0
Vin
NoConn ~ 13300 2800
NoConn ~ 13300 2700
Text Notes 13400 1500 0    61   ~ 0
AREF
Text Notes 6400 4600 0    61   ~ 0
add a button for autorunning tests, or getting to DFU mode? or is that just via\na usb control method in the host we just "always impelemtn"
Text Label 2100 1500 0    60   ~ 0
UART_RX_OUT
Wire Wire Line
	2200 1600 2200 1700
NoConn ~ 1600 1600
$EndSCHEMATC
