# build the PCB 'as usual'. Fixes:

- 3.9k -> 39k
- 5.6k -> 56k
- 1.0 uF -> 0.1 uF
- 32k, 46k, 50k -> 47k

# programming procedure

NOTE: all the following is tested on Ubuntu.

## code for the Arduino barebone

- power controller: using commit **b76f6a5** (current master IS BROKEN), upload the code in the **PowerControl** folder on the barebone Arduino (for example, using an Arduino Uno to program the chip). Check the **parameters** and that you are ready for deployment, not test. This can be done either by using ```git checkout``` or by using the folder in **DevelopmentCode/PowerController_commitb76f6a5**.

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

Can either use the code in the DevelopmentCode / RPi_Side and the tips on my website post: https://folk.uio.no/jeanra/Informatics/GettingStartedRaspberryPi.html .

or

Use the Raspberry Pi image (for example, tested and verified with **img_0208_Rpi_2Bv11.img**). Then:

TODO: find the image from the HDD and upload.

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

- If using a larger SD card than the image size (16GB), for example a 32GB card, resize the root partition to full size:

### method 1 for resizing

1. look with gparted at the right device (for example, /dev/mmcblk0).

2. if necessary, unmount partitions.

3. use resize to grow the root partition (for example, /dev/mmcblk0p2)

### method 2 for resizing

From https://raspberrypi.stackexchange.com/questions/499/how-can-i-resize-my-root-partition .

From the command line or a terminal window enter the following

sudo fdisk /dev/mmcblk0

then type p to list the partition table

you should see three partitions. if you look in the last column labeled System you should have

    W95 FAT32
    Linux
    Linux Swap

make a note of the start number for partiton 2, you will need this later. though it will likely still be on the screen (just in case).

next type d to delete a partition.

You will then be prompted for the number of the partition you want to delete. In the case above you want to delete both the Linux and Linux swap partitions.

So type 2

then type d again and then type 3 to delete the swap partition.

Now you can resize the main partition.

type n to create a new partition.

This new partition needs to be a primary partition so type p.

Next enter 2 when prompted for a partition number.

You will now be prompted for the first sector for the new partition. Enter the start number from the earlier step (the Linux partition)

Next you will be prompted for the last sector you can just hit enter to accept the default which will utilize the remaining disk space.

Type w to save the changes you have made.

Next reboot the system with the following command:

sudo reboot

once the system has reboot and you are back at the commandline enter the following command:

sudo resize2fs /dev/mmcblk0p2

Note: this can take a long time (depending on the card size and speed) be patient and let it finish so you do not mess up the file system and have to start from scratch.

Once it is done reboot the system with the following command:

sudo reboot

You can now verify that the system is using the full capacity of the SD Card by entering the following command:

df -h

### For example, working fine with:

```
(VE_P2) jrlab@jrlab-T150s:~$ sudo fdisk /dev/mmcblk0
[sudo] password for jrlab: 

Welcome to fdisk (util-linux 2.27.1).
Changes will remain in memory only, until you decide to write them.
Be careful before using the write command.


Command (m for help): p
Disk /dev/mmcblk0: 29,7 GiB, 31914983424 bytes, 62333952 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x823a85f9

Device         Boot Start      End  Sectors  Size Id Type
/dev/mmcblk0p1       8192    93236    85045 41,5M  c W95 FAT32 (LBA)
/dev/mmcblk0p2      94208 31116287 31022080 14,8G 83 Linux

Command (m for help): 


Command (m for help): d
Partition number (1,2, default 2): 2

Partition 2 has been deleted.

Command (m for help): n
Partition type
   p   primary (1 primary, 0 extended, 3 free)
   e   extended (container for logical partitions)
Select (default p): p
Partition number (2-4, default 2): 2
First sector (2048-62333951, default 2048): 94208
Last sector, +sectors or +size{K,M,G,T,P} (94208-62333951, default 62333951): 

Created a new partition 2 of type 'Linux' and of size 29,7 GiB.

Command (m for help): w
The partition table has been altered.
Calling ioctl() to re-read partition table.
Syncing disks.

(VE_P2) jrlab@jrlab-T150s:~$ sudo e2fsck -f /dev/mmcblk0p2
e2fsck 1.42.13 (17-May-2015)
Pass 1: Checking inodes, blocks, and sizes
Pass 2: Checking directory structure
Pass 3: Checking directory connectivity
Pass 4: Checking reference counts
Pass 5: Checking group summary information
rootfs: 55411/940576 files (0.2% non-contiguous), 447272/3877760 blocks
(VE_P2) jrlab@jrlab-T150s:~$ sudo resize2fs /dev/mmcblk0p2
resize2fs 1.42.13 (17-May-2015)
Resizing the filesystem on /dev/mmcblk0p2 to 7779968 (4k) blocks.
The filesystem on /dev/mmcblk0p2 is now 7779968 (4k) blocks long.


```

# Buying Iridium credits

- Go to https://rockblock.rock7.com

- Buy the amount of credits and line rental you want.
