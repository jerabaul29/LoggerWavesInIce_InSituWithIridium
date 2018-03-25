from __future__ import print_function
import pynmea2

SIZE_MSG_BATTERY = 4
SIZE_MSG_FILENAME = 6


def load_status_information(filename, verbose=0):
    dict_data = {}

    with open(filename, 'rb') as f:
        data = f.read()

    if verbose > 3:
        for crrt_char in data:
            print(crrt_char)

    battery_level = data[0:SIZE_MSG_BATTERY]
    filename = data[SIZE_MSG_BATTERY: SIZE_MSG_BATTERY + SIZE_MSG_FILENAME]

    GPRMC_string = []

    for crrt_char in data[SIZE_MSG_BATTERY + SIZE_MSG_FILENAME:]:
        if crrt_char == '\n' or crrt_char == '\r':
            break
        else:
            GPRMC_string.append(crrt_char)

    GPRMC_string = "".join(GPRMC_string)

    if verbose > 0:
        print("battery_level: {}".format(battery_level))
        print("filename: {}".format(filename))
        print("GPRMC string: {}".format(GPRMC_string))

    dict_data["battery_level_V"] = battery_level
    dict_data["filename"] = filename
    dict_data["GPRMC_data"] = pynmea2.parse(GPRMC_string)

    return(dict_data)


def expand_status_information(dict_data):

    battery_level = dict_data["battery_level_V"]
    filename = dict_data["filename"]
    GPRMC_data = dict_data["GPRMC_data"]

    return(battery_level, filename, GPRMC_data)
