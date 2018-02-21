from __future__ import print_function
import serial
import glob
from printind.printind_function import printi
from parser import Parser_logger
from compute_statistics import WaveStatistics
from time import sleep
import os

"""
TODO: do not tranmit reduced spectrum if no signal
TODO: test processing and sending back through Iridium together with the Mega
TODO: command for sending back whole parts of a file (TRT command)
NOTE: may need to help the Arduino for transmitting through the Iridium by cutting the file in different parts to accomodate
Iridium buffer size.
"""


class RPi_control(object):
    def __init__(self, verbose=0, main_path='/home/pi/Logger/'):
        self.verbose = verbose
        self.main_path = main_path

    def open_serial_to_arduino(self):
        '''Find available serial ports to Arduino
        '''
        available_ports = glob.glob('/dev/ttyACM*')
        self.serial_port = serial.Serial(available_ports[0], baudrate=115200)
        self.serial_port.flushInput()
        self.serial_port.flushOutput()

        if self.verbose > 0:
            printi('Using port: ' + str(available_ports[0]))

    def connect_to_Arduino(self):
        """Connect to the Arduino: acknowledge that listening."""

        wait_for_acknowledged_connection = True

        # clear the incoming buffer except last received char
        while self.serial_port.in_waiting > 1:
            self.serial_port.read()

        # look at last received char
        while wait_for_acknowledged_connection:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 1:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'B':
                    self.serial_port.write('R')

                wait_for_acknowledged_connection = False

        if self.verbose > 0:
            printi("Connection Acknowledged")

    def receive_from_Arduino(self):
        """Receive commands, filename and data from Arduino in this order.

        -- The commands from Arduino should be in format: C [COMMAND] .
        The [COMMAND] sequence should not contain the character 'C' nor 'N'.

        -- The filename should be transmitted as: N CCCCCC . The C should all
        be different from 'N'

        -- The data in the file should be transmitted as: N [DATA] FINISHED
        """

        self.list_of_commands = []
        self.filename = ""
        self.data = ""

        # received commands
        if self.verbose > 0:
            printi("Receive commands")

        wait_for_commands = True
        current_command = ['DEFAULT']

        while wait_for_commands:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 1:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'C':
                    self.list_of_commands.append(''.join(current_command))
                    current_command = []

                elif crrt_char == 'N':
                    self.list_of_commands.append(''.join(current_command))
                    wait_for_commands = False

                else:
                    current_command.append(crrt_char)

        # receive filename
        if self.verbose > 0:
            printi("Receive Filename")

        wait_for_filename = True
        filename_list = []

        while wait_for_filename:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 1:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'N':
                    wait_for_filename = False

                else:
                    filename_list.append(crrt_char)

        self.filename = ''.join(filename_list)

        # receive data
        if self.verbose > 0:
            printi("Receive data")

        wait_for_data = True

        data_list = []
        remaining_finish = 8

        while wait_for_data:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 1:
                    printi('Received char: ' + str(crrt_char))

                data_list.append(crrt_char)

                if (remaining_finish == 8) and (crrt_char == 'F'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 8')

                elif (remaining_finish == 7) and (crrt_char == 'I'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 7')

                elif (remaining_finish == 6) and (crrt_char == 'N'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 6')

                elif (remaining_finish == 5) and (crrt_char == 'I'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 5')

                elif (remaining_finish == 4) and (crrt_char == 'S'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 4')

                elif (remaining_finish == 3) and (crrt_char == 'H'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 3')

                elif (remaining_finish == 2) and (crrt_char == 'E'):
                    remaining_finish -= 1

                    if self.verbose > 1:
                        printi('FINISHED 2')

                elif (remaining_finish == 1) and (crrt_char == 'D'):
                    wait_for_data = False

                    if self.verbose > 1:
                        printi('FINISHED 1')

                else:
                    remaining_finish = 8

        self.data = ''.join(data_list[:-8])

    def save_all(self):
        """Save received commands and data, and append the filename list."""

        if self.verbose > 0:
            printi("Append filename_list.txt")

        with open(self.main_path + 'filename_list.txt', "a") as myfile:
            myfile.write(self.filename)
            myfile.write('\n')

        if self.verbose > 0:
            printi("Add entry in Commands")

        with open(self.main_path + 'Commands/' + self.filename, "w") as myfile:
            for current_command in self.list_of_commands:
                myfile.write(current_command)
                myfile.write('\n')

        if self.verbose > 0:
            printi("Add entry in Data")

        with open(self.main_path + 'Data/' + self.filename, "w") as myfile:
            myfile.write(self.data)

    def confirm_finished(self):
        """Confirm to Arduino Mega that done with RPi and can be shut down."""

        self.serial_port.write('F')

    def processing(self):
        """Launch the processing of data"""

        # NOTE: this is here I should rather use the code from Graig!!

        # do the parsing of the file from the Mega
        path_in = self.main_path + 'Data/'
        path_out = self.main_path + 'ResultAnalyzis/'
        parser_instance = Parser_logger(path_in, path_out, self.filename, self.verbose)
        parser_instance.process_file()

        # do the analysis of the data
        path_in_processing = self.main_path + 'ResultAnalyzis/'
        instance_compute_statistics = WaveStatistics(path_in=path_in_processing, filename=self.filename)
        instance_compute_statistics.perform_all_processing()
        instance_compute_statistics.save_all_results()

    def wait_for_arduino_acknowledgement(self):
        while True:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()
                if crrt_char == 'R':
                    break

    def send_over_Iridium(self):
        """Send the processed data over Iridium"""

        # when should send through Iridium, ask it to the Mega as:
        # I [#numberOfBytes] S

        path_in_processing = self.main_path + 'ResultAnalyzis/'

        # go through commands received by Iridium
        for current_command in self.list_of_commands:

            # default
            if current_command == 'DEFAULT':
                # send back SWH ------------------------------------------------
                self.serial_port.write('I')

                self.serial_port.write(3 + 2)

                self.serial_port.write('S')

                # identifier for the receiving side of the Iridium message
                self.serial_port.write('S')
                self.serial_port.write('W')
                self.serial_port.write('H')

                self.send_content_binary_file(path_in_processing + '_SWH.bdat')

                # wait for transmission by Iridium done
                self.wait_for_arduino_acknowledgement()

                # send back spectral_properties --------------------------------
                self.serial_port.write('I')

                self.serial_port.write(3 + 6)

                self.serial_port.write('S')

                # identifier for the receiving side of the Iridium message
                self.serial_port.write('S')
                self.serial_port.write('P')
                self.serial_port.write('P')

                self.send_content_binary_file(path_in_processing + '_spectral_properties.bdat')

                # wait for transmission by Iridium done
                self.wait_for_arduino_acknowledgement()

                # send back max_value_limited_spectrum -------------------------
                self.serial_port.write('I')

                self.serial_port.write(3 + 2)

                self.serial_port.write('S')

                # identifier for the receiving side of the Iridium message
                self.serial_port.write('M')
                self.serial_port.write('V')
                self.serial_port.write('L')

                self.send_content_binary_file(path_in_processing + '_max_value_limited_spectrum.bdat')

                # wait for transmission by Iridium done
                self.wait_for_arduino_acknowledgement()

                # send back binary_red_spectrum --------------------------------
                self.serial_port.write('I')

                path_to_BRS = path_in_processing + '_spectral_properties.bdat'
                size_in_bytes = int(os.path.getsize(path_to_BRS))
                self.serial_port.write(3 + size_in_bytes)

                self.serial_port.write('S')

                # identifier for the receiving side of the Iridium message
                self.serial_port.write('B')
                self.serial_port.write('R')
                self.serial_port.write('S')

                self.send_content_binary_file(path_to_BRS)

                # wait for transmission by Iridium done
                self.wait_for_arduino_acknowledgement()

            # transmit all data in a file (at least part of it)
            if current_command[0:3] == 'TRT':
                filename_to_transmit = current_command[3:]
                # TODO

    def send_content_binary_file(self, path_to_file):
        """Send by serial the content of a binary file to the Arduino Mega."""

        with open(path_to_file, "rb") as f:
            byte = f.read(1)
            while byte != "":
                # send the byte to the Mega
                self.serial_port.write(byte)
                sleep(5.0 / 1000.0)  # sleep 5 ms

                # read next byte
                byte = f.read(1)

    def launch_RPi_command(self):
        """Launch all RPi processing."""

        if self.verbose > 0:
            printi("Start RPi processing")

        self.open_serial_to_arduino()
        self.connect_to_Arduino()
        self.receive_from_Arduino()
        self.save_all()
        self.processing()
        self.send_over_Iridium()

        if self.verbose > 0:
            printi("Done with everything!")

        self.confirm_finished()
