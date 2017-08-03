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

## Access to RPi

To be able to talk to the RPi by ssh make sure that:

- Network is available (if necessary crontab) and ssh is openend
- The Pi does not shutoff immediately; for this, may be necessary to plug a dummy
Arduino
- In order to upload code to the RPi, from this folder do: ```scp * pi@10.42.0.214:/home/pi/Logger/Code/.```
