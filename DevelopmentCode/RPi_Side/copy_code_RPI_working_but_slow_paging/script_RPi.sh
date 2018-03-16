#!/bin/bash

echo "booted" >> /home/pi/Logger/Code/logs_script.log
python /home/pi/Logger/Code/launch_processing_debugging.py >> /home/pi/Logger/Code/logs_script.log 2>&1
echo "done with Python; shutdown" >> /home/pi/Logger/Code/logs_script.log

sudo shutdown now
