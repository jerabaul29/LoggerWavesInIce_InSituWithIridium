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

list_folders = ["300234066587240",
                "300234066687520",
                "300234066885440",
                "300234066885480"]

time_start = datetime(year=2018, month=8, day=1, hour=23, tzinfo=None)
time_end = datetime(year=2018, month=8, day=11, hour=23, tzinfo=None)
min_delay = timedelta(hours=0, minutes=20)
max_number_directional_spectra = 1

for crrt_folder in list_folders:
    data_manager = process_data.DataManager(path_to_repo_Irdium_data=path_to_repo_Irdium_data, verbose=5)

    # bundle new data that may have been uploaded on the repor
    data_manager.bundle_data_folder(crrt_folder)

    # an example of how to show the battery level
    data_manager.show_battery(folder=crrt_folder,
                              time_start=time_start,
                              time_end=time_end,
                              save_fig=True,
                              )

    data_manager.show_directional_spectrum(folder=crrt_folder,
                                           time_start=time_start,
                                           time_end=time_end,
                                           min_delay=min_delay,
                                           save_fig=True,
                                           max_number=max_number_directional_spectra,
                                           show_all=False)

    # an example of how to show a number of spectra
    data_manager.show_spectrum(folder=crrt_folder,
                               time_start=time_start,
                               time_end=time_end,
                               min_delay=min_delay,
                               save_fig=True,
                               remove_noise=False)

    # an example of how to show data as a spectrogram
    data_manager.show_spectrogram(folder=crrt_folder,
                                  time_start=time_start,
                                  time_end=time_end,
                                  save_fig=True,
                                  noise_normalize=False,
                                  remove_noise=False
                                  )

    plt.show()
