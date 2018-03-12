from __future__ import print_function
import serial
import glob
from printind.printind_function import printi
from parser_logger import Parser_logger
from compute_statistics import WaveStatistics
from time import sleep
import os
from analyze_stream import AnalyzeStream

"""
TODO: do not tranmit reduced spectrum if no signal
TODO: test processing and sending back through Iridium together with the Mega
TODO: command for sending back whole parts of a file (TRT command)
NOTE: may need to help the Arduino for transmitting through the Iridium by cutting the file in different parts to accomodate
Iridium buffer size.
"""


class RPi_control(object):
    def __init__(self, verbose=0, main_path='/home/pi/Logger/', debug=False):
        self.verbose = verbose
        self.main_path = main_path
        self.debug = debug

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

                if self.verbose > 5:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'B':
                    self.serial_port.write('R')

                wait_for_acknowledged_connection = False

        if self.verbose > 5:
            printi("Connection Acknowledged")

    def receive_from_Arduino(self):

        self.list_of_commands = []
        self.filename = ""
        self.data = ""

        # received commands ----------------------------------------------------
        if self.verbose > 0:
            printi("Receive commands")

        wait_for_commands = True
        current_command = ['DEFAULT']

        while wait_for_commands:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 5:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'C':
                    self.list_of_commands.append(''.join(current_command))
                    current_command = []

                elif crrt_char == 'N':
                    self.list_of_commands.append(''.join(current_command))
                    wait_for_commands = False

                else:
                    current_command.append(crrt_char)

        # receive filename ----------------------------------------------------
        if self.verbose > 5:
            printi("Receive Filename")

        self.serial_port.write('N')

        wait_for_filename = True
        filename_list = []

        while wait_for_filename:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()

                if self.verbose > 5:
                    printi('Received char: ' + str(crrt_char))

                if crrt_char == 'N':
                    wait_for_filename = False
                    self.serial_port.write('N')

                else:
                    filename_list.append(crrt_char)

        self.filename = ''.join(filename_list)

        if self.verbose > 0:
            print("filename received: " + self.filename)

        # receive data ---------------------------------------------------------
        if self.verbose > 0:
            printi("Receive data")

        data_list = []
        message_start = "START_TRANSMIT_FILE"
        message_end = "END_TRANSMIT_FILE"

        # receive the start message
        analyze_stream = AnalyzeStream(message_start)

        while not analyze_stream.has_found_message():
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()
                analyze_stream.read_char(crrt_char)

        if self.verbose > 0:
            print("received " + message_start)

        # receive the data including the end message
        analyze_stream = AnalyzeStream(message_end)

        while not analyze_stream.has_found_message():
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()
                data_list.append(crrt_char)
                analyze_stream.read_char(crrt_char)

        if self.verbose > 0:
            print("received " + message_end)

        # generate the data, taking away end message
        self.data = ''.join(data_list[:-len(message_end)])

    def save_all(self):
        """Save received commands and data, and append the filename list."""

        if self.verbose > 0:
            printi("Append filename_list.txt")

        with open(self.main_path + 'filename_list.txt', "a") as myfile:
            myfile.write(self.filename)
            myfile.write('\n')
            myfile.write("\n")

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

        # NOTE: can be some problems with paths here...

        # do the parsing of the file from the Mega -----------------------------
        path_in = self.main_path + '/Data/'
        path_out_logger = self.main_path + '/ResultAnalyzis/'

        data_parser = Parser_logger(verbose=1)
        data_parser.load_file(path_in + self.filename)
        data_parser.parse_current_data(path_out_logger + self.filename)

        # do the analyzis of the data ------------------------------------------
        if self.debug:
            print("CAREFUL: IN DEBUG MODE! USE DUMMY DATA")
            path_in_processing = '/home/jrlab/Data/WOICE/pi_logger/ResultAnalyzis/'
            instance_compute_statistics = WaveStatistics(path_in=path_in_processing, filename='test_00470_00471.csv', verbose=self.verbose)
        else:
            path_in_processing = path_out_logger
            instance_compute_statistics = WaveStatistics(path_in=path_in_processing, filename=self.filename + "_B", verbose=self.verbose)

        instance_compute_statistics.perform_all_processing()

        if self.debug:
            instance_compute_statistics.writeData(path_output="/home/jrlab/Desktop/Current/Logger/ResultAnalyzis/" + "/" + self.filename + ".bin")
        else:
            instance_compute_statistics.writeData()

    def wait_for_arduino_acknowledgement(self):
        while True:
            if self.serial_port.in_waiting > 0:
                crrt_char = self.serial_port.read()
                if crrt_char == 'R':
                    break

    def send_over_Iridium(self):  # TODO: change from here
        """Send the processed data over Iridium"""

        # when should send through Iridium, ask it to the Mega as:
        # I [#numberOfBytes] S

        path_in_processing = self.main_path + 'ResultAnalyzis/'

        # go through commands received by Iridium
        for current_command in self.list_of_commands:

            # default
            # NOTE: could transmit as an answer to a request by Iridium by
            # applying controls here
            if current_command == 'DEFAULT':
                current_binary_file = path_in_processing + self.filename + ".bin"

                if self.verbose > 0:
                    print("sending the content of file: {}".format(current_binary_file))

                self.serial_port.write("PROCESSING_OK")

                self.send_content_binary_file(current_binary_file)

                self.serial_port.write("PROCESSED_END")

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
