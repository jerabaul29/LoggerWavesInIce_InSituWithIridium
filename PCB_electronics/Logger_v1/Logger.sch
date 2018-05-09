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
LIBS:arduino
LIBS:+1.8V
LIBS:+3.3V
LIBS:+5V
LIBS:+9V
LIBS:25AA02EXXX-XOT
LIBS:74LVC8T245
LIBS:4021
LIBS:7408
LIBS:74125
LIBS:74138
LIBS:74148
LIBS:74151
LIBS:74157
LIBS:74541
LIBS:ANT
LIBS:AP6502
LIBS:AUDIO-JACK-3
LIBS:AUDIO-JACK-6
LIBS:BATTERY
LIBS:BELFUSE-08B0-1X1T-06-F
LIBS:C
LIBS:CP
LIBS:CRYSTAL
LIBS:CRYSTAL-14
LIBS:CVAR
LIBS:DIODE
LIBS:DIODES-AP3211
LIBS:DIODESCH
LIBS:DIODEZENER
LIBS:DMMT5401
LIBS:EDISON-BULB-E10
LIBS:EDISON-SOCKET-E10
LIBS:ENC28J60
LIBS:FTDI-PLUG
LIBS:GNDPWR
LIBS:~GNDPWR
LIBS:INDUCTOR
LIBS:INTEL-EDISON-COMPUTE-MODULE
LIBS:IS31AP4066D
LIBS:JTAG
LIBS:JTAG10
LIBS:LATTICE-LC4256
LIBS:LED
LIBS:LED-BI
LIBS:LED-BI-COMMON-CATHODE
LIBS:LM324
LIBS:LM1875
LIBS:MAX3221
LIBS:MC33926
LIBS:MC34063A
LIBS:MCP49X1
LIBS:MCP3204
LIBS:MECH
LIBS:MIC2039
LIBS:MIC2288
LIBS:MICROSD-AMPHENOL-114-00841-68
LIBS:MOSFET-N
LIBS:MOSFET-N-123
LIBS:MOSFET-N-134
LIBS:MOSFET-P
LIBS:NPN
LIBS:nRF8001
LIBS:NXPUDA1334ATS
LIBS:OPAMP-DUAL
LIBS:P01
LIBS:P02
LIBS:P03
LIBS:P05
LIBS:P06
LIBS:P06-MOTOR
LIBS:P08
LIBS:P10
LIBS:P22
LIBS:P24
LIBS:PHOTO_TRANSISTOR
LIBS:PIEZO
LIBS:PNP
LIBS:POLYFUSE
LIBS:POT
LIBS:PWR_FLAG
LIBS:QRE1113
LIBS:R
LIBS:RASPBERRYPI
LIBS:RASPBERRYPI2
LIBS:RBUS8
LIBS:RN4020
LIBS:ROBOT-P10-IO
LIBS:RS-232-DB-9-DTE
LIBS:SEN-SHT3x-DIS
LIBS:SERVO
LIBS:SI4737-C40-GM
LIBS:SI4737-GU
LIBS:SKYWORKS-AAT1217-3.3
LIBS:SPDT
LIBS:SPST
LIBS:SPST-4PIN
LIBS:SST25VF032B-S2A
LIBS:SST26VF032B
LIBS:ST-LIS3MDL
LIBS:STM32-32PIN
LIBS:STM32-48PIN
LIBS:STM32-64PIN
LIBS:STM32SWDUART
LIBS:ST-SWD
LIBS:ST-TDA7266
LIBS:SWD
LIBS:TCS3472
LIBS:TI-ADS7866
LIBS:TICC3000
LIBS:TIREF30XX
LIBS:TP
LIBS:TS5A623157
LIBS:TSV631
LIBS:TVS-DIODE
LIBS:TXB0108-PW
LIBS:USBAB
LIBS:Vcc
LIBS:Vdd
LIBS:VIN
LIBS:VREG
LIBS:VREG_VOUTCENTER
LIBS:FQP27P06
LIBS:Logger-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
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
$EndDescr
$Comp
L Arduino_Mega2560_Shield XA1
U 1 1 5AEB1BE3
P 2600 3750
F 0 "XA1" V 2700 3750 60  0000 C CNN
F 1 "Arduino_Mega2560_Shield" V 2500 3750 60  0000 C CNN
F 2 "Arduino:Arduino_Mega2560_Shield" H 3300 6500 60  0001 C CNN
F 3 "" H 3300 6500 60  0001 C CNN
	1    2600 3750
	1    0    0    1   
