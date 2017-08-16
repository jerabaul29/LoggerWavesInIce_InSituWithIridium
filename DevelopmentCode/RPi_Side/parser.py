from crcmod import mkCrcFun
import numpy as np
from binascii import hexlify, unhexlify
from struct import unpack
import os
import fnmatch
from printind.printind_function import printi


def load_until_end_line(data, start_index):
    """
    Loads the data in an accumulator starting at start_index until hit end of line character
    Return the accumulator
    """

    accumulator = []
    current_index = start_index
    hit_end_of_line = False

    while not hit_end_of_line:
        try:
            current_char = data[current_index]
        except IndexError:
            print "bad end of line at the end of the file; probably due to power disconnect"
            return (accumulator, current_index - 1)
            break

        current_index += 1

        if current_char == '\n':
            return (accumulator, current_index)

        else:
            accumulator.append(current_char)


def add_entry_list_strings(list_string, entry):
    """
    add a new entry to a list of strings, including some newlines
    """

    list_string.append(entry)
    list_string.append('\n')

    return list_string


class Parser_logger():
    """
    A class for parsing one logging file.
    """

    def __init__(self, path_in, path_out, filename, verbose=0):
        """Parse all data in the folder structure (should agree with the definition
        in the docstring of the class).

        path: path to the root of the folder structure
        path_out: where to save the data
        """
        self.path_in = path_in
        self.path_out = path_out
        self.filename = filename
        self.verbose = verbose

    def process_file(self):
        self.load_file(self.path_in + self.filename, verbose=self.verbose)
        self.parse_current_data(self.path_out + self.filename, verbose=self.verbose)

    def load_file(self, path_to_file, verbose=0):
        with open(path_to_file, 'r') as file:
            self.current_data = file.read()

        self.current_data_length = len(self.current_data)

        if verbose > 0:
            print "Total data length: " + str(self.current_data_length)

    def parse_current_data(self, path_out, verbose=0):
        current_data_index = 0

        list_strings_log_G = []
        list_strings_log_G_time = []

        list_strings_log_I = []
        list_strings_log_I_time = []

        list_strings_log_broken = []
        list_strings_log_broken_time = []

        # expected_next_timestamp says what if a timestamp is expected
        # 0: no timestamp expected
        # 1: timestamp expected for GPS
        # 2: timestamp expected for IMU
        expected_next_timestamp = 0

        # while some data to analyse in the file, go through it
        while current_data_index < self.current_data_length:

            if verbose > 1:
                print "Current index: " + str(current_data_index)

            # coming next may be just an empty line
            if self.current_data[current_data_index] == '\n' or self.current_data[current_data_index] == '\r':
                current_data_index += 1

                if verbose > 0:
                    print "Newline char"

            # if not, start of a new message
            else:
                # at this point, look for the indication of which type of data to expect
                next_two_chars = self.current_data[current_data_index:current_data_index + 2]
                current_data_index += 2

                if verbose > 1:
                    print "Current next two chars: " + str(next_two_chars)

                # case information about timestamp in milliseconds
                if next_two_chars == 'M,':

                    if verbose > 0:
                        print "Hit start of a milliseconds timestamp"

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 0:
                        print "Message: " + message_string

                    if expected_next_timestamp == 0:
                        print "Expected no timestamp!"

                    elif expected_next_timestamp == 1:
                        list_strings_log_G_time = add_entry_list_strings(list_strings_log_G_time, message_string)

                    elif expected_next_timestamp == 2:
                        list_strings_log_I_time = add_entry_list_strings(list_strings_log_I_time, message_string)

                # case GPS data
                elif next_two_chars == '$G':

                    if verbose > 0:
                        print "Hit start of a GPS data string"

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 0:
                        print "Message: $G" + message_string

                    expected_next_timestamp = 1
                    list_strings_log_G.append('G')
                    list_strings_log_G = add_entry_list_strings(list_strings_log_G, message_string)

                # case IMU data
                elif next_two_chars == 'I;':

                    if verbose > 0:
                        print "Hit start of an IMU message"

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 0:
                        print "Message: I;" + message_string

                    expected_next_timestamp = 2
                    list_strings_log_I = add_entry_list_strings(list_strings_log_I, message_string)

                # case broken message
                else:

                    if verbose > 0:
                        print "Broken message, read until next line break"

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 0:
                        print "Message: " + message_string

                    expected_next_timestamp = -1
                    list_strings_log_broken = add_entry_list_strings(list_strings_log_broken, message_string)

        # generate the data strings and save them
        G_data = "".join(list_strings_log_G)
        Gt_data = "".join(list_strings_log_G_time)
        with open(path_out + "_G", "w") as text_file:
            text_file.write(G_data)
        with open(path_out + "_Gt", "w") as text_file:
            text_file.write(Gt_data)

        I_data = "".join(list_strings_log_I)
        It_data = "".join(list_strings_log_I_time)
        with open(path_out + "_I", "w") as text_file:
            text_file.write(I_data)
        with open(path_out + "_It", "w") as text_file:
            text_file.write(It_data)
