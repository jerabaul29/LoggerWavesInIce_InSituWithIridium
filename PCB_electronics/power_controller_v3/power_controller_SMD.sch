EESchema Schematic File Version 2
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:SeeedOPL-Capacitor-2016
LIBS:SeeedOPL-Connector-2016
LIBS:SeeedOPL-Crystal Oscillator-2016
LIBS:SeeedOPL-Diode-2016
LIBS:SeeedOPL-Display-2016
LIBS:SeeedOPL-Fuse-2016
LIBS:SeeedOPL-IC-2016
LIBS:SeeedOPL-Inductor-2016
LIBS:SeeedOPL-LED-2016
LIBS:SeeedOPL-Relay-2016
LIBS:SeeedOPL-Resistor-2016
LIBS:SeeedOPL-Sensor-2016
LIBS:SeeedOPL-Switch-2016
LIBS:SeeedOPL-Transistor-2016
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Power Manager SMD"
Date "2017-12-12"
Rev "0.1"
Comp "UiO"
Comment1 "Jean Rabault"
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L +5V #PWR01
U 1 1 5A2F95A3
P 3100 2050
F 0 "#PWR01" H 3100 1900 50  0001 C CNN
F 1 "+5V" H 3100 2190 50  0000 C CNN
F 2 "" H 3100 2050 50  0000 C CNN
F 3 "" H 3100 2050 50  0000 C CNN
	1    3100 2050
	1    0    0    -1  
$EndComp
$Comp
L SMD-RES-47K-5%-1_10W_0603_ R4
U 1 1 5A2F9616
P 3350 2400
F 0 "R4" H 3200 2450 45  0000 L BNN
F 1 "SMD-RES-47K-5%-1_10W_0603_" H 2700 2300 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 3350 2400 40  0001 C CNN
F 3 "" H 3350 2400 40  0001 C CNN
F 4 "RC0603JR-0747KL" H 3380 2550 20  0001 C CNN "MPN"
F 5 "301010084" H 3380 2550 20  0001 C CNN "SKU"
	1    3350 2400
	1    0    0    -1  
$EndComp
$Comp
L DIP-CRYSTAL-16MHZ-18PF-30PPM-40R_HC49US_ Y1
U 1 1 5A2F9C2D
P 2650 3200
F 0 "Y1" H 2550 3300 45  0000 L BNN
F 1 "DIP-CRYSTAL-16MHZ-18PF-30PPM-40R_HC49US_" V 2450 1450 45  0000 L BNN
F 2 "SeeedOPL-Crystal Oscillator-2016:HC49US" H 2650 3200 60  0001 C CNN
F 3 "" H 2650 3200 60  0000 C CNN
F 4 "HC-49US    16.000MHz" H 2680 3350 20  0001 C CNN "MPN"
F 5 "306010014" H 2680 3350 20  0001 C CNN "SKU"
	1    2650 3200
	0    1    1    0   
$EndComp
$Comp
L CERAMIC-18PF-50V-5%-NPO_0402_ C2
U 1 1 5A2F9CA9
P 2150 3550
F 0 "C2" H 2000 3600 45  0000 L BNN
F 1 "CERAMIC-18PF-50V-5%-NPO_0402_" V 2300 3000 45  0000 L BNN
F 2 "SeeedOPL-Capacitor-2016:C0402" H 2150 3550 60  0001 C CNN
F 3 "" H 2150 3550 60  0000 C CNN
F 4 "CC0402JRNPO9BN180" H 2180 3700 20  0001 C CNN "MPN"
F 5 "302010016" H 2180 3700 20  0001 C CNN "SKU"
	1    2150 3550
	0    1    1    0   
$EndComp
$Comp
L CERAMIC-18PF-50V-5%-NPO_0402_ C1
U 1 1 5A2F9D78
P 1550 3550
F 0 "C1" H 1400 3600 45  0000 L BNN
F 1 "CERAMIC-18PF-50V-5%-NPO_0402_" V 1350 2650 45  0000 L BNN
F 2 "SeeedOPL-Capacitor-2016:C0402" H 1550 3550 60  0001 C CNN
F 3 "" H 1550 3550 60  0000 C CNN
F 4 "CC0402JRNPO9BN180" H 1580 3700 20  0001 C CNN "MPN"
F 5 "302010016" H 1580 3700 20  0001 C CNN "SKU"
	1    1550 3550
	0    1    1    0   
