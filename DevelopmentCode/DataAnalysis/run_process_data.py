from __future__ import print_function
import process_data
from datetime import datetime
from datetime import timedelta
import os

# path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'

path_home = os.path.expanduser("~")
path_to_repo_Irdium_data = path_home + '/Desktop/Git/IridiumData_Svalbard2018/'

if not os.path.isdir(path_to_repo_Irdium_data):
    raise RuntimeError("You must have the iridium data github repo at ~/Desktop/Git/IridiumData_Svalbard2018")

folder_test = '300234066333770'

data_manager = process_data.DataManager(path_to_repo_Irdium_data=path_to_repo_Irdium_data, verbose=5)

# bundle new data that may have been uploaded on the repor
data_manager.bundle_data_folder(folder_test)

# an example of how to show the battery level
data_manager.show_battery(folder=folder_test,
                          time_start=datetime(year=2018, month=03, day=19, hour=12, tzinfo=None),
                          time_end=datetime(year=2018, month=03, day=30, hour=20, tzinfo=None),
                          save_fig=True,
                          )

# NOT REMOVING NOISE %%

data_manager.show_directional_spectrum(folder=folder_test,
                                       time_start=datetime(year=2018, month=03, day=21, hour=12, tzinfo=None),
                                       time_end=datetime(year=2018, month=03, day=30, hour=12, tzinfo=None),
                                       min_delay=timedelta(hours=12),
                                       save_fig=True,
                                       remove_noise=False,
                                       max_number=20,
                                       show_all=True)

# an example of how to show a number of spectra
data_manager.show_spectrum(folder=folder_test,
                           time_start=datetime(year=2018, month=03, day=21, hour=12, tzinfo=None),
                           time_end=datetime(year=2018, month=03, day=30, hour=12, tzinfo=None),
                           min_delay=timedelta(hours=12),
                           save_fig=True,
                           remove_noise=False)

# an example of how to show data as a spectrogram
data_manager.show_spectrogram(folder=folder_test,
                              time_start=datetime(year=2018, month=03, day=19, hour=12, tzinfo=None),
                              time_end=datetime(year=2018, month=03, day=30, hour=12, tzinfo=None),
                              save_fig=True,
                              noise_normalize=False,
                              remove_noise=False
                              )

# REMOVING NOISE %%

# an example of how to show a number of spectra
data_manager.show_spectrum(folder=folder_test,
                           time_start=datetime(year=2018, month=03, day=21, hour=12, tzinfo=None),
                           time_end=datetime(year=2018, month=03, day=30, hour=12, tzinfo=None),
                           min_delay=timedelta(hours=12),
                           save_fig=True,
                           remove_noise=True)

# an example of how to show data as a spectrogram
data_manager.show_spectrogram(folder=folder_test,
                              time_start=datetime(year=2018, month=03, day=19, hour=12, tzinfo=None),
                              time_end=datetime(year=2018, month=03, day=30, hour=12, tzinfo=None),
                              save_fig=True,
                              noise_normalize=False,
                              remove_noise=True,
                              peak_frequency='T_z'
                              )