$EndComp
$Comp
L CONN_01X09 P1
U 1 1 5AEC3FFC
P 6350 1150
F 0 "P1" H 6350 1650 50  0000 C CNN
F 1 "GPS_breakout_connector" V 6450 1150 50  0000 C CNN
F 2 "" H 6350 1150 50  0001 C CNN
F 3 "" H 6350 1150 50  0000 C CNN
	1    6350 1150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P2
U 1 1 5AEC40A7
P 6350 2250
F 0 "P2" H 6350 2700 50  0000 C CNN
F 1 "SD_breakout_connector" V 6450 2250 50  0000 C CNN
F 2 "" H 6350 2250 50  0001 C CNN
F 3 "" H 6350 2250 50  0000 C CNN
	1    6350 2250
	1    0    0    -1  
$EndComp
$Comp
L MAX232 U1
U 1 1 5AEC4111
P 6450 5450
F 0 "U1" H 6000 6300 50  0000 L CNN
F 1 "MAX232" H 6650 6300 50  0000 L CNN
F 2 "" H 6450 5450 50  0001 C CNN
F 3 "" H 6450 5450 50  0000 C CNN
	1    6450 5450
	1    0    0    -1  
$EndComp
$Comp
L FQP27P06 U2
U 1 1 5AEC586F
P 8050 3050
F 0 "U2" H 8250 2950 60  0000 C CNN
F 1 "FQP27P06" H 8400 3150 60  0000 C CNN
F 2 "" H 8050 3050 60  0001 C CNN
F 3 "" H 8050 3050 60  0000 C CNN
	1    8050 3050
	1    0    0    -1  
$EndComp
$Comp
L CP C1
U 1 1 5AEC5993
P 800 2950
F 0 "C1" H 825 3050 50  0000 L CNN
F 1 "1uF" H 825 2850 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 838 2800 50  0001 C CNN
F 3 "" H 800 2950 50  0000 C CNN
	1    800  2950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P4
U 1 1 5AEC59F8
P 6350 3950
F 0 "P4" H 6350 4200 50  0000 C CNN
F 1 "VN100_connector" V 6450 3950 50  0000 C CNN
F 2 "" H 6350 3950 50  0001 C CNN
F 3 "" H 6350 3950 50  0000 C CNN
	1    6350 3950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P7
U 1 1 5AEC5A65
P 8900 3650
F 0 "P7" H 8900 3800 50  0000 C CNN
F 1 "RPI_connector" V 9000 3650 50  0000 C CNN
F 2 "" H 8900 3650 50  0001 C CNN
F 3 "" H 8900 3650 50  0000 C CNN
	1    8900 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P5
U 1 1 5AEC5B07
P 8300 1100
F 0 "P5" H 8300 1300 50  0000 C CNN
F 1 "PowerManager_connector" V 8400 1100 50  0000 C CNN
F 2 "" H 8300 1100 50  0001 C CNN
F 3 "" H 8300 1100 50  0000 C CNN
	1    8300 1100
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X05 P3
U 1 1 5AEC5B68
P 6350 3150
F 0 "P3" H 6350 3450 50  0000 C CNN
F 1 "Iridium_connector" V 6450 3150 50  0000 C CNN
F 2 "" H 6350 3150 50  0001 C CNN
F 3 "" H 6350 3150 50  0000 C CNN
	1    6350 3150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 P6
U 1 1 5AEC5C02
P 8300 1950
F 0 "P6" H 8300 2050 50  0000 C CNN
F 1 "BATT_connector" V 8400 1950 50  0000 C CNN
F 2 "" H 8300 1950 50  0001 C CNN
F 3 "" H 8300 1950 50  0000 C CNN
	1    8300 1950
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR01
U 1 1 5AEC655D
P 1250 1150
F 0 "#PWR01" H 1250 1000 50  0001 C CNN
F 1 "+5V" H 1250 1290 50  0000 C CNN
F 2 "" H 1250 1150 50  0000 C CNN
F 3 "" H 1250 1150 50  0000 C CNN
	1    1250 1150
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR02
U 1 1 5AEC65BE
P 750 1150
F 0 "#PWR02" H 750 900 50  0001 C CNN
F 1 "GND" H 750 1000 50  0000 C CNN
F 2 "" H 750 1150 50  0000 C CNN
F 3 "" H 750 1150 50  0000 C CNN
	1    750  1150
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG03
U 1 1 5AEC662B
P 750 1150
F 0 "#FLG03" H 750 1245 50  0001 C CNN
F 1 "PWR_FLAG" H 750 1330 50  0000 C CNN
F 2 "" H 750 1150 50  0000 C CNN
F 3 "" H 750 1150 50  0000 C CNN
	1    750  1150
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG04
U 1 1 5AEC6655
P 1250 1150
F 0 "#FLG04" H 1250 1245 50  0001 C CNN
F 1 "PWR_FLAG" H 1250 1330 50  0000 C CNN
F 2 "" H 1250 1150 50  0000 C CNN
F 3 "" H 1250 1150 50  0000 C CNN
	1    1250 1150
	1    0    0    -1  
