from __future__ import print_function
import numpy as np
from binascii import hexlify, unhexlify
from struct import unpack
import os
import fnmatch
from crcmod import mkCrcFun
crc16 = mkCrcFun(0x11021, 0x0000, False, 0x0000)

# length of one binary dataframe
length_binary_frame = 124
# number of 4 bits floating points in one frame
number_of_VN100_fields = 29


def checksum_vn100(binary_data):
    """
    checksum binary_data
    """

    crc1 = np.int('0x' + binary_data[-4:], 16)
    crc2 = crc16(unhexlify(binary_data[2:-4]))
    if crc1 == crc2:
        return True
    else:
        return False


def add_entry_list_strings(list_string, entry):
    """
    add a new entry to a list of strings, including some newlines
    """

    list_string.append(entry)
    list_string.append('\n')

    return list_string


def parse_frame_vn100(data_frame, verbose=0):
    """
    parse one VN100 checksummed frame
    """

    output = np.zeros((number_of_VN100_fields,))

    for ind_data in range(number_of_VN100_fields):
        ind_float_start = 8 + 4 + 4 * 2 * ind_data
        ind_float_final = ind_float_start + 4 * 2
        current_binary_bytes = data_frame[ind_float_start:ind_float_final]

        if verbose > 5:
            print("Parsing bytes:")
            display_binary_data(current_binary_bytes)

        current_value = np.float(unpack('<f', unhexlify(current_binary_bytes))[0])
        output[ind_data] = current_value

    if verbose > 5:
        print("Parsed VN100 data frame")
        print(output)

    return output


def print_parsed_frame_vn100(output):
    """
    user friendly print of VN100 parsed data
    """

    # Mag
    print(output[0:3])
    # Accel
    print(output[3:6])
    # Gyro
    print(output[6:9])
    # Temp
    print(output[9])
    # Pres
    print(output[10])
    # YawPitchRoll
    print(output[11:14])
    # DCM
    print(output[14:23].reshape((3, 3)))
    # MagNed
    print(output[23:26])
    # AccNed
    print(output[26:29])


def convert_numpy_to_scientific_string(numpy_array):
    """
    converts a numpy array in a string composed of scientific notation numbers
    """

    list_strings = []

    for value in numpy_array:
        list_strings.append("%e" % value)
        list_strings.append(",")

    string_result = "".join(list_strings)

    return string_result


def display_binary_data(binary_data):
    """
    Display some binary data in hex format
    """

    print("Print binary data")

    length_data = len(binary_data)
    print("Length of binary data as ASCII: " + str(length_data))

    str_print = ""

    for ind in range(int(length_data / 2)):
        str_print += binary_data[2 * ind:2 * ind + 2]
        str_print += " "

    print(str_print)


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
            print("bad end of line at the end of the file; probably due to power disconnect")
            return (accumulator, current_index - 1)
            break

        current_index += 1

        if current_char == '\n':
            return (accumulator, current_index)

        else:
            accumulator.append(current_char)