$EndComp
$Comp
L SCREW-TERMINAL-GREEN_2P-5.0_ J2
U 1 1 5A2F9FBA
P 9400 1350
F 0 "J2" H 9150 1600 45  0000 L BNN
F 1 "SCREW-TERMINAL-GREEN_2P-5.0_" H 9400 1600 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H2-5.0-10.5X8.0MM" H 9400 1350 60  0001 C CNN
F 3 "" H 9400 1350 60  0000 C CNN
F 4 "320110031" H 9430 1500 20  0001 C CNN "SKU"
	1    9400 1350
	1    0    0    -1  
$EndComp
Text Notes 9850 1400 0    60   ~ 0
solar panel\n
$Comp
L SCREW-TERMINAL-GREEN_2P-5.0_ J3
U 1 1 5A2FA031
P 9400 2100
F 0 "J3" H 9150 2350 45  0000 L BNN
F 1 "SCREW-TERMINAL-GREEN_2P-5.0_" H 9400 2350 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H2-5.0-10.5X8.0MM" H 9400 2100 60  0001 C CNN
F 3 "" H 9400 2100 60  0000 C CNN
F 4 "320110031" H 9430 2250 20  0001 C CNN "SKU"
	1    9400 2100
	1    0    0    -1  
$EndComp
Text Notes 9900 2150 0    60   ~ 0
Pololu
$Comp
L SCREW-TERMINAL-GREEN_2P-5.0_ J4
U 1 1 5A2FA076
P 9400 2800
F 0 "J4" H 9150 3050 45  0000 L BNN
F 1 "SCREW-TERMINAL-GREEN_2P-5.0_" H 9400 3050 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H2-5.0-10.5X8.0MM" H 9400 2800 60  0001 C CNN
F 3 "" H 9400 2800 60  0000 C CNN
F 4 "320110031" H 9430 2950 20  0001 C CNN "SKU"
	1    9400 2800
	1    0    0    -1  
$EndComp
Text Notes 9900 2850 0    60   ~ 0
battery
$Comp
L SCREW-TERMINAL-GREEN_2P-5.0_ J5
U 1 1 5A2FA152
P 9400 3550
F 0 "J5" H 9150 3800 45  0000 L BNN
F 1 "SCREW-TERMINAL-GREEN_2P-5.0_" H 9400 3800 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H2-5.0-10.5X8.0MM" H 9400 3550 60  0001 C CNN
F 3 "" H 9400 3550 60  0000 C CNN
F 4 "320110031" H 9430 3700 20  0001 C CNN "SKU"
	1    9400 3550
	1    0    0    -1  
$EndComp
Text Notes 9900 3600 0    60   ~ 0
supercaps
$Comp
L SCREW-TERMINAL-GREEN_3P-5.0_ J6
U 1 1 5A2FA1AD
P 9400 4300
F 0 "J6" H 9100 4550 45  0000 L BNN
F 1 "SCREW-TERMINAL-GREEN_3P-5.0_" H 9350 4550 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H3-5.0-15.5X8.0MM" H 9400 4300 60  0001 C CNN
F 3 "" H 9400 4300 60  0000 C CNN
F 4 "320110029" H 9430 4450 20  0001 C CNN "SKU"
	1    9400 4300
	1    0    0    -1  
$EndComp
Text Notes 9950 4350 0    60   ~ 0
Mega
Text Label 6250 3750 0    60   ~ 0
CMD_MEGA
$Comp
L SMD-MOSFET-N-CH-20V-2.1A-CJ2302_SOT-23_ Q2
U 1 1 5A2FAB41
P 6400 5850
F 0 "Q2" H 6250 5950 45  0000 L BNN
F 1 "SMD-MOSFET-N-CH-20V-2.1A-CJ2302_SOT-23_" H 6600 6050 45  0000 L BNN
F 2 "SeeedOPL-Transistor-2016:SOT-23" H 6400 5850 60  0001 C CNN
F 3 "" H 6400 5850 60  0000 C CNN
F 4 "CJ2302" H 6430 6000 20  0001 C CNN "MPN"
F 5 "305030015" H 6430 6000 20  0001 C CNN "SKU"
	1    6400 5850
	1    0    0    -1  