$EndComp
$Comp
L +5V #PWR05
U 1 1 5AEC673A
P 8000 1000
F 0 "#PWR05" H 8000 850 50  0001 C CNN
F 1 "+5V" H 8000 1140 50  0000 C CNN
F 2 "" H 8000 1000 50  0000 C CNN
F 3 "" H 8000 1000 50  0000 C CNN
	1    8000 1000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR06
U 1 1 5AEC6764
P 7800 1100
F 0 "#PWR06" H 7800 850 50  0001 C CNN
F 1 "GND" H 7800 950 50  0000 C CNN
F 2 "" H 7800 1100 50  0000 C CNN
F 3 "" H 7800 1100 50  0000 C CNN
	1    7800 1100
	1    0    0    -1  
$EndComp
Text Label 7400 1450 0    60   ~ 0
MEGA_feedback
Wire Wire Line
	7400 1450 8100 1450
Wire Wire Line
	8100 1450 8100 1200
Wire Wire Line
	8100 1100 7800 1100
Wire Wire Line
	8100 1000 8000 1000
Text Label 7650 1950 0    60   ~ 0
BATT_+
Wire Wire Line
	7650 1950 8100 1950
$Comp
L CP C2
U 1 1 5AEC687B
P 8450 3650
F 0 "C2" H 8475 3750 50  0000 L CNN
F 1 "1000uF" H 8475 3550 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 8488 3500 50  0001 C CNN
F 3 "" H 8450 3650 50  0000 C CNN
	1    8450 3650
	1    0    0    -1  
$EndComp
Wire Wire Line
	8700 3400 8700 3600
Wire Wire Line
	8050 3400 8700 3400
Wire Wire Line
	8450 3400 8450 3500
Wire Wire Line
	8450 3800 8450 3900
Wire Wire Line
	8050 3900 8700 3900
Wire Wire Line
	8700 3700 8700 4100
$Comp
L GND #PWR07
U 1 1 5AEC68E3
P 8700 4100
F 0 "#PWR07" H 8700 3850 50  0001 C CNN
F 1 "GND" H 8700 3950 50  0000 C CNN
F 2 "" H 8700 4100 50  0000 C CNN
F 3 "" H 8700 4100 50  0000 C CNN
	1    8700 4100
	1    0    0    -1  
$EndComp
Connection ~ 8700 3900
Connection ~ 8450 3400
Text Label 6850 3050 0    60   ~ 0
RPI_control
Wire Wire Line
	6850 3050 7700 3050
$Comp
L +5V #PWR08
U 1 1 5AEC6E73
P 8050 2550
F 0 "#PWR08" H 8050 2400 50  0001 C CNN
F 1 "+5V" H 8050 2690 50  0000 C CNN
F 2 "" H 8050 2550 50  0000 C CNN
F 3 "" H 8050 2550 50  0000 C CNN
	1    8050 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 2550 8050 2700
$Comp
L R R1
U 1 1 5AEC6EBD
P 7700 2650
F 0 "R1" V 7780 2650 50  0000 C CNN
F 1 "46k" V 7700 2650 50  0000 C CNN
F 2 "" V 7630 2650 50  0001 C CNN
F 3 "" H 7700 2650 50  0000 C CNN
	1    7700 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	7850 2650 8050 2650
Connection ~ 8050 2650
Wire Wire Line
	7550 2650 7500 2650
Wire Wire Line
	7500 2650 7500 3050
Connection ~ 7500 3050
$Comp
L R R2
U 1 1 5AEC722B
P 8050 3700
F 0 "R2" V 8130 3700 50  0000 C CNN
F 1 "32k" V 8050 3700 50  0000 C CNN
F 2 "" V 7980 3700 50  0001 C CNN
F 3 "" H 8050 3700 50  0000 C CNN
	1    8050 3700
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 3400 8050 3550
Wire Wire Line
	8050 3850 8050 3900
