from __future__ import print_function
import process_data
from datetime import datetime
from datetime import timedelta
import matplotlib.pyplot as plt
import os

# path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'

path_home = os.path.expanduser("~")
path_to_repo_Irdium_data = path_home + '/Desktop/Git/IridiumData_Svalbard2018/'

if not os.path.isdir(path_to_repo_Irdium_data):
    raise RuntimeError("You must have the iridium data github repo at ~/Desktop/Git/IridiumData_Svalbard2018")

# folder_test = 'J Rt'
# folder_test = "300234066587240"
folder_test = "300234066687520"
# folder_test = "300234066885440"
# folder_test = "300234066885480"

time_start = datetime(year=2018, month=8, day=1, hour=23, tzinfo=None)
time_end = datetime(year=2018, month=8, day=11, hour=23, tzinfo=None)
min_delay = timedelta(hours=0, minutes=20)

data_manager = process_data.DataManager(path_to_repo_Irdium_data=path_to_repo_Irdium_data, verbose=5)

# bundle new data that may have been uploaded on the repor
data_manager.bundle_data_folder(folder_test)

# an example of how to show the battery level
data_manager.show_battery(folder=folder_test,
                          time_start=time_start,
                          time_end=time_end,
                          save_fig=True,
                          )

plt.show()

# NOT REMOVING NOISE %%

data_manager.show_directional_spectrum(folder=folder_test,
                                       time_start=time_start,
                                       time_end=time_end,
                                       min_delay=min_delay,
                                       save_fig=True,
                                       max_number=3,
                                       show_all=False)

# an example of how to show a number of spectra
data_manager.show_spectrum(folder=folder_test,
                           time_start=time_start,
                           time_end=time_end,
                           min_delay=min_delay,
                           save_fig=True,
                           remove_noise=False)

# an example of how to show data as a spectrogram
data_manager.show_spectrogram(folder=folder_test,
                              time_start=time_start,
                              time_end=time_end,
                              save_fig=True,
                              noise_normalize=False,
                              remove_noise=False
                              )

plt.show()

# REMOVING NOISE %%

# an example of how to show a number of spectra
data_manager.show_spectrum(folder=folder_test,
                           time_start=time_start,
                           time_end=time_end,
                           min_delay=min_delay,
                           save_fig=True,
                           remove_noise=True)

# an example of how to show data as a spectrogram
data_manager.show_spectrogram(folder=folder_test,
                              time_start=time_start,
                              time_end=time_end,
                              save_fig=True,
                              noise_normalize=False,
                              remove_noise=True,
                              peak_frequency='T_z'
                              )

plt.show()