$EndComp
$Comp
L SMD-MOSFET-N-CH-20V-2.1A-CJ2302_SOT-23_ Q1
U 1 1 5A2FABA2
P 3500 5850
F 0 "Q1" H 3350 5950 45  0000 L BNN
F 1 "SMD-MOSFET-N-CH-20V-2.1A-CJ2302_SOT-23_" H 3700 6050 45  0000 L BNN
F 2 "SeeedOPL-Transistor-2016:SOT-23" H 3500 5850 60  0001 C CNN
F 3 "" H 3500 5850 60  0000 C CNN
F 4 "CJ2302" H 3530 6000 20  0001 C CNN "MPN"
F 5 "305030015" H 3530 6000 20  0001 C CNN "SKU"
	1    3500 5850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 5A2FB002
P 3500 6350
F 0 "#PWR02" H 3500 6100 50  0001 C CNN
F 1 "GND" H 3500 6200 50  0000 C CNN
F 2 "" H 3500 6350 50  0000 C CNN
F 3 "" H 3500 6350 50  0000 C CNN
	1    3500 6350
	1    0    0    -1  
$EndComp
Text Label 8950 1450 2    60   ~ 0
SOL_-
Text Label 8950 1250 2    60   ~ 0
SOL_+
Text Label 6400 6350 0    60   ~ 0
SOL_-
Text Label 5900 5850 2    60   ~ 0
CMD_SOLAR
Text Label 2950 5850 2    60   ~ 0
CMD_MEGA
$Comp
L SMD-RES-47K-5%-1_10W_0603_ R3
U 1 1 5A2FB718
P 3100 6150
F 0 "R3" H 2950 6200 45  0000 L BNN
F 1 "SMD-RES-47K-5%-1_10W_0603_" H 2950 6050 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 3100 6150 40  0001 C CNN
F 3 "" H 3100 6150 40  0001 C CNN
F 4 "RC0603JR-0747KL" H 3130 6300 20  0001 C CNN "MPN"
F 5 "301010084" H 3130 6300 20  0001 C CNN "SKU"
	1    3100 6150
	0    1    1    0   
$EndComp
$Comp
L SMD-RES-47K-5%-1_10W_0603_ R6
U 1 1 5A2FB7F9
P 6050 6100
F 0 "R6" H 5900 6150 45  0000 L BNN
F 1 "SMD-RES-47K-5%-1_10W_0603_" H 5900 6000 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 6050 6100 40  0001 C CNN
F 3 "" H 6050 6100 40  0001 C CNN
F 4 "RC0603JR-0747KL" H 6080 6250 20  0001 C CNN "MPN"
F 5 "301010084" H 6080 6250 20  0001 C CNN "SKU"
	1    6050 6100
	0    1    1    0   
$EndComp
Text Label 8800 4300 2    60   ~ 0
MEGA_-
Text Label 8800 4450 2    60   ~ 0
feedback_MEGA
Text Label 3500 5450 0    60   ~ 0
MEGA_-
Text Label 8950 2700 2    60   ~ 0
BAT_+
Text Label 8950 2900 2    60   ~ 0
BAT_-
Text Label 6400 5450 0    60   ~ 0
BAT_-
$Comp
L GND #PWR03
U 1 1 5A2FC579
P 9100 3000
F 0 "#PWR03" H 9100 2750 50  0001 C CNN
F 1 "GND" H 9100 2850 50  0000 C CNN
F 2 "" H 9100 3000 50  0000 C CNN
F 3 "" H 9100 3000 50  0000 C CNN
	1    9100 3000
	1    0    0    -1  
$EndComp
Text Label 7150 1300 0    60   ~ 0
SOL_+
Text Label 6700 1300 2    60   ~ 0
BAT_+
$Comp
L +5V #PWR04
U 1 1 5A2FCE46
P 8950 1900
F 0 "#PWR04" H 8950 1750 50  0001 C CNN
F 1 "+5V" H 8950 2040 50  0000 C CNN
F 2 "" H 8950 1900 50  0000 C CNN
F 3 "" H 8950 1900 50  0000 C CNN
	1    8950 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR05