Connection ~ 8450 3900
$Comp
L GND #PWR09
U 1 1 5AEC7323
P 800 3100
F 0 "#PWR09" H 800 2850 50  0001 C CNN
F 1 "GND" H 800 2950 50  0000 C CNN
F 2 "" H 800 3100 50  0000 C CNN
F 3 "" H 800 3100 50  0000 C CNN
	1    800  3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  2800 1300 2800
NoConn ~ 1300 1600
$Comp
L +5V #PWR010
U 1 1 5AEC741F
P 1150 1700
F 0 "#PWR010" H 1150 1550 50  0001 C CNN
F 1 "+5V" H 1150 1840 50  0000 C CNN
F 2 "" H 1150 1700 50  0000 C CNN
F 3 "" H 1150 1700 50  0000 C CNN
	1    1150 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 1700 1300 1700
Wire Wire Line
	1300 1700 1300 1900
Connection ~ 1300 1800
Connection ~ 1300 1700
Connection ~ 1300 1900
$Comp
L GND #PWR011
U 1 1 5AEC74D1
P 1150 2550
F 0 "#PWR011" H 1150 2300 50  0001 C CNN
F 1 "GND" H 1150 2400 50  0000 C CNN
F 2 "" H 1150 2550 50  0000 C CNN
F 3 "" H 1150 2550 50  0000 C CNN
	1    1150 2550
	1    0    0    -1  
$EndComp
Wire Wire Line
	1150 2550 1150 2500
Wire Wire Line
	1150 2500 1300 2500
Connection ~ 1300 2400
Connection ~ 1300 2300
Connection ~ 1300 2200
Wire Wire Line
	1300 2500 1300 2100
Connection ~ 1300 2100
Connection ~ 1300 2500
Connection ~ 8050 3400
NoConn ~ 1300 2700
NoConn ~ 1300 2000
NoConn ~ 1300 3000
Text Label 4050 2300 0    60   ~ 0
RPI_control
Wire Wire Line
	4050 2300 3900 2300
Text Label 800  4600 0    60   ~ 0
BATT_+
Wire Wire Line
	800  4600 1300 4600
Text Label 4050 2100 0    60   ~ 0
MEGA_feedback
Wire Wire Line
	4050 2100 3900 2100
$Comp
L +5V #PWR012
U 1 1 5AEC7C96
P 5550 1550
F 0 "#PWR012" H 5550 1400 50  0001 C CNN
F 1 "+5V" H 5550 1690 50  0000 C CNN
F 2 "" H 5550 1550 50  0000 C CNN
F 3 "" H 5550 1550 50  0000 C CNN
	1    5550 1550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR013
U 1 1 5AEC7CC6
P 5350 1350
F 0 "#PWR013" H 5350 1100 50  0001 C CNN
F 1 "GND" H 5350 1200 50  0000 C CNN
F 2 "" H 5350 1350 50  0000 C CNN
F 3 "" H 5350 1350 50  0000 C CNN
	1    5350 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	5550 1550 5800 1550
Wire Wire Line
	5800 1550 5800 1450
Wire Wire Line
	5800 1450 6150 1450
Wire Wire Line
	5350 1350 6150 1350
Text Label 700  5600 0    60   ~ 0
MEGA_TX_1
Text Label 700  5700 0    60   ~ 0
MEGA_RX_1
Wire Wire Line
	1300 5700 700  5700
Wire Wire Line
	1300 5600 700  5600
Text Label 5350 1150 0    60   ~ 0
MEGA_RX_1
Text Label 5350 1250 0    60   ~ 0
MEGA_TX_1
Wire Wire Line
	6150 1250 5350 1250
Wire Wire Line
	6150 1150 5350 1150
NoConn ~ 6150 1550
NoConn ~ 6150 1050
NoConn ~ 6150 950 
NoConn ~ 6150 850 
NoConn ~ 6150 750 
$Comp
L +5V #PWR014
U 1 1 5AEC85EF
P 5450 2600
F 0 "#PWR014" H 5450 2450 50  0001 C CNN
F 1 "+5V" H 5450 2740 50  0000 C CNN
F 2 "" H 5450 2600 50  0000 C CNN
F 3 "" H 5450 2600 50  0000 C CNN
	1    5450 2600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 5AEC861F
P 5650 2400
F 0 "#PWR015" H 5650 2150 50  0001 C CNN
F 1 "GND" H 5650 2250 50  0000 C CNN
F 2 "" H 5650 2400 50  0000 C CNN
F 3 "" H 5650 2400 50  0000 C CNN
	1    5650 2400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5650 2400 6150 2400
Wire Wire Line
	5450 2600 6150 2600
