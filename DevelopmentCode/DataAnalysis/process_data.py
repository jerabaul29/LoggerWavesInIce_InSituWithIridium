"""TODO: automatically:

- dictionary at root of each folder about the metadata and bundling

- automatic bundling of data

- automatic processing
- automatic plotting if needed

- generate dicts:
-- for bookkeeping
-- for bundled data
NOTE: for the generated dicts: use key corresponding to the time of start of recording

- bundle together status strings and binary messages
- generate dictionaries with the data
- add on github and push

TODO other files: get a list of new pushed messages and things to update
keep a bookkeeping of what information received etc.
"""

from __future__ import print_function
import os
import pickle
import load_Iridium_wave_data
from datetime import datetime
from dateutil import parser
from datetime import timedelta
import fnmatch
import matplotlib.pyplot as plt
import numpy as np
import pytz
import load_status_information
import matplotlib.dates as mdates
from matplotlib.patches import Rectangle

now_utc = datetime.now(pytz.utc)
today_utc = now_utc.date()

# max duration between different files that should be bundled together
MAX_DURATION_BETWEEN_FILES_S = 15 * 60
SIZE_FILE_DATA_BITS = 340
MAX_VALUE_SPECTRUM_REAL = 0.001


def timestamp_from_filename(filename, verbose=0):
    filename_modified = filename.replace("_", " ")[0:-4]
    datetime_object = parser.parse(filename_modified, ignoretz=True)

    if verbose > 1:
        print("filename: {}".format(filename))
        print("timestamp: {}".format(datetime_object))

    return(datetime_object)


def associate(dict_non_associated, verbose=0):
    list_newly_associated = []
    dict_new_associations = {}

    # go through the dict
    for crrt_file in dict_non_associated:

        # only try to associate those that have not been associated yet
        if crrt_file not in list_newly_associated:

            if verbose > 2:
                print("trying to associate: {}".format(crrt_file))

            crrt_list_associated = []

            # search is another file in +- MAX_DURATION_BETWEEN_FILES_S
            for another_file in dict_non_associated:

                # cannot associate against itself  and   cannot associate several times
                if (another_file is not crrt_file) and (another_file not in list_newly_associated):

                    if verbose > 2:
                        print("seeing if could be associated with {}".format(another_file))

                    if (abs((dict_non_associated[crrt_file] - dict_non_associated[another_file]).total_seconds()) < MAX_DURATION_BETWEEN_FILES_S):
                        if verbose > 2:
                            print("associate!")

                        crrt_list_associated.append(crrt_file)
                        crrt_list_associated.append(another_file)

                        list_newly_associated.append(crrt_file)
                        list_newly_associated.append(another_file)

            crrt_list_associated = list(set(crrt_list_associated))
            list_newly_associated = list(set(list_newly_associated))

            crrt_list_associated = sorted(crrt_list_associated, key=timestamp_from_filename)

            if verbose > 2:
                print("crrt_list_associated after going through files: {}".format(crrt_list_associated))

            if crrt_list_associated:

                min_timestamp_string = str(min([dict_non_associated[key] for key in crrt_list_associated])).replace(" ", "_")

                if verbose > 2:
                    print("date of oldest of the list to associate: {}".format(min_timestamp_string))

                dict_new_associations[min_timestamp_string] = crrt_list_associated

    if verbose > 0:
        print("list_newly_associated: {}".format(list_newly_associated))
        print("dict_new_associations: {}".format(dict_new_associations))

    return(list_newly_associated, dict_new_associations)


def is_binary_data():
    # when only 1 binary data, easy: size is 340 bytes
    pass


def is_status_string():
    pass
    # use features that repeat: V, ., F, GPRMC, others.