U 1 1 5A2FCE76
P 8950 2300
F 0 "#PWR05" H 8950 2050 50  0001 C CNN
F 1 "GND" H 8950 2150 50  0000 C CNN
F 2 "" H 8950 2300 50  0000 C CNN
F 3 "" H 8950 2300 50  0000 C CNN
	1    8950 2300
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR06
U 1 1 5A2FD102
P 8900 3400
F 0 "#PWR06" H 8900 3250 50  0001 C CNN
F 1 "+5V" H 8900 3540 50  0000 C CNN
F 2 "" H 8900 3400 50  0000 C CNN
F 3 "" H 8900 3400 50  0000 C CNN
	1    8900 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR07
U 1 1 5A2FD132
P 8900 3700
F 0 "#PWR07" H 8900 3450 50  0001 C CNN
F 1 "GND" H 8900 3550 50  0000 C CNN
F 2 "" H 8900 3700 50  0000 C CNN
F 3 "" H 8900 3700 50  0000 C CNN
	1    8900 3700
	1    0    0    -1  
$EndComp
$Comp
L SMD-RES-10K-1%-1_10W_0603_ R1
U 1 1 5A2FD788
P 1200 5550
F 0 "R1" H 1050 5600 45  0000 L BNN
F 1 "SMD-RES-10K-1%-1_10W_0603_" H 1050 5450 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 1200 5550 40  0001 C CNN
F 3 "" H 1200 5550 40  0001 C CNN
F 4 "RC0603FR-0710KL" H 1230 5700 20  0001 C CNN "MPN"
F 5 "301010299" H 1230 5700 20  0001 C CNN "SKU"
	1    1200 5550
	0    -1   -1   0   
$EndComp
$Comp
L SMD-RES-6.8K-1%-1_10W_0603_ R2
U 1 1 5A2FD826
P 1200 6100
F 0 "R2" H 1050 6150 45  0000 L BNN
F 1 "SMD-RES-6.8K-1%-1_10W_0603_" H 1050 6000 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 1200 6100 40  0001 C CNN
F 3 "" H 1200 6100 40  0001 C CNN
F 4 "RC0805JR-07220KL" H 1230 6250 20  0001 C CNN "MPN"
F 5 "301010198" H 1230 6250 20  0001 C CNN "SKU"
	1    1200 6100
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR08
U 1 1 5A2FD885
P 950 5000
F 0 "#PWR08" H 950 4850 50  0001 C CNN
F 1 "+5V" H 950 5140 50  0000 C CNN
F 2 "" H 950 5000 50  0000 C CNN
F 3 "" H 950 5000 50  0000 C CNN
	1    950  5000
	1    0    0    -1  
$EndComp
Text Label 1450 6500 0    60   ~ 0
SOL_-
Text Label 1550 5850 0    60   ~ 0
MEAS_SOL
Text Label 6250 2500 0    60   ~ 0
MEAS_SOL
Text Label 6250 2400 0    60   ~ 0
BAT_+
$Comp
L DIP-BLACK-MALE-HEADER_2X3P-2.54) J1
U 1 1 5A2FE208
P 7000 1900
F 0 "J1" H 6750 2150 45  0000 L BNN
F 1 "DIP-BLACK-MALE-HEADER_2X3P-2.54)" H 6150 2250 45  0000 L BNN
F 2 "SeeedOPL-Connector-2016:H2X3-2.54" H 7000 1900 60  0001 C CNN
F 3 "" H 7000 1900 60  0000 C CNN
F 4 "P125-12A1BS116A1" H 7030 2050 20  0001 C CNN "MPN"
F 5 "320020077" H 7030 2050 20  0001 C CNN "SKU"
	1    7000 1900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR09
U 1 1 5A2FE47E
P 7600 2200
F 0 "#PWR09" H 7600 1950 50  0001 C CNN
F 1 "GND" H 7600 2050 50  0000 C CNN
F 2 "" H 7600 2200 50  0000 C CNN
F 3 "" H 7600 2200 50  0000 C CNN
	1    7600 2200
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR010
U 1 1 5A2FE4FA
P 7600 1700
F 0 "#PWR010" H 7600 1550 50  0001 C CNN
F 1 "+5V" H 7600 1840 50  0000 C CNN
F 2 "" H 7600 1700 50  0000 C CNN
F 3 "" H 7600 1700 50  0000 C CNN
	1    7600 1700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P2
U 1 1 5A2FF381
P 6300 3350
F 0 "P2" H 6450 3350 50  0000 C CNN
F 1 "CONN_01X02" H 6750 3300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 6300 3350 50  0001 C CNN
F 3 "" H 6300 3350 50  0000 C CNN
	1    6300 3350
	1    0    0    -1  