NoConn ~ 6150 2500
Text Label 4050 1700 0    60   ~ 0
SCK
Text Label 5650 2300 0    60   ~ 0
SCK
Wire Wire Line
	5650 2300 6150 2300
Text Label 4050 1900 0    60   ~ 0
MISO
Text Label 5650 2200 0    60   ~ 0
MISO
Wire Wire Line
	5650 2200 6150 2200
Text Label 4050 1800 0    60   ~ 0
MOSI
Text Label 5650 2100 0    60   ~ 0
MOSI
Wire Wire Line
	5650 2100 6150 2100
Wire Wire Line
	4050 1700 3900 1700
Wire Wire Line
	4050 1800 3900 1800
Wire Wire Line
	4050 1900 3900 1900
Text Label 5650 2000 0    60   ~ 0
SS
Wire Wire Line
	5650 2000 6150 2000
NoConn ~ 6150 1900
Text Label 4050 1600 0    60   ~ 0
SS
Wire Wire Line
	4050 1600 3900 1600
NoConn ~ 2350 6350
NoConn ~ 2450 6350
NoConn ~ 2550 6350
NoConn ~ 2650 6350
NoConn ~ 2750 6350
NoConn ~ 2850 6350
$Comp
L +5V #PWR016
U 1 1 5AECA362
P 5050 3400
F 0 "#PWR016" H 5050 3250 50  0001 C CNN
F 1 "+5V" H 5050 3540 50  0000 C CNN
F 2 "" H 5050 3400 50  0000 C CNN
F 3 "" H 5050 3400 50  0000 C CNN
	1    5050 3400
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 5AECA392
P 5250 3200
F 0 "#PWR017" H 5250 2950 50  0001 C CNN
F 1 "GND" H 5250 3050 50  0000 C CNN
F 2 "" H 5250 3200 50  0000 C CNN
F 3 "" H 5250 3200 50  0000 C CNN
	1    5250 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 3400 6150 3400
Wire Wire Line
	6150 3400 6150 3350
Wire Wire Line
	5250 3200 5350 3200
Wire Wire Line
	5350 3200 5350 3250
Wire Wire Line
	5350 3250 6150 3250
Text Label 5500 3150 0    60   ~ 0
IRD_ON_OFF
Wire Wire Line
	5500 3150 6150 3150
Text Label 5500 3050 0    60   ~ 0
IRD_TXD
Text Label 5500 2950 0    60   ~ 0
IRD_RXC
Wire Wire Line
	5500 2950 6150 2950
Wire Wire Line
	5500 3050 6150 3050
Text Label 4050 2000 0    60   ~ 0
IRD_ON_OFF
Wire Wire Line
	4050 2000 3900 2000
Text Label 700  5400 0    60   ~ 0
IRD_TXD
Wire Wire Line
	700  5400 1300 5400
Text Label 700  5500 0    60   ~ 0
IRD_RXC
Wire Wire Line
	700  5500 1300 5500
$Comp
L +5V #PWR018
U 1 1 5AECAF91
P 5050 4150
F 0 "#PWR018" H 5050 4000 50  0001 C CNN
F 1 "+5V" H 5050 4290 50  0000 C CNN
F 2 "" H 5050 4150 50  0000 C CNN
F 3 "" H 5050 4150 50  0000 C CNN
	1    5050 4150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 5AECAFCF
P 5250 3950
F 0 "#PWR019" H 5250 3700 50  0001 C CNN
F 1 "GND" H 5250 3800 50  0000 C CNN
F 2 "" H 5250 3950 50  0000 C CNN
F 3 "" H 5250 3950 50  0000 C CNN
	1    5250 3950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 4150 6150 4150
Wire Wire Line
	6150 4150 6150 4100
Wire Wire Line
	6150 4000 5400 4000
Wire Wire Line
	5400 4000 5400 3950
Wire Wire Line
	5400 3950 5250 3950
Text Label 5500 3900 0    60   ~ 0
VN_WHITE
Text Label 5500 3800 0    60   ~ 0
VN_YELLOW
Wire Wire Line
	5500 3800 6150 3800
Wire Wire Line
	5500 3900 6150 3900