class DataManager(object):
    def __init__(self, path_to_repo, verbose=0):
        self.path_to_repo = path_to_repo
        self.verbose = verbose

    def generate_pickled_information_name(self, folder):
        return(self.path_to_repo + '/' + folder + '/information.pkl')

    def bundle_data_folder(self, folder):
        # load the pickled information
        information = self.load_information_folder(folder)

        if self.verbose > 2:
            print("information dict: ")
            print(information)

        # list all files
        all_files = []
        for crrt_file in os.listdir(self.path_to_repo + '/' + folder):
            if fnmatch.fnmatch(crrt_file, '*.bin'):
                all_files.append(crrt_file)

        if self.verbose > 2:
            print("all files:")
            print(all_files)

        # work on all files that are not associated yet
        not_associated = list(set(all_files) - set(information["associated_files"]))

        if self.verbose > 0:
            print("non associated files: ")
            print(not_associated)

        # get all timestamps of arrival for non associated files
        dict_non_associated = {}
        for crrt_file in not_associated:
            dict_non_associated[crrt_file] = timestamp_from_filename(crrt_file, verbose=self.verbose)

        # apply rules for associating files
        (list_newly_associated, dict_new_associations) = associate(dict_non_associated, verbose=self.verbose)

        # update associated files
        information["associated_files"] = information["associated_files"] + list_newly_associated
        information["association_tables"].update(dict_new_associations)

        # write information
        self.write_information_folder(folder, information)

    def load_information_folder(self, folder):
        filename_pickled_information = self.generate_pickled_information_name(folder)

        # create pickle dump if does not exist
        if not os.path.exists(filename_pickled_information):
            with open(filename_pickled_information, 'wb') as crrt_file:
                information = {}
                information["associated_files"] = []
                information["association_tables"] = {}
                pickle.dump(information, crrt_file, protocol=pickle.HIGHEST_PROTOCOL)

        # load information
        with open(filename_pickled_information, 'rb') as crrt_file:
            information = pickle.load(crrt_file)

        return(information)

    def write_information_folder(self, folder, information):
        filename_pickled_information = self.generate_pickled_information_name(folder)

        with open(filename_pickled_information, 'wb') as crrt_file:
            pickle.dump(information, crrt_file, protocol=pickle.HIGHEST_PROTOCOL)

    def associated_to_use(self, folder, time_start=None, time_end=None, min_delay=None):

        if time_end and time_start:
            assert(time_start < time_end)

        information = self.load_information_folder(folder)

        associated = information["association_tables"]

        keys_to_use = associated.keys()

        if self.verbose > 2:
            print("all keys: {}".format(keys_to_use))

        if time_start is not None:
            keys_to_use = [crrt_key for crrt_key in keys_to_use if timestamp_from_filename(crrt_key) > time_start]

        if time_end is not None:
            keys_to_use = [crrt_key for crrt_key in keys_to_use if timestamp_from_filename(crrt_key) < time_end]

        if self.verbose > 2:
            print("keys_to_use after time constraints: {}".format(keys_to_use))

        keys_to_use = sorted(keys_to_use, key=timestamp_from_filename)

        if self.verbose > 2:
            print("list_sorted_keys: {}".format(keys_to_use))

        if min_delay is not None:
            all_possible_keys = list(keys_to_use)  # make a copy
            keys_to_use = []
            crrt_key = all_possible_keys[0]
            keys_to_use.append(crrt_key)

            for next_key in all_possible_keys:
                if (timestamp_from_filename(next_key) - timestamp_from_filename(crrt_key) > min_delay):
                    crrt_key = next_key
                    keys_to_use.append(crrt_key)

                    if self.verbose > 0:
                        print("adding key {}".format(crrt_key))

        return(keys_to_use)

    def show_spectrum(self, time_start=None, time_end=None, min_delay=None, folder=None, save_fig=False):
        information = self.load_information_folder(folder)
        associated = information["association_tables"]

        if folder is not None:

            # find list of keys that are valid
            list_keys = self.associated_to_use(folder, time_start, time_end, min_delay)

            plt.figure()

            for crrt_key in list_keys:
                if self.verbose > 2:
                    print("using crrt_key {}".format(crrt_key))
                    print("value: {}".format(associated[crrt_key]))

                file_to_load = self.path_to_repo + folder + "/" + associated[crrt_key][1]

                if self.verbose > 0:
                    print("loading file: {}".format(file_to_load))

                crrt_dict = load_Iridium_wave_data.load_data(file_to_load)
                (SWH, T_z0, Hs, T_z, freq, fmin, fmax, nfreq, a0_proc, a1_proc, a2_proc, b1_proc, b2_proc, R_proc) = load_Iridium_wave_data.expand_raw_variables(crrt_dict)
                plt.plot(freq, a0_proc, label=crrt_key)

            noise = (0.24 * 9.81e-3)**2 * ((2 * np.pi * freq)**(-4))
            plt.plot(freq, noise, label='noise', color='k', linestyle='--', linewidth=2)
            plt.xlabel(r'$\mathrm{f} \, / \, \mathrm{Hz}$')
            plt.ylabel(r'$S \, / \,\mathrm{m}^2 \mathrm{Hz}^{-1}$')
            plt.legend()
            # plt.yscale('log')
            plt.yscale('linear')
            plt.xlim([fmin, fmax])
            plt.ylim([0, MAX_VALUE_SPECTRUM_REAL])
            plt.tight_layout()

            if save_fig:
                figure_path = self.path_to_repo + folder + "/Figures/"

                if not os.path.exists(figure_path):
                    os.makedirs(figure_path)

                fig_name = "spectra_" + str(time_start) + "_" + str(time_end) + "_" + str(min_delay) + ".pdf"
                plt.savefig(figure_path + fig_name, format="pdf")

            plt.show()

        else:
            print("plot for all folders simultaneously not yet implemented")
        pass

    def show_spectrogram(self, time_start=None, time_end=None, folder=None, save_fig=False, noise_normalize=False):
        information = self.load_information_folder(folder)
        associated = information["association_tables"]

        if folder is not None:

            # find list of keys that are valid
            list_keys = self.associated_to_use(folder, time_start, time_end)

            list_data_to_plot = []
            X_axis = []

            for crrt_key in list_keys:
                if self.verbose > 2:
                    print("using crrt_key {}".format(crrt_key))
                    print("value: {}".format(associated[crrt_key]))

                file_to_load = self.path_to_repo + folder + "/" + associated[crrt_key][1]

                if self.verbose > 0:
                    print("loading file: {}".format(file_to_load))

                crrt_dict = load_Iridium_wave_data.load_data(file_to_load)
                (SWH, T_z0, Hs, T_z, freq, fmin, fmax, nfreq, a0_proc, a1_proc, a2_proc, b1_proc, b2_proc, R_proc) = load_Iridium_wave_data.expand_raw_variables(crrt_dict)

                if noise_normalize:
                    noise = (0.24 * 9.81e-3)**2 * ((2 * np.pi * freq)**(-4))
                    list_data_to_plot.append(np.array(a0_proc) / np.array(noise))
                else:
                    list_data_to_plot.append(a0_proc)

                X_axis.append(timestamp_from_filename(associated[crrt_key][1]))

            plt.figure()
            if noise_normalize:
                MAX_VALUE_SPECTRUM = 10
            else:
                MAX_VALUE_SPECTRUM = MAX_VALUE_SPECTRUM_REAL
            plt.pcolor(X_axis, freq, np.transpose(np.array(list_data_to_plot)), vmin=0, vmax=MAX_VALUE_SPECTRUM)
            plt.colorbar()
            plt.tight_layout()
            plt.xticks(rotation=90)
            # plt.locator_params(axis='x', nticks=10)

            if save_fig:
                figure_path = self.path_to_repo + folder + "/Figures/"

                if not os.path.exists(figure_path):
                    os.makedirs(figure_path)

                fig_name = "spectrogram_" + str(time_start) + "_" + str(time_end) + ".pdf"
                plt.savefig(figure_path + fig_name, format="pdf")

            plt.show()

        else:
            print("plot for all folders simultaneously not yet implemented")
        pass

    def show_battery(self, time_start=None, time_end=None, folder=None, save_fig=True):
        """None arguments: no limitations; shows either all folders, or unlimited time."""

        information = self.load_information_folder(folder)
        associated = information["association_tables"]

        if folder is not None:

            # find list of keys that are valid
            list_keys = self.associated_to_use(folder, time_start, time_end)

            list_battery_levels = []
            X_axis = []

            for crrt_key in list_keys:
                if self.verbose > 2:
                    print("using crrt_key {}".format(crrt_key))
                    print("value: {}".format(associated[crrt_key]))

                file_to_load = self.path_to_repo + folder + "/" + associated[crrt_key][0]

                if self.verbose > 0:
                    print("loading file: {}".format(file_to_load))

                dict_status_data = load_status_information.load_status_information(file_to_load)
                (battery_level, filename, GPRMC_info) = load_status_information.expand_status_information(dict_status_data)
                list_battery_levels.append(float(battery_level))
                X_axis.append(timestamp_from_filename(associated[crrt_key][1]))

            time_start = X_axis[0]
            time_end = X_axis[-1]

            # time_start
            time_start_std = datetime(year=time_start.year, month=time_start.month, day=time_start.day - 1)
            time_end_std = datetime(year=time_end.year, month=time_end.month, day=time_end.day + 1)

            delta = time_end_std - time_start_std

            fig = plt.figure()
            ax = fig.add_subplot(111)

            for i in range(delta.days + 1):
                current_day = time_start_std + timedelta(days=i)

                start_day = current_day + timedelta(hours=6)
                end_day = current_day + timedelta(hours=18)

                # convert to matplotlib date representation
                start = mdates.date2num(start_day)
                end = mdates.date2num(end_day)
                width = end - start

                # Plot rectangle
                rect = Rectangle((start, 0), width, 100, color='yellow')
                ax.add_patch(rect)

                start_night = current_day + timedelta(hours=18)
                end_might = current_day + timedelta(hours=24 + 6)

                # convert to matplotlib date representation
                start = mdates.date2num(start_night)
                end = mdates.date2num(end_might)
                width = end - start

                # Plot rectangle
                rect = Rectangle((start, 0), width, 100, color='gray')
                ax.add_patch(rect)

            plt.plot(X_axis, list_battery_levels, color='red', linewidth=2)
            plt.xticks(rotation=90)
            plt.ylabel("battery (V)")
            plt.xlim(X_axis[0], X_axis[-1])
            plt.ylim(min(list_battery_levels) - 0.05, max(list_battery_levels) + 0.05)

            if save_fig:
                figure_path = self.path_to_repo + folder + "/Figures/"

                if not os.path.exists(figure_path):
                    os.makedirs(figure_path)

                fig_name = "battery_" + str(time_start) + "_" + str(time_end) + ".pdf"
                plt.savefig(figure_path + fig_name, format="pdf")

            plt.show()

        else:
            print("plot for all folders simultaneously not yet implemented")
        pass

    def show_position(self, time_start=None, time_end=None, folder=None, background_image=None):
        pass