$EndComp
Text Notes 6650 3300 0    60   ~ 0
Serial\n
Text Notes 6600 2800 0    60   ~ 0
free_ADC
$Comp
L CONN_01X06 P3
U 1 1 5A2FF8E0
P 6600 4700
F 0 "P3" H 6600 5050 50  0000 C CNN
F 1 "CONN_01X06" V 6700 4700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 6600 4700 50  0001 C CNN
F 3 "" H 6600 4700 50  0000 C CNN
	1    6600 4700
	1    0    0    -1  
$EndComp
Text Notes 6900 4700 0    60   ~ 0
free_DPINS
$Comp
L PWR_FLAG #FLG011
U 1 1 5A300AE6
P 1050 1050
F 0 "#FLG011" H 1050 1145 50  0001 C CNN
F 1 "PWR_FLAG" H 1050 1230 50  0000 C CNN
F 2 "" H 1050 1050 50  0000 C CNN
F 3 "" H 1050 1050 50  0000 C CNN
	1    1050 1050
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG012
U 1 1 5A300B24
P 1700 1050
F 0 "#FLG012" H 1700 1145 50  0001 C CNN
F 1 "PWR_FLAG" H 1700 1230 50  0000 C CNN
F 2 "" H 1700 1050 50  0000 C CNN
F 3 "" H 1700 1050 50  0000 C CNN
	1    1700 1050
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR013
U 1 1 5A300BB6
P 1050 1300
F 0 "#PWR013" H 1050 1150 50  0001 C CNN
F 1 "+5V" H 1050 1440 50  0000 C CNN
F 2 "" H 1050 1300 50  0000 C CNN
F 3 "" H 1050 1300 50  0000 C CNN
	1    1050 1300
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR014
U 1 1 5A300C84
P 1700 1300
F 0 "#PWR014" H 1700 1050 50  0001 C CNN
F 1 "GND" H 1700 1150 50  0000 C CNN
F 2 "" H 1700 1300 50  0000 C CNN
F 3 "" H 1700 1300 50  0000 C CNN
	1    1700 1300
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR015
U 1 1 5A302696
P 8800 4150
F 0 "#PWR015" H 8800 4000 50  0001 C CNN
F 1 "+5V" H 8800 4290 50  0000 C CNN
F 2 "" H 8800 4150 50  0000 C CNN
F 3 "" H 8800 4150 50  0000 C CNN
	1    8800 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 5A3037FB
P 1850 4000
F 0 "#PWR016" H 1850 3750 50  0001 C CNN
F 1 "GND" H 1850 3850 50  0000 C CNN
F 2 "" H 1850 4000 50  0000 C CNN
F 3 "" H 1850 4000 50  0000 C CNN
	1    1850 4000
	1    0    0    -1  
$EndComp
$Comp
L MCU-ATMEGA328P-AU_TQFP32_ U1
U 1 1 5A2FC455
P 4650 3550
F 0 "U1" H 3900 4800 45  0000 L BNN
F 1 "MCU-ATMEGA328P-AU_TQFP32_" H 4150 4800 45  0000 L BNN
F 2 "SeeedOPL-IC-2016:TQFP32-0.8-7X7MM" H 4650 3550 60  0001 C CNN
F 3 "" H 4650 3550 60  0000 C CNN
F 4 "ATMEGA328P-AU" H 4680 3700 20  0001 C CNN "MPN"
F 5 "310010049" H 4680 3700 20  0001 C CNN "SKU"
	1    4650 3550
	1    0    0    -1  
$EndComp
NoConn ~ 3700 2700
$Comp
L +5V #PWR017
U 1 1 5A2FCC05
P 3100 2750
F 0 "#PWR017" H 3100 2600 50  0001 C CNN
F 1 "+5V" H 3100 2890 50  0000 C CNN
F 2 "" H 3100 2750 50  0000 C CNN
F 3 "" H 3100 2750 50  0000 C CNN
	1    3100 2750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 5A2FCC43
P 3300 4150
F 0 "#PWR018" H 3300 3900 50  0001 C CNN
F 1 "GND" H 3300 4000 50  0000 C CNN
F 2 "" H 3300 4150 50  0000 C CNN
F 3 "" H 3300 4150 50  0000 C CNN
	1    3300 4150
	1    0    0    -1  