Text Notes 8450 5450 0    60   ~ 0
TODO: MAX232 and VN100\nTODO: pin headers SPI\nTODO: pin headers I2C\nTODO: a few AnalogPins\nTODO: a few DigitalPins
$Comp
L CONN_01X08 P8
U 1 1 5AF2DFDD
P 10450 1250
F 0 "P8" H 10450 1700 50  0000 C CNN
F 1 "CONN_01X08" V 10550 1250 50  0000 C CNN
F 2 "" H 10450 1250 50  0001 C CNN
F 3 "" H 10450 1250 50  0000 C CNN
	1    10450 1250
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P9
U 1 1 5AF2E1A0
P 10450 2350
F 0 "P9" H 10450 2800 50  0000 C CNN
F 1 "CONN_01X08" V 10550 2350 50  0000 C CNN
F 2 "" H 10450 2350 50  0001 C CNN
F 3 "" H 10450 2350 50  0000 C CNN
	1    10450 2350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P10
U 1 1 5AF2E215
P 10450 3200
F 0 "P10" H 10450 3400 50  0000 C CNN
F 1 "CONN_01X03" V 10550 3200 50  0000 C CNN
F 2 "" H 10450 3200 50  0001 C CNN
F 3 "" H 10450 3200 50  0000 C CNN
	1    10450 3200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P11
U 1 1 5AF2E272
P 10450 3850
F 0 "P11" H 10450 4000 50  0000 C CNN
F 1 "CONN_01X02" V 10550 3850 50  0000 C CNN
F 2 "" H 10450 3850 50  0001 C CNN
F 3 "" H 10450 3850 50  0000 C CNN
	1    10450 3850
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P13
U 1 1 5AF2E3A3
P 10450 5100
F 0 "P13" H 10450 5250 50  0000 C CNN
F 1 "CONN_01X02" V 10550 5100 50  0000 C CNN
F 2 "" H 10450 5100 50  0001 C CNN
F 3 "" H 10450 5100 50  0000 C CNN
	1    10450 5100
	1    0    0    -1  
$EndComp
Text Notes 10650 2000 0    60   ~ 0
digital pins
Text Notes 10650 850  0    60   ~ 0
ADC
Text Notes 10700 3050 0    60   ~ 0
SPI
Text Notes 10700 3750 0    60   ~ 0
I2C
Text Notes 10700 5000 0    60   ~ 0
power
$Comp
L +5V #PWR020
U 1 1 5AF30318
P 8100 4750
F 0 "#PWR020" H 8100 4600 50  0001 C CNN
F 1 "+5V" H 8100 4890 50  0000 C CNN
F 2 "" H 8100 4750 50  0000 C CNN
F 3 "" H 8100 4750 50  0000 C CNN
	1    8100 4750
	1    0    0    -1  
$EndComp
Wire Wire Line
	8100 4750 7050 4750
$Comp
L GND #PWR021
U 1 1 5AF303C7
P 8100 5350
F 0 "#PWR021" H 8100 5100 50  0001 C CNN
F 1 "GND" H 8100 5200 50  0000 C CNN
F 2 "" H 8100 5350 50  0000 C CNN
F 3 "" H 8100 5350 50  0000 C CNN
	1    8100 5350
	1    0    0    -1  
$EndComp
Wire Wire Line
	7050 5350 8100 5350
Wire Wire Line
	4850 5850 5850 5850
Wire Wire Line
	4950 6050 5850 6050
Wire Wire Line
	7050 6050 8300 6050
Wire Wire Line
	7050 5850 8400 5850
Text Label 700  5300 0    60   ~ 0
RX3
Wire Wire Line
	700  5300 1300 5300
Text Label 700  5200 0    60   ~ 0
TX3
Wire Wire Line
	700  5200 1300 5200
Text Label 5350 5950 0    60   ~ 0
RX3
Wire Wire Line
	5350 5950 5850 5950
Text Label 5350 6150 0    60   ~ 0
TX3
Wire Wire Line
	5350 6150 5850 6150
Text Label 7350 5950 0    60   ~ 0
VN_YELLOW
Text Label 7350 6150 0    60   ~ 0
VN_WHITE
Wire Wire Line
	7350 6150 7050 6150
Wire Wire Line
	7350 5950 7050 5950
$Comp
L C C3
U 1 1 5AF30E52
P 5450 4950
F 0 "C3" H 5475 5050 50  0000 L CNN
F 1 "1.0uF" H 5475 4850 50  0000 L CNN
F 2 "" H 5488 4800 50  0001 C CNN
F 3 "" H 5450 4950 50  0000 C CNN
	1    5450 4950
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 4800 5450 4750
Wire Wire Line
	5450 4750 5850 4750
Wire Wire Line
	5450 5100 5450 5150
Wire Wire Line
	5450 5150 5850 5150
