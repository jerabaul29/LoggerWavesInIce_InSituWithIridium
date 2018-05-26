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
LIBS:power_controller-cache
LIBS:Logger-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 3
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
U 1 1 5B09A20E
P 2600 3750
F 0 "XA1" V 2700 3750 60  0000 C CNN
F 1 "Arduino_Mega2560_Shield" V 2500 3750 60  0000 C CNN
F 2 "Arduino:Arduino_Mega2560_Shield" H 3300 6500 60  0001 C CNN
F 3 "" H 3300 6500 60  0001 C CNN
	1    2600 3750
	1    0    0    1   
$EndComp
$Comp
L CONN_01X09 P5
U 1 1 5B09A20F
P 6350 1150
F 0 "P5" H 6350 1650 50  0000 C CNN
F 1 "GPS_breakout_connector" V 6450 1150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x09" H 6350 1150 50  0001 C CNN
F 3 "" H 6350 1150 50  0000 C CNN
	1    6350 1150
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P6
U 1 1 5B09A210
P 6350 2250
F 0 "P6" H 6350 2700 50  0000 C CNN
F 1 "SD_breakout_connector" V 6450 2250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x08" H 6350 2250 50  0001 C CNN
F 3 "" H 6350 2250 50  0000 C CNN
	1    6350 2250
	1    0    0    -1  
$EndComp
$Comp
L MAX232 U3
U 1 1 5B09A211
P 6450 5450
F 0 "U3" H 6000 6300 50  0000 L CNN
F 1 "MAX232" H 6650 6300 50  0000 L CNN
F 2 "Housings_DIP:DIP-16_W7.62mm_LongPads" H 6450 5450 50  0001 C CNN
F 3 "" H 6450 5450 50  0000 C CNN
	1    6450 5450
	1    0    0    -1  
$EndComp
$Comp
L FQP27P06 U4
U 1 1 5B09A212
P 8050 3050
F 0 "U4" H 8250 2950 60  0000 C CNN
F 1 "FQP27P06" H 8400 3150 60  0000 C CNN
F 2 "TO_SOT_Packages_THT:TO-220_Neutral123_Vertical" H 8050 3050 60  0001 C CNN
F 3 "" H 8050 3050 60  0000 C CNN
	1    8050 3050
	1    0    0    -1  
$EndComp
$Comp
L CP C3
U 1 1 5B09A213
P 800 2950
F 0 "C3" H 825 3050 50  0000 L CNN
F 1 "1uF" H 825 2850 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D8_L11.5_P3.5" H 838 2800 50  0001 C CNN
F 3 "" H 800 2950 50  0000 C CNN
	1    800  2950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X04 P8
U 1 1 5B09A214
P 6350 3950
F 0 "P8" H 6350 4200 50  0000 C CNN
F 1 "VN100_connector" V 6450 3950 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 6350 3950 50  0001 C CNN
F 3 "" H 6350 3950 50  0000 C CNN
	1    6350 3950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P9
U 1 1 5B09A215
P 8900 3650
F 0 "P9" H 8900 3800 50  0000 C CNN
F 1 "RPI_connector" V 9000 3650 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x01" H 8900 3650 50  0001 C CNN
F 3 "" H 8900 3650 50  0000 C CNN
	1    8900 3650
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X05 P7
U 1 1 5B09A217
P 6350 3150
F 0 "P7" H 6350 3450 50  0000 C CNN
F 1 "Iridium_connector" V 6450 3150 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 6350 3150 50  0001 C CNN
F 3 "" H 6350 3150 50  0000 C CNN
	1    6350 3150
	1    0    0    -1  
$EndComp
Text Label 7400 1450 0    60   ~ 0
MEGA_feedback
Text Label 7650 1950 0    60   ~ 0
BATT_+
$Comp
L CP C8
U 1 1 5B09A21F
P 8450 3650
F 0 "C8" H 8475 3750 50  0000 L CNN
F 1 "1000uF" H 8475 3550 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Radial_D8_L13_P3.8" H 8488 3500 50  0001 C CNN
F 3 "" H 8450 3650 50  0000 C CNN
	1    8450 3650
	1    0    0    -1  
