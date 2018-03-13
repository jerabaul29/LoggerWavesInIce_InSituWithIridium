# RPi_side

This folder contains the code to be put on the Raspberry Pi of the logger.

## Present folder structure and files

The following folders and files should be present on the RPi in **/home/pi/Logger/**:

.
├── Code
│   ├── launch_processing_RPI.py
│   ├── RPi_Control.py
│   ├── script_RPi.sh
├── Commands
├── Data
├── filename_list.txt
├── Logs
├── README.md
└── ResultAnalyzis

The following should be added to the sudo crontab (sudo crontab -e):

```
@reboot bash /home/pi/Logger/Code/script_RPi.sh
```

## Setup of the RPi

- Get a copy of Raspbian lite: https://www.raspberrypi.org/downloads/raspbian/
- Use *startup disk creator* to create the SD card with Raspbian
- Make the system able to boot on ssh: add an empty 'ssh' file in the boot partition of the SD card
- Disable internet on main computer
- Connect RPi and computer with ethernet
- Give current to RPi to boot RPi
- Find the IP of the host computer: *hostname -I* Example output: 10.42.0.1
- Find the IP of the RPi: *nmap -sP IP__.\** Example: *nmap -sP 10.42.0.\**
- Connect to the RPi: ssh pi@IP_RPi
- There may be a warning because of SSH fingerprint; follow instructions to ignore
- Default RPi password: raspberry
- Change password on the RPi using the *passwd* command, for example to: user: pi password: RPi_WavesIce+
- Enable internet on main computer to share connection to RPi
- Update RPI: *sudo apt-get update* and *sudo apt-get upgrade*
- Install some base packages: *sudo apt-get install vim* and *sudo apt-get install python-pip* 
- Install pip, and necessary packages: *sudo pip install pyserial*, *sudo pip install printind*, *sudo pip install crcmod* **NOTE**: use *apt-get* instead to not need to compile the heavy packages. example: *sudo apt-get install python-numpy* and *sudo apt-get install python-matplotlib* and *sudo apt-get install python-scipy* .
- Copy the logger files to the RPi and setup the tree structure: go to *example_structure_on_Pi* and *scp -r * pi@IP_Pi:/home/pi/*
- Copy the code to the Code folder: *scp *.py pi@IP_Pi:/home/pi/Logger/Code/* and *scp *.sh pi@IP_Pi:/home/pi/Logger/Code/*
- Once the system is working and communicating with the logger, reduce boot time following https://folk.uio.no/jeanra/Informatics/GettingStartedRaspberryPi.html
- change the parameters in the logger code: in **launch_processing_debugging.py** use *main_path=/home/pi/Logger* , if necessary make the *debug=True* or *False*.
- Run tests to check that everything ok.

## Reminders

Some reminders from https://folk.uio.no/jeanra/Informatics/GettingStartedRaspberryPi.html

### Access to RPi

To be able to talk to the RPi by ssh make sure that:

- Network is available (if necessary crontab) and ssh is openend
- The Pi does not shutoff immediately; for this, may be necessary to plug a dummy
Arduino
- In order to upload code to the RPi, from this folder do: ```scp * pi@10.42.0.214:/home/pi/Logger/Code/.```
- If necessary, see the webpage on my website: https://folk.uio.no/jeanra/Informatics/GettingStartedRaspberryPi.html