$EndComp
Text Label 6250 3650 0    60   ~ 0
feedback_MEGA
Text Label 6250 3550 0    60   ~ 0
CMD_SOLAR
$Comp
L CONN_01X06 P1
U 1 1 5A2FDF7B
P 6300 2850
F 0 "P1" H 6450 2850 50  0000 C CNN
F 1 "CONN_01X06" H 6650 2700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x06" H 6300 2850 50  0001 C CNN
F 3 "" H 6300 2850 50  0000 C CNN
	1    6300 2850
	1    0    0    -1  
$EndComp
Text Label 6300 1800 2    60   ~ 0
MISO
Text Label 5750 4900 0    60   ~ 0
SCK
Text Label 6300 1900 2    60   ~ 0
SCK
Text Label 7600 1900 0    60   ~ 0
MOSI
Text Label 5750 4700 0    60   ~ 0
MOSI
Text Label 5750 4800 0    60   ~ 0
MISO
$Comp
L SMD-RES-47K-5%-1_10W_0603_ R7
U 1 1 5A304A42
P 9050 4800
F 0 "R7" H 8900 4850 45  0000 L BNN
F 1 "SMD-RES-47K-5%-1_10W_0603_" H 8900 4600 45  0000 L BNN
F 2 "SeeedOPL-Resistor-2016:R0603" H 9050 4800 40  0001 C CNN
F 3 "" H 9050 4800 40  0001 C CNN
F 4 "RC0603JR-0747KL" H 9080 4950 20  0001 C CNN "MPN"
F 5 "301010084" H 9080 4950 20  0001 C CNN "SKU"
	1    9050 4800
	0    1    1    0   
$EndComp
$Comp
L GND #PWR019
U 1 1 5A304AA3
P 9050 5200
F 0 "#PWR019" H 9050 4950 50  0001 C CNN
F 1 "GND" H 9050 5050 50  0000 C CNN
F 2 "" H 9050 5200 50  0000 C CNN
F 3 "" H 9050 5200 50  0000 C CNN
	1    9050 5200
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 5200 9050 4950
Connection ~ 9050 4450
Wire Wire Line
	9050 4650 9050 4450
Wire Wire Line
	6150 4950 6400 4950
Wire Wire Line
	6150 4400 6150 4950
Wire Wire Line
	5600 4400 6150 4400
Wire Wire Line
	6200 4850 6400 4850
Wire Wire Line
	6200 4300 6200 4850
Wire Wire Line
	5600 4300 6200 4300
Wire Wire Line
	6250 4750 6400 4750
Wire Wire Line
	6250 4200 6250 4750
Wire Wire Line
	5600 4200 6250 4200
Wire Wire Line
	6300 4650 6400 4650
Wire Wire Line
	6300 4000 6300 4650
Wire Wire Line
	5600 4000 6300 4000
Wire Wire Line
	6350 4550 6400 4550
Wire Wire Line
	6350 3900 6350 4550
Wire Wire Line
	5600 3900 6350 3900
Wire Wire Line
	6400 3800 6400 4450
Wire Wire Line
	5600 3800 6400 3800
Wire Wire Line
	5700 4700 5750 4700
Wire Wire Line
	5700 4500 5700 4700
Wire Wire Line
	5600 4500 5700 4500
Wire Wire Line
	5650 4800 5750 4800
Wire Wire Line
	5650 4600 5650 4800
Wire Wire Line
	5600 4600 5650 4600
Wire Wire Line
	5600 4900 5750 4900
Wire Wire Line
	5600 4700 5600 4900
Wire Wire Line
	3500 2400 3700 2400
Wire Wire Line
	3200 2400 3100 2400
Wire Wire Line
	3100 2400 3100 2050
Wire Wire Line
	1550 3050 3600 3050
Wire Wire Line
	2150 3350 3600 3350
Wire Wire Line
	1550 3050 1550 3400
Connection ~ 2650 3050
Wire Wire Line
	2150 3350 2150 3400
Connection ~ 2650 3350
Wire Wire Line
	1550 3700 1550 4000
Wire Wire Line
	2150 4000 2150 3700
Wire Wire Line
	3500 6050 3500 6350
Wire Wire Line
	8950 1450 9200 1450
Wire Wire Line
	8950 1250 9200 1250