$EndComp
Text Label 6850 3050 0    60   ~ 0
RPI_control
$Comp
L R R8
U 1 1 5B09A222
P 7700 2650
F 0 "R8" V 7780 2650 50  0000 C CNN
F 1 "46k" V 7700 2650 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 7630 2650 50  0001 C CNN
F 3 "" H 7700 2650 50  0000 C CNN
	1    7700 2650
	0    1    1    0   
$EndComp
$Comp
L R R9
U 1 1 5B09A223
P 8050 3700
F 0 "R9" V 8130 3700 50  0000 C CNN
F 1 "32k" V 8050 3700 50  0000 C CNN
F 2 "Resistors_ThroughHole:Resistor_Horizontal_RM10mm" V 7980 3700 50  0001 C CNN
F 3 "" H 8050 3700 50  0000 C CNN
	1    8050 3700
	1    0    0    -1  
$EndComp
NoConn ~ 1300 1600
NoConn ~ 1300 2700
NoConn ~ 1300 2000
NoConn ~ 1300 3000
Text Label 4050 2300 0    60   ~ 0
RPI_control
Text Label 800  4600 0    60   ~ 0
BATT_+
Text Label 4050 2100 0    60   ~ 0
MEGA_feedback
Text Label 700  5600 0    60   ~ 0
MEGA_TX_1
Text Label 700  5700 0    60   ~ 0
MEGA_RX_1
Text Label 5350 1150 0    60   ~ 0
MEGA_RX_1
Text Label 5350 1250 0    60   ~ 0
MEGA_TX_1
NoConn ~ 6150 1550
NoConn ~ 6150 1050
NoConn ~ 6150 950 
NoConn ~ 6150 850 
NoConn ~ 6150 750 
NoConn ~ 6150 2500
Text Label 4050 1700 0    60   ~ 0
SCK
Text Label 5650 2300 0    60   ~ 0
SCK
Text Label 4050 1900 0    60   ~ 0
MISO
Text Label 5650 2200 0    60   ~ 0
MISO
Text Label 4050 1800 0    60   ~ 0
MOSI
Text Label 5650 2100 0    60   ~ 0
MOSI
Text Label 5650 2000 0    60   ~ 0
SS
NoConn ~ 6150 1900
Text Label 4050 1600 0    60   ~ 0
SS
NoConn ~ 2350 6350
NoConn ~ 2450 6350
NoConn ~ 2550 6350
NoConn ~ 2650 6350
NoConn ~ 2750 6350
NoConn ~ 2850 6350
Text Label 5500 3150 0    60   ~ 0
IRD_ON_OFF
Text Label 5500 3050 0    60   ~ 0
IRD_TXD
Text Label 5500 2950 0    60   ~ 0
IRD_RXC
Text Label 4050 2000 0    60   ~ 0
IRD_ON_OFF
Text Label 700  5400 0    60   ~ 0
IRD_TXD
Text Label 700  5500 0    60   ~ 0
IRD_RXC
Text Label 5500 3900 0    60   ~ 0
VN_WHITE
Text Label 5500 3800 0    60   ~ 0
VN_YELLOW
$Comp
L CONN_01X08 P10
U 1 1 5B09A22F
P 10450 1250
F 0 "P10" H 10450 1700 50  0000 C CNN
F 1 "CONN_01X08" V 10550 1250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x08" H 10450 1250 50  0001 C CNN
F 3 "" H 10450 1250 50  0000 C CNN
	1    10450 1250
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X08 P11
U 1 1 5B09A230
P 10450 2350
F 0 "P11" H 10450 2800 50  0000 C CNN
F 1 "CONN_01X08" V 10550 2350 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x08" H 10450 2350 50  0001 C CNN
F 3 "" H 10450 2350 50  0000 C CNN
	1    10450 2350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X03 P12
