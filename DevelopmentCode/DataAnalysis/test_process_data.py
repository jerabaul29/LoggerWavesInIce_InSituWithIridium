import process_data
from datetime import datetime
from datetime import timedelta

path_to_repo = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = '300234066333770'

data_manager = process_data.DataManager(path_to_repo=path_to_repo, verbose=0)

# bundle new data that may have been uploaded on the repor
data_manager.bundle_data_folder(folder_test)

# some example of how to show a number of spectra
data_manager.show_spectrum(folder=folder_test,
                           time_start=datetime(year=2018, month=03, day=21, hour=12, tzinfo=None),
                           time_end=datetime(year=2018, month=03, day=25, hour=12, tzinfo=None),
                           min_delay=timedelta(hours=12),
                           save_fig=True)

data_manager.show_spectrogram(folder=folder_test,
                              time_start=datetime(year=2018, month=03, day=19, hour=12, tzinfo=None),
                              time_end=datetime(year=2018, month=03, day=25, hour=12, tzinfo=None),
                              save_fig=True,
                              noise_normalize=False
                              )

data_manager.show_battery(folder=folder_test,
                          time_start=datetime(year=2018, month=03, day=19, hour=12, tzinfo=None),
                          time_end=datetime(year=2018, month=03, day=25, hour=20, tzinfo=None),
                          save_fig=True,
                          )