class Parser_logger():
    """
    A class for helping to parse output from the Waves in ice loggers
    """

    def __init__(self, path=None, path_output=None, verbose=0):
        self.path = path
        self.path_output = path_output
        self.verbose = verbose

    def process_folder(self):
        """
        Process all the files in the self.path folder
        """

        verbose = self.verbose

        for file_crrt in os.listdir(self.path):
            if fnmatch.fnmatch(file_crrt, 'F*'):

                if self.verbose > 0:
                    print("Processing file: " + str(file_crrt))

                self.load_file(self.path + file_crrt, verbose=verbose)

                self.parse_current_data(self.path_output + file_crrt, verbose=verbose)

    def load_file(self, path_to_file):
        verbose = self.verbose

        with open(path_to_file, 'r') as file:
            self.current_data = file.read()

        self.current_data_length = len(self.current_data)

        if verbose > 0:
            print("Total data length: " + str(self.current_data_length))

    def parse_current_data(self, path_output):
        verbose = self.verbose

        current_data_index = 0

        list_strings_log_S = []

        list_strings_log_R = []
        list_strings_log_R_time = []

        list_strings_log_C = []
        list_strings_log_C_time = []

        list_strings_log_G = []
        list_strings_log_G_time = []

        list_strings_log_B = ['MagX, MagY, MagZ, AccX, AccY, AccZ, GyroX, GyroY, GyroZ, Temp, Pres, Yaw, Pitch, Roll, DCM1, DCM2, DCM3, DCM4, DCM5, DCM6, DCM7, DCM8, DCM9, MagNED1, MagNED2, MagNED3, AccNED1, AccNED2, ACCNED3, \n']
        list_strings_log_B_time = []

        list_strings_log_broken = []
        list_strings_log_broken_time = []

        # expected_next_timestamp says what if a timestamp is expected
        # 0: no timestamp expected
        expected_next_timestamp = 0

        # while some data to analyse in the file, go through it
        while current_data_index < self.current_data_length:

            if verbose > 5:
                print("Current index: " + str(current_data_index))

            # coming next may be just an empty line
            if self.current_data[current_data_index] == '\n' or self.current_data[current_data_index] == '\r':
                current_data_index += 1

                if verbose > 5:
                    print("Newline char")

            else:
                # at this point, look for the indication of which type of data to expect
                next_two_chars = self.current_data[current_data_index:current_data_index + 2]
                current_data_index += 2

                if verbose > 5:
                    print("Current next two chars: " + str(next_two_chars))

                # case information about timestamp in milliseconds
                if next_two_chars == 'M,':

                    if verbose > 5:
                        print("Hit start of a milliseconds timestamp")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: " + message_string)

                    if expected_next_timestamp == 0:
                        print("Expected no timestamp!")

                    elif expected_next_timestamp == 2:
                        list_strings_log_R_time = add_entry_list_strings(list_strings_log_R_time, message_string)
                        list_strings_log_C_time = add_entry_list_strings(list_strings_log_C_time, message_string)

                    elif expected_next_timestamp == 3:
                        list_strings_log_G_time = add_entry_list_strings(list_strings_log_G_time, message_string)

                    elif expected_next_timestamp == 4:
                        list_strings_log_B_time = add_entry_list_strings(list_strings_log_B_time, message_string)

                    elif expected_next_timestamp == 5:
                        list_strings_log_broken_time = add_entry_list_strings(list_strings_log_broken_time, message_string)

                # case start message of a file
                elif next_two_chars == 'S,':

                    if verbose > 0:
                        print("Hit start of a file")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: " + message_string)

                    expected_next_timestamp = 0
                    list_strings_log_S = add_entry_list_strings(list_strings_log_S, message_string)

                # case information about battery level (raw)
                elif next_two_chars == 'R,':

                    if verbose > 0:
                        print("Hit start of a raw reading battery message")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: " + message_string)

                    expected_next_timestamp = 0
                    list_strings_log_R = add_entry_list_strings(list_strings_log_R, message_string)

                # case information about converted battery level
                elif next_two_chars == 'C,':

                    if verbose > 0:
                        print("Hit start of a converted level battery message")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: " + message_string)

                    expected_next_timestamp = 2
                    list_strings_log_C = add_entry_list_strings(list_strings_log_C, message_string)

                # case GPS data
                elif next_two_chars == '$G':

                    if verbose > 5:
                        print("Hit start of a GPS data string")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: G" + message_string)

                    expected_next_timestamp = 3
                    list_strings_log_G.append('G')
                    list_strings_log_G = add_entry_list_strings(list_strings_log_G, message_string)

                # case binary data
                elif next_two_chars == 'B,':

                    if verbose > 5:
                        print("Hit start of a binary data frame")

                    current_data_index += 3
                    current_binary_data = hexlify(self.current_data[current_data_index:current_data_index + length_binary_frame])
                    current_data_index += length_binary_frame

                    if verbose > 5:
                        display_binary_data(current_binary_data)

                    validity_checksum = checksum_vn100(current_binary_data)

                    if verbose > 5:
                        print("Validity checksum " + str(validity_checksum))

                    if validity_checksum:
                        output = parse_frame_vn100(current_binary_data, verbose=verbose)

                        if verbose > 5:
                            print_parsed_frame_vn100(output)

                    else:
                        output = np.array([0])

                    expected_next_timestamp = 4
                    list_strings_log_B = add_entry_list_strings(list_strings_log_B, convert_numpy_to_scientific_string(output))

                # or broken message
                else:

                    if verbose > 0:
                        print("Broken message, read until next line break")

                    (message, current_data_index) = load_until_end_line(self.current_data, current_data_index)
                    message_string = ''.join(message)

                    if verbose > 5:
                        print("Message: " + message_string)

                    expected_next_timestamp = 5
                    list_strings_log_broken = add_entry_list_strings(list_strings_log_broken, message_string)

        # generate the data strings and save them
        S_data = "".join(list_strings_log_S)
        with open(path_output + "_S", "w") as text_file:
            text_file.write(S_data)

        R_data = "".join(list_strings_log_R)
        Rt_data = "".join(list_strings_log_R_time)
        with open(path_output + "_R", "w") as text_file:
            text_file.write(R_data)
        with open(path_output + "_Rt", "w") as text_file:
            text_file.write(Rt_data)

        C_data = "".join(list_strings_log_C)
        Ct_data = "".join(list_strings_log_C_time)
        with open(path_output + "_C", "w") as text_file:
            text_file.write(C_data)
        with open(path_output + "_Ct", "w") as text_file:
            text_file.write(Ct_data)

        G_data = "".join(list_strings_log_G)
        Gt_data = "".join(list_strings_log_G_time)
        with open(path_output + "_G", "w") as text_file:
            text_file.write(G_data)
        with open(path_output + "_Gt", "w") as text_file:
            text_file.write(Gt_data)

        B_data = "".join(list_strings_log_B)
        Bt_data = "".join(list_strings_log_B_time)
        with open(path_output + "_B", "w") as text_file:
            text_file.write(B_data)
        with open(path_output + "_Bt", "w") as text_file:
            text_file.write(Bt_data)

        br_data = "".join(list_strings_log_broken)
        brt_data = "".join(list_strings_log_broken_time)
        with open(path_output + "_P", "w") as text_file:
            text_file.write(br_data)
        with open(path_output + "_Pt", "w") as text_file:
            text_file.write(brt_data)