Wire Wire Line
	6400 6050 6400 6350
Wire Wire Line
	5900 5850 6200 5850
Wire Wire Line
	2950 5850 3300 5850
Wire Wire Line
	3100 6000 3100 5850
Connection ~ 3100 5850
Wire Wire Line
	3100 6300 3500 6300
Connection ~ 3500 6300
Wire Wire Line
	6050 5950 6050 5850
Connection ~ 6050 5850
Wire Wire Line
	6050 6250 6400 6250
Connection ~ 6400 6250
Wire Wire Line
	8800 4450 9100 4450
Wire Wire Line
	8800 4300 9100 4300
Wire Wire Line
	3500 5450 3500 5650
Wire Wire Line
	8950 2900 9200 2900
Wire Wire Line
	8950 2700 9200 2700
Wire Wire Line
	6400 5450 6400 5650
Wire Wire Line
	9100 3000 9100 2900
Connection ~ 9100 2900
Wire Wire Line
	6700 1300 7150 1300
Wire Wire Line
	8950 2300 8950 2200
Wire Wire Line
	8950 2200 9200 2200
Wire Wire Line
	9200 2000 8950 2000
Wire Wire Line
	8950 2000 8950 1900
Wire Wire Line
	8900 3700 8900 3650
Wire Wire Line
	8900 3650 9200 3650
Wire Wire Line
	9200 3450 8900 3450
Wire Wire Line
	8900 3450 8900 3400
Wire Wire Line
	950  5000 950  5250
Wire Wire Line
	950  5250 1200 5250
Wire Wire Line
	1200 5250 1200 5400
Wire Wire Line
	1200 5700 1200 5950
Wire Wire Line
	1200 6250 1200 6500
Wire Wire Line
	1200 6500 1450 6500
Wire Wire Line
	1550 5850 1200 5850
Connection ~ 1200 5850
Wire Wire Line
	6300 1800 6650 1800
Wire Wire Line
	6300 1900 6650 1900
Wire Wire Line
	7600 1700 7600 1800
Wire Wire Line
	7600 1800 7350 1800
Wire Wire Line
	7350 2000 7600 2000
Wire Wire Line
	7600 2000 7600 2200
Wire Wire Line
	7350 1900 7600 1900
Connection ~ 3550 2400
Wire Wire Line
	1700 1300 1700 1050
Wire Wire Line
	1050 1300 1050 1050
Wire Wire Line
	9100 4150 8800 4150
Wire Wire Line
	1550 4000 2150 4000
Connection ~ 1850 4000
Wire Wire Line
	3600 3050 3600 3100
Wire Wire Line
	3600 3100 3700 3100
Wire Wire Line
	3600 3350 3600 3300
Wire Wire Line
	3600 3300 3700 3300
Wire Wire Line
	3550 2400 3550 2000
Wire Wire Line
	3550 2000 6650 2000
Wire Wire Line
	3100 2750 3100 3900
Wire Wire Line
	3100 2800 3700 2800
Wire Wire Line
	3100 3800 3700 3800
Connection ~ 3100 2800
Wire Wire Line
	3100 3900 3700 3900
Connection ~ 3100 3800
Wire Wire Line
	3300 2600 3700 2600
Wire Wire Line
	3300 2600 3300 4150
Wire Wire Line
	3700 3600 3300 3600
Connection ~ 3300 3600
Wire Wire Line
	3300 3700 3700 3700
Connection ~ 3300 3700
Wire Wire Line
	5600 2400 6250 2400
Wire Wire Line
	6250 2500 5600 2500
Wire Wire Line
	5600 3100 6100 3100
Wire Wire Line
	5600 3000 6100 3000
Wire Wire Line
	5600 2900 6100 2900
Wire Wire Line
	5600 2800 6100 2800
Wire Wire Line
	5600 2700 6100 2700
Wire Wire Line
	5600 2600 6100 2600
Wire Wire Line
	6100 3300 5600 3300
Wire Wire Line
	5600 3400 6100 3400
Wire Wire Line
	5600 3500 5600 3550
Wire Wire Line
	5600 3550 6250 3550
Wire Wire Line
	5600 3600 5600 3650
Wire Wire Line
	5600 3650 6250 3650
Wire Wire Line
	5600 3700 5600 3750
Wire Wire Line
	5600 3750 6250 3750
$EndSCHEMATC