$Comp
L C C4
U 1 1 5AF30F87
P 5450 5450
F 0 "C4" H 5475 5550 50  0000 L CNN
F 1 "1.0uF" H 5475 5350 50  0000 L CNN
F 2 "" H 5488 5300 50  0001 C CNN
F 3 "" H 5450 5450 50  0000 C CNN
	1    5450 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	5450 5300 5450 5250
Wire Wire Line
	5450 5250 5850 5250
Wire Wire Line
	5450 5600 5450 5650
Wire Wire Line
	5450 5650 5850 5650
$Comp
L C C5
U 1 1 5AF31143
P 7400 5150
F 0 "C5" H 7425 5250 50  0000 L CNN
F 1 "1.0uF" V 7250 5050 50  0000 L CNN
F 2 "" H 7438 5000 50  0001 C CNN
F 3 "" H 7400 5150 50  0000 C CNN
	1    7400 5150
	0    1    1    0   
$EndComp
Wire Wire Line
	7250 5150 7050 5150
Wire Wire Line
	7550 5150 7800 5150
Wire Wire Line
	7800 5150 7800 5650
Connection ~ 7800 5350
$Comp
L C C6
U 1 1 5AF31415
P 7400 5650
F 0 "C6" H 7425 5750 50  0000 L CNN
F 1 "1.0uF" V 7250 5550 50  0000 L CNN
F 2 "" H 7438 5500 50  0001 C CNN
F 3 "" H 7400 5650 50  0000 C CNN
	1    7400 5650
	0    1    1    0   
$EndComp
Wire Wire Line
	7050 5650 7250 5650
Wire Wire Line
	7800 5650 7550 5650
NoConn ~ 1300 3100
NoConn ~ 1300 3200
NoConn ~ 1300 3300
NoConn ~ 1300 3400
NoConn ~ 1300 3500
NoConn ~ 1300 3600
$Comp
L +5V #PWR022
U 1 1 5AF35465
P 10100 5050
F 0 "#PWR022" H 10100 4900 50  0001 C CNN
F 1 "+5V" H 10100 5190 50  0000 C CNN
F 2 "" H 10100 5050 50  0000 C CNN
F 3 "" H 10100 5050 50  0000 C CNN
	1    10100 5050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 5050 10250 5050
$Comp
L GND #PWR023
U 1 1 5AF35551
P 10100 5150
F 0 "#PWR023" H 10100 4900 50  0001 C CNN
F 1 "GND" H 10100 5000 50  0000 C CNN
F 2 "" H 10100 5150 50  0000 C CNN
F 3 "" H 10100 5150 50  0000 C CNN
	1    10100 5150
	1    0    0    -1  
$EndComp
Wire Wire Line
	10100 5150 10250 5150
Text Label 700  4800 0    60   ~ 0
SCL
Text Label 700  4900 0    60   ~ 0
SDA
Wire Wire Line
	700  4900 1300 4900
Wire Wire Line
	1300 4800 700  4800
NoConn ~ 1300 5000
NoConn ~ 1300 5100
Text Label 700  5800 0    60   ~ 0
TX0
Text Label 700  5900 0    60   ~ 0
RX0
Wire Wire Line
	700  5900 1300 5900
Wire Wire Line
	1300 5800 700  5800
$Comp
L CONN_01X02 P12
U 1 1 5AF36193
P 10450 4450
F 0 "P12" H 10450 4600 50  0000 C CNN
F 1 "CONN_01X02" V 10550 4450 50  0000 C CNN
F 2 "" H 10450 4450 50  0001 C CNN
F 3 "" H 10450 4450 50  0000 C CNN
	1    10450 4450
	1    0    0    -1  
$EndComp
Text Notes 10700 4400 0    60   ~ 0
Serial 0\n(for RPi)
Text Label 9800 4400 0    60   ~ 0
TX0
Text Label 9800 4500 0    60   ~ 0
RX0
Wire Wire Line
	9800 4500 10250 4500
Wire Wire Line
	10250 4400 9800 4400
Text Label 9800 3800 0    60   ~ 0
SCL
Text Label 9800 3900 0    60   ~ 0
SDA
Wire Wire Line
	9800 3800 10250 3800
Wire Wire Line
	9800 3900 10250 3900
Text Label 9800 3100 0    60   ~ 0
SCK
Text Label 9800 3200 0    60   ~ 0
MOSI
Text Label 9800 3300 0    60   ~ 0
MISO
Wire Wire Line
	9800 3300 10250 3300
Wire Wire Line
	10250 3200 9800 3200
Wire Wire Line
	9800 3100 10250 3100
