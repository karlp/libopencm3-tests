EESchema Schematic File Version 5
EELAYER 31 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 3
Title "Cascading per port controllable USB hub"
Date "2019-12-03"
Rev ""
Comp ""
Comment1 "intended for controlling libopencm3 regression test boards"
Comment2 "three double ports for discos w/ user usb"
Comment3 "one up, one down"
Comment4 ""
Comment5 ""
Comment6 ""
Comment7 ""
Comment8 ""
Comment9 ""
$EndDescr
Text Notes 900  1900 0    315  ~ 0
If you think this sheet is boring,\ntake it up with ANTTO!
Text Notes 4100 3600 0    197  ~ 0
Feeling uppitty, into configuration\nand the world at large?
Text Notes 4100 5500 0    197  ~ 0
Feeling down, feeling like power\nswitching and chunks of metal?
$Comp
L Mechanical:MountingHole H1
U 1 1 5E6B1AB6
P 7000 4200
F 0 "H1" H 7100 4245 50  0000 L CNN
F 1 "MountingHole" H 7100 4155 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 7000 4200 50  0001 C CNN
F 3 "~" H 7000 4200 50  0001 C CNN
	1    7000 4200
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H2
U 1 1 5E6B1F07
P 7300 4200
F 0 "H2" H 7400 4245 50  0000 L CNN
F 1 "MountingHole" H 7400 4155 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 7300 4200 50  0001 C CNN
F 3 "~" H 7300 4200 50  0001 C CNN
	1    7300 4200
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H3
U 1 1 5E6B268F
P 7600 4200
F 0 "H3" H 7700 4245 50  0000 L CNN
F 1 "MountingHole" H 7700 4155 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 7600 4200 50  0001 C CNN
F 3 "~" H 7600 4200 50  0001 C CNN
	1    7600 4200
	1    0    0    -1  
$EndComp
$Comp
L Mechanical:MountingHole H4
U 1 1 5E6B27CC
P 7900 4200
F 0 "H4" H 8000 4245 50  0000 L CNN
F 1 "MountingHole" H 8000 4155 50  0000 L CNN
F 2 "MountingHole:MountingHole_2.7mm_M2.5" H 7900 4200 50  0001 C CNN
F 3 "~" H 7900 4200 50  0001 C CNN
	1    7900 4200
	1    0    0    -1  
$EndComp
$Sheet
S 1400 4800 2300 1200
U 5E1AD41F
F0 "downstream-port-power" 50
F1 "downstream-port-power.sch" 50
$EndSheet
$Sheet
S 1400 2900 2300 1100
U 5E1AD3C1
F0 "upstream-config-leds" 50
F1 "upstream-config-leds.sch" 50
$EndSheet
$EndSCHEMATC
