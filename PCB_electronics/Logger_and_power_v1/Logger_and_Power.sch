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
Sheet 1 3
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 1400 1050 2650 4150
U 5B0974B7
F0 "Power_part" 60
F1 "Power_part.sch" 60
F2 "Feedback_Mega" O R 4050 1850 60 
F3 "Mega_power_in" O R 4050 2050 60 
F4 "Mega_ground" O R 4050 2250 60 
F5 "Batt_plus" O R 4050 3100 60 
$EndSheet
$Sheet
S 5550 1050 2350 4150
U 5B0974DF
F0 "Logger_part" 60
F1 "Logger_part.sch" 60
F2 "Power_mega_plus" I L 5550 2050 60 
F3 "Power_mega_gnd" I L 5550 2250 60 
F4 "Mega_feedback" I L 5550 1900 60 
F5 "Mega_measure_battery" I L 5550 3100 60 
$EndSheet
Wire Wire Line
	4050 1850 5100 1850
Wire Wire Line
	5100 1850 5100 1900
Wire Wire Line
	5100 1900 5550 1900
Wire Wire Line
	4050 2050 5550 2050
Wire Wire Line
	4050 2250 5550 2250
Wire Wire Line
	4050 3100 5550 3100
$EndSCHEMATC