U 1 1 5B09A231
P 10450 3200
F 0 "P12" H 10450 3400 50  0000 C CNN
F 1 "CONN_01X03" V 10550 3200 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x03" H 10450 3200 50  0001 C CNN
F 3 "" H 10450 3200 50  0000 C CNN
	1    10450 3200
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P13
U 1 1 5B09A232
P 10450 3850
F 0 "P13" H 10450 4000 50  0000 C CNN
F 1 "CONN_01X02" V 10550 3850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x01" H 10450 3850 50  0001 C CNN
F 3 "" H 10450 3850 50  0000 C CNN
	1    10450 3850
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
Text Label 700  5300 0    60   ~ 0
RX3
Text Label 700  5200 0    60   ~ 0
TX3
Text Label 5350 5950 0    60   ~ 0
TX3
Text Label 5350 6150 0    60   ~ 0
RX3
Text Label 7350 5950 0    60   ~ 0
VN_YELLOW
Text Label 7350 6150 0    60   ~ 0
VN_WHITE
$Comp
L C C4
U 1 1 5B09A236
P 5450 4950
F 0 "C4" H 5475 5050 50  0000 L CNN
F 1 "1.0uF" H 5475 4850 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 5488 4800 50  0001 C CNN
F 3 "" H 5450 4950 50  0000 C CNN
	1    5450 4950
	1    0    0    -1  
$EndComp
$Comp
L C C5
U 1 1 5B09A237
P 5450 5450
F 0 "C5" H 5475 5550 50  0000 L CNN
F 1 "1.0uF" H 5475 5350 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 5488 5300 50  0001 C CNN
F 3 "" H 5450 5450 50  0000 C CNN
	1    5450 5450
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5B09A238
P 7400 5150
F 0 "C6" H 7425 5250 50  0000 L CNN
F 1 "1.0uF" V 7250 5050 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 7438 5000 50  0001 C CNN
F 3 "" H 7400 5150 50  0000 C CNN
	1    7400 5150
	0    1    1    0   
$EndComp
$Comp
L C C7
U 1 1 5B09A239
P 7400 5650
F 0 "C7" H 7425 5750 50  0000 L CNN
F 1 "1.0uF" V 7250 5550 50  0000 L CNN
F 2 "Capacitors_ThroughHole:C_Disc_D3_P2.5" H 7438 5500 50  0001 C CNN
F 3 "" H 7400 5650 50  0000 C CNN
	1    7400 5650
	0    1    1    0   
$EndComp
NoConn ~ 1300 3100
NoConn ~ 1300 3200
NoConn ~ 1300 3300
NoConn ~ 1300 3400
NoConn ~ 1300 3500
NoConn ~ 1300 3600
Text Label 700  4800 0    60   ~ 0
SCL
Text Label 700  4900 0    60   ~ 0
SDA
NoConn ~ 1300 5000
NoConn ~ 1300 5100
Text Label 700  5800 0    60   ~ 0
TX0
Text Label 700  5900 0    60   ~ 0
RX0
$Comp
L CONN_01X02 P14
U 1 1 5B09A23C
P 10450 4450
F 0 "P14" H 10450 4600 50  0000 C CNN
F 1 "CONN_01X02" V 10550 4450 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x01" H 10450 4450 50  0001 C CNN
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
Text Label 9800 3800 0    60   ~ 0
SCL
Text Label 9800 3900 0    60   ~ 0
SDA
Text Label 9800 3100 0    60   ~ 0
SCK
Text Label 9800 3200 0    60   ~ 0
MOSI
Text Label 9800 3300 0    60   ~ 0
MISO
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
Text Notes 10700 5000 0    60   ~ 0
power
$Comp
L CONN_01X02 P15
U 1 1 5B09A233
P 10450 5100
F 0 "P15" H 10450 5250 50  0000 C CNN
F 1 "CONN_01X02" V 10550 5100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x01" H 10450 5100 50  0001 C CNN
F 3 "" H 10450 5100 50  0000 C CNN
	1    10450 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	7400 1450 8100 1450
