# build the PCB 'as usual'. Fixes:

- 3.9k -> 39k
- 5.6k -> 56k
- 1.0 uF -> 0.1 uF
- 32k, 46k, 50k -> 47k

# programming procedure

NOTE: all the following is tested on Ubuntu.

## code for the Arduino barebone

- power controller: using commit **b76f6a5** (current master IS BROKEN), upload the code in the **PowerControl** folder on the barebone Arduino (for example, using an Arduino Uno to program the chip).

- go back to master in the code

## code for the Arduino Mega logger

- take away the Arduino from logger shield

- Initialize the EEPROM (run **InitializeEEPROM** on the Mega).

- upload the code (from **ArduinoMega**, current master, for example using commit **39c30775849a**). Make sure that you well use the extended buffer compilation flags (see the platformio.ini parameters folder). The sizes of the data and program should read something in this kind:

```
DATA:    [========  ]  81.5% (used 6673 bytes from 8192 bytes)
PROGRAM: [=         ]  10.9% (used 27732 bytes from 253952 bytes)
```

I use the platformio plugin on Visual Studio Code to compile / upload.

## VN100 setup

The VN100 needs to be setup similarly to the waves in ice logger in binary configuration:

https://github.com/jerabaul29/LoggerWavesInIce/tree/master/Logger_GPS_SD_VN_Binary_output

```
---------------------------------------------------------------
The configuration of the VN100 is:

Baud rate      : 57600
Async          : NoOutput
Binary output 1: 
    Async mode    : Serial Port 1
    Rate dividor  : 80
    Common group  : None
    IMU group     : UncompensatedMag
                    UncompensatedAccel
                    UncompensatedGyro
                    Temp
                    Press
    Attitude group: Yaw Pitch Roll
                    DCM
                    MagNed
                    AccelNed
    IMU filtering : 80
 
 This corresponds to the binary header (in HEX):
 fa 14 3e 00 3a 00
 
 The length of the message is (in byte): 123
 
 Note that the post can have in addition an
 end of line marker (in HEX):
 0a 0d
 ---------------------------------------------------------------
 
 ---------------------------------------------------------------
 To examine the content of the SD card on Linux computer,
 use for example xxd:
 
 tail -x FileName | head -y | xxd
 ---------------------------------------------------------------
 
 ---------------------------------------------------------------
 CAUTION: when using VectorNav software, do not forget to:
 - connect to the sensor
 - write to non volatile memory (right click on the sensor in the tree view)
 - disconnect from the sensor
 ---------------------------------------------------------------
 ```

# Raspberry Pi

This is to setup the RPi SD card.

Use the Raspberry Pi image (for example, tested and verified with **img_0208_Rpi_2Bv11.img**). Then:

- Format SD card (exFAT)

- Find it the name of the SD card: ```sudo fdisk -l```

- Unmount it (for example, if /dev/mmcblk0 is the SD card): ```sudo umount /dev/mmcblk0```

- Copy the right image using dd. Be careful, this is a 'dangerous' command.

1. note: this works:
    ```sudo dd if=/media/jrlab/SAMSUNG/Images_RPi/img_0208_Rpi_2Bv11.img dd of=/dev/mmcblk0 bs=4M```

2. note: seems that this works
    ```sudo dd if=/media/jrlab/SAMSUNG/Images_RPi/img_0208_Rpi_2Bv11.img | pv | sudo dd of=/dev/mmcblk0 bs=4M```

3. note: seems to work and be relatively fast
    ```sudo dd if=/media/jrlab/SAMSUNG/Images_RPi/img_0208_Rpi_2Bv11.img bs=16M | pv | sudo dd of=/dev/mmcblk0 bs=16M```

# Buying Iridium credits

- Go to https://rockblock.rock7.com

- Buy the amount of credits and line rental you want.
