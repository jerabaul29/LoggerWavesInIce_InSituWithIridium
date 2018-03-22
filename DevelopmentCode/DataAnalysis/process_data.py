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
from datetime import datetime
from dateutil import parser
import fnmatch

# max duration between different files that should be bundled together
MAX_DURATION_BETWEEN_FILES_S = 15 * 60
SIZE_FILE_DATA_BITS = 340


def timestamp_from_filename(filename, verbose=0):
    filename_modified = filename.replace("_", " ")[0:-4]
    datetime_object = parser.parse(filename_modified)

    if verbose > 0:
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
            dict_non_associated[crrt_file + "_timestamp"] = timestamp_from_filename(crrt_file, verbose=self.verbose)

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

    def show_spectrum_one_instrument(self, time_start, time_end, folder):
        pass

    def show_spectrum_all_instruments(self, time_start, time_end):
        pass

    def show_battery_one_instrument(self, time_start, time_end, folder):
        pass

    def show_position_one_instrument(self, time_start, time_end, background_image, folder):
        pass
