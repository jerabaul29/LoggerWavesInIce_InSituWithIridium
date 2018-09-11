import load_status_information

# look at one data file

path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = 'J Rt/'
example_file = 'Mon_6_Aug_2018_09:27:39_+0200.bin'

load_status_information.load_status_information(path_to_repo_Irdium_data + folder_test + example_file, verbose=2)

# another

path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = 'J Rt/'
example_file = 'Mon_6_Aug_2018_09:33:42_+0200.bin'

load_status_information.load_status_information(path_to_repo_Irdium_data + folder_test + example_file, verbose=2)

# another

path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = 'J Rt/'
example_file = 'Mon_6_Aug_2018_09:35:45_+0200.bin'

load_status_information.load_status_information(path_to_repo_Irdium_data + folder_test + example_file, verbose=2)
