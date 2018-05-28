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
LIBS:RASPBERRYPI
LIBS:RASPBERRYPI2
LIBS:Logger_and_Power-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 4
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
L RASPBERRYPI2 P18
U 1 1 5B0BE3C0
P 3200 3000
F 0 "P18" H 2600 4150 60  0000 L CNN
F 1 "RASPBERRYPI2" H 2600 1950 60  0000 L CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x20" H 3000 2200 60  0001 C CNN
F 3 "" H 3000 2200 60  0000 C CNN
	1    3200 3000
	1    0    0    -1  
$EndComp
Text Notes 6400 3600 0    60   ~ 0
pins for serial Arduino to RPI:\nneed to use a level shifter.\nConnections (for MAX3323):\n- as usual\n- Ard TX -> 10\n- Ard Rx -> 9 \n- RPi TXD -> 8\n- RPi RXD -> 7\n\nproblem at this stage with DMA issue on RPI: see to fix.\n\nBest would be to use SPI:\nneed  a level converter:\nsee:\nhttps://www.adafruit.com/product/1875\nhttps://www.adafruit.com/product/757
Text HLabel 4350 2600 2    60   Input ~ 0
RPI_power_gnd
Text HLabel 4350 2000 2    60   Input ~ 0
RPI_power_5V
Wire Wire Line
	4350 2600 4000 2600
Wire Wire Line
	4000 2200 4150 2200
Wire Wire Line
	4150 2200 4150 4350
Connection ~ 4150 2600
Wire Wire Line
	4150 2900 4000 2900
Wire Wire Line
	4150 3400 4000 3400
Connection ~ 4150 2900
Wire Wire Line
	4150 3600 4000 3600
Connection ~ 4150 3400
Wire Wire Line
	2400 3900 2200 3900
Wire Wire Line
	2200 2400 2200 4350
Wire Wire Line
	2200 4350 4150 4350
Connection ~ 4150 3600
Wire Wire Line
	2400 3200 2200 3200
Connection ~ 2200 3900
Wire Wire Line
	2400 2400 2200 2400
Connection ~ 2200 3200
Wire Wire Line
	4350 2000 4000 2000
Wire Wire Line
	4000 2100 4150 2100
Wire Wire Line
	4150 2100 4150 2000
Connection ~ 4150 2000
NoConn ~ 4000 2300
NoConn ~ 4000 2400
NoConn ~ 4000 2500
NoConn ~ 4000 2700
NoConn ~ 4000 2800
NoConn ~ 4000 3000
NoConn ~ 4000 3100
NoConn ~ 4000 3200
NoConn ~ 4000 3300
NoConn ~ 4000 3500
NoConn ~ 4000 3700
NoConn ~ 4000 3800
NoConn ~ 4000 3900
NoConn ~ 2400 3800
NoConn ~ 2400 3700
NoConn ~ 2400 3600
NoConn ~ 2400 3500
NoConn ~ 2400 3400
NoConn ~ 2400 3300
NoConn ~ 2400 3100
NoConn ~ 2400 3000
NoConn ~ 2400 2900
NoConn ~ 2400 2800
NoConn ~ 2400 2700
NoConn ~ 2400 2600
NoConn ~ 2400 2500
NoConn ~ 2400 2300
NoConn ~ 2400 2200
NoConn ~ 2400 2100
NoConn ~ 2400 2000
$EndSCHEMATC