Wire Wire Line
	8100 1450 8100 1200
Wire Wire Line
	8100 1000 8000 1000
Wire Wire Line
	7650 1950 8100 1950
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
Connection ~ 8700 3900
Connection ~ 8450 3400
Wire Wire Line
	6850 3050 7700 3050
Wire Wire Line
	8050 2550 8050 2700
Wire Wire Line
	7850 2650 8050 2650
Connection ~ 8050 2650
Wire Wire Line
	7550 2650 7500 2650
Wire Wire Line
	7500 2650 7500 3050
Connection ~ 7500 3050
Wire Wire Line
	8050 3400 8050 3550
Wire Wire Line
	8050 3850 8050 3900
Connection ~ 8450 3900
Wire Wire Line
	800  2800 1300 2800
Wire Wire Line
	1150 1700 1300 1700
Wire Wire Line
	1300 1700 1300 1900
Connection ~ 1300 1800
Connection ~ 1300 1700
Connection ~ 1300 1900
Wire Wire Line
	1300 2500 1150 2500
Connection ~ 1300 2400
Connection ~ 1300 2300
Connection ~ 1300 2200
Wire Wire Line
	1300 2100 1300 2500
Connection ~ 1300 2100
Connection ~ 1300 2500
Connection ~ 8050 3400
Wire Wire Line
	4050 2300 3900 2300
Wire Wire Line
	800  4600 1300 4600
Wire Wire Line
	4050 2100 3900 2100
Wire Wire Line
	5350 1450 6150 1450
Wire Wire Line
	5350 1350 6150 1350
Wire Wire Line
	1300 5700 700  5700
Wire Wire Line
	1300 5600 700  5600
Wire Wire Line
	6150 1250 5350 1250
Wire Wire Line
	6150 1150 5350 1150
Wire Wire Line
	5650 2400 6150 2400
Wire Wire Line
	5650 2300 6150 2300
Wire Wire Line
	5650 2200 6150 2200
Wire Wire Line
	5650 2100 6150 2100
Wire Wire Line
	4050 1700 3900 1700
Wire Wire Line
	4050 1800 3900 1800
Wire Wire Line
	4050 1900 3900 1900
Wire Wire Line
	5650 2000 6150 2000
Wire Wire Line
	4050 1600 3900 1600
Wire Wire Line
	5500 3150 6150 3150
Wire Wire Line
	5500 2950 6150 2950
Wire Wire Line
	5500 3050 6150 3050
Wire Wire Line
	4050 2000 3900 2000
Wire Wire Line
	700  5400 1300 5400
Wire Wire Line
	700  5500 1300 5500
Wire Wire Line
	5500 3800 6150 3800
Wire Wire Line
	5500 3900 6150 3900
Wire Wire Line
	8100 4750 7050 4750
Wire Wire Line
	7050 5350 8100 5350
Wire Wire Line
	4850 5850 5850 5850
Wire Wire Line
	4950 6050 5850 6050
Wire Wire Line
	7050 6050 8400 6050
Wire Wire Line
	7050 5850 8400 5850
Wire Wire Line
	700  5300 1300 5300
Wire Wire Line
	700  5200 1300 5200
Wire Wire Line
	5350 5950 5850 5950
Wire Wire Line
	5350 6150 5850 6150
Wire Wire Line
	7350 6150 7050 6150
Wire Wire Line
	7350 5950 7050 5950
Wire Wire Line
	5450 4800 5450 4750
Wire Wire Line
	5450 4750 5850 4750
Wire Wire Line
	5450 5100 5450 5150
Wire Wire Line
	5450 5150 5850 5150
Wire Wire Line
	5450 5300 5450 5250
Wire Wire Line
	5450 5250 5850 5250
Wire Wire Line
	5450 5600 5450 5650
