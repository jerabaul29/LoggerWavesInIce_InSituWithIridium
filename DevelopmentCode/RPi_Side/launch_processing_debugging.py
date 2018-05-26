from __future__ import print_function
from RPi_Control import RPi_control

# debug: use dummy data
# debug = True
# no debug: use true data received from IMU
debug = False

# path to use on my laptop
# path = '/home/jrlab/Desktop/Current/Logger/'
# path to use on RPi
path = '/home/pi/Logger/'

print("Python well started")

RPi_instance = RPi_control(verbose=2, main_path=path, debug=debug)
RPi_instance.launch_RPi_command()