NoConn ~ 3900 2200
NoConn ~ 3900 2400
NoConn ~ 3900 2500
NoConn ~ 3900 2600
NoConn ~ 3900 2700
NoConn ~ 3900 2800
NoConn ~ 3900 2900
NoConn ~ 3900 3000
NoConn ~ 3900 3100
NoConn ~ 3900 3200
NoConn ~ 3900 3300
NoConn ~ 3900 3400
NoConn ~ 3900 3500
NoConn ~ 3900 3600
NoConn ~ 3900 3700
NoConn ~ 3900 3800
NoConn ~ 3900 3900
NoConn ~ 3900 4000
NoConn ~ 3900 4100
NoConn ~ 3900 4200
NoConn ~ 3900 4300
NoConn ~ 3900 4400
NoConn ~ 3900 4500
NoConn ~ 3900 4600
NoConn ~ 3900 4700
NoConn ~ 3900 4800
NoConn ~ 3900 4900
NoConn ~ 3900 5000
NoConn ~ 3900 5100
Text Label 10250 900  2    60   ~ 0
ANALOG_1
Text Label 10250 1000 2    60   ~ 0
ANALOG_2
Text Label 10250 1100 2    60   ~ 0
ANALOG_3
Text Label 10250 1200 2    60   ~ 0
ANALOG_4
Text Label 10250 1300 2    60   ~ 0
ANALOG_5
Text Label 10250 1400 2    60   ~ 0
ANALOG_6
Text Label 10250 1500 2    60   ~ 0
ANALOG_7
Text Label 10250 1600 2    60   ~ 0
ANALOG_8
Text Label 1300 3800 2    60   ~ 0
ANALOG_1
Text Label 1300 3900 2    60   ~ 0
ANALOG_2
Text Label 1300 4000 2    60   ~ 0
ANALOG_3
Text Label 1300 4100 2    60   ~ 0
ANALOG_4
Text Label 1300 4200 2    60   ~ 0
ANALOG_5
Text Label 1300 4300 2    60   ~ 0
ANALOG_6
Text Label 1300 4400 2    60   ~ 0
ANALOG_7
Text Label 1300 4500 2    60   ~ 0
ANALOG_8
NoConn ~ 1300 3700
Text Label 3900 5200 0    60   ~ 0
DGTL_1
Text Label 3900 5300 0    60   ~ 0
DGTL_2
Text Label 3900 5400 0    60   ~ 0
DGTL_3
Text Label 3900 5500 0    60   ~ 0
DGTL_4
Text Label 3900 5600 0    60   ~ 0
DGTL_5
Text Label 3900 5700 0    60   ~ 0
DGTL_6
Text Label 3900 5800 0    60   ~ 0
DGTL_7
Text Label 3900 5900 0    60   ~ 0
DGTL_8
Text Label 10250 2000 2    60   ~ 0
DGTL_1
Text Label 10250 2100 2    60   ~ 0
DGTL_2
Text Label 10250 2200 2    60   ~ 0
DGTL_3
Text Label 10250 2300 2    60   ~ 0
DGTL_4
Text Label 10250 2400 2    60   ~ 0
DGTL_5
Text Label 10250 2500 2    60   ~ 0
DGTL_6
Text Label 10250 2600 2    60   ~ 0
DGTL_7
Text Label 10250 2700 2    60   ~ 0
DGTL_8
$Comp
L CONN_01X02 P14
U 1 1 5AF3A744
P 4900 6350
F 0 "P14" H 4900 6500 50  0000 C CNN
F 1 "CONN_01X02" V 5000 6350 50  0000 C CNN
F 2 "" H 4900 6350 50  0001 C CNN
F 3 "" H 4900 6350 50  0000 C CNN
	1    4900 6350
	0    1    1    0   
$EndComp
Wire Wire Line
	4950 6150 4950 6050
Wire Wire Line
	4850 6150 4850 5850
Text Notes 4600 6600 0    60   ~ 0
5V_logic_spare
$Comp
L CONN_01X02 P15
U 1 1 5AF3AA28
P 8350 6250
F 0 "P15" H 8350 6400 50  0000 C CNN
F 1 "CONN_01X02" V 8450 6250 50  0000 C CNN
F 2 "" H 8350 6250 50  0001 C CNN
F 3 "" H 8350 6250 50  0000 C CNN
	1    8350 6250
	0    1    1    0   
$EndComp
Wire Wire Line
	8400 5850 8400 6050
Text Notes 8050 6500 0    60   ~ 0
3V_logic_spare
$EndSCHEMATC