Wire Wire Line
	5450 5650 5850 5650
Wire Wire Line
	7250 5150 7050 5150
Wire Wire Line
	7550 5150 7800 5150
Wire Wire Line
	7800 5150 7800 5650
Connection ~ 7800 5350
Wire Wire Line
	7050 5650 7250 5650
Wire Wire Line
	7800 5650 7550 5650
Wire Wire Line
	10100 5050 10250 5050
Wire Wire Line
	10100 5150 10250 5150
Wire Wire Line
	700  4900 1300 4900
Wire Wire Line
	1300 4800 700  4800
Wire Wire Line
	700  5900 1300 5900
Wire Wire Line
	1300 5800 700  5800
Wire Wire Line
	9800 4500 10250 4500
Wire Wire Line
	10250 4400 9800 4400
Wire Wire Line
	9800 3800 10250 3800
Wire Wire Line
	9800 3900 10250 3900
Wire Wire Line
	9800 3300 10250 3300
Wire Wire Line
	10250 3200 9800 3200
Wire Wire Line
	9800 3100 10250 3100
Wire Wire Line
	4950 6150 4950 6050
Wire Wire Line
	4850 6150 4850 5850
Wire Wire Line
	8400 5850 8400 6200
Wire Wire Line
	4850 6150 4950 6150
Wire Wire Line
	4900 6450 4900 6150
Connection ~ 4900 6150
Connection ~ 8400 6050
Text HLabel 8100 1000 2    60   Input ~ 0
Power_mega_plus
Text HLabel 8100 1100 2    60   Input ~ 0
Power_mega_gnd
Text HLabel 8100 1200 2    60   Input ~ 0
Mega_feedback
Text HLabel 8100 1950 2    60   Input ~ 0
Mega_measure_battery
Text Label 800  3100 3    60   ~ 0
gnd_mega
Text Label 1150 1700 1    60   ~ 0
5V_mega
Text Label 1150 2500 2    60   ~ 0
gnd_mega
Text Label 5350 1450 2    60   ~ 0
5V_mega
Text Label 5350 1350 2    60   ~ 0
gnd_mega
Text Label 5650 2400 2    60   ~ 0
gnd_mega
Text Label 5650 2600 2    60   ~ 0
5V_mega
Wire Wire Line
	5650 2600 6150 2600
Text Label 6150 3250 2    60   ~ 0
gnd_mega
Text Label 6150 3350 2    60   ~ 0
5V_mega
Text Label 6150 4000 2    60   ~ 0
gnd_mega
Text Label 6150 4100 2    60   ~ 0
5V_mega
Text Label 8100 5350 0    60   ~ 0
gnd_mega
Text Label 8400 6200 2    60   ~ 0
5V_mega
Text Label 4900 6450 2    60   ~ 0
5V_mega
Text Label 8100 4750 0    60   ~ 0
5V_mega
Text Label 8000 1000 2    60   ~ 0
5V_mega
Text Label 7950 1100 2    60   ~ 0
gnd_mega
Wire Wire Line
	7950 1100 8100 1100
Text Label 8050 2550 2    60   ~ 0
5V_mega
Text Label 8700 4100 2    60   ~ 0
gnd_mega
Text Label 10100 5150 2    60   ~ 0
gnd_mega
Text Label 10100 5050 2    60   ~ 0
5V_mega
Text Label 1100 750  2    60   ~ 0
gnd_mega
$Comp
L PWR_FLAG #FLG019
U 1 1 5B09F8E8
P 1100 900
F 0 "#FLG019" H 1100 995 50  0001 C CNN
F 1 "PWR_FLAG" H 1100 1080 50  0000 C CNN
F 2 "" H 1100 900 50  0000 C CNN
F 3 "" H 1100 900 50  0000 C CNN
	1    1100 900 
	-1   0    0    1   
$EndComp
Wire Wire Line
	1100 900  1100 750 
$EndSCHEMATC
