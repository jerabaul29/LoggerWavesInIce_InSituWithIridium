import load_status_information

path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = '300234066333770/'
example_file = 'Fri_23_Mar_2018_04:14:52_+0000.bin'

load_status_information.load_status_information(path_to_repo_Irdium_data + folder_test + example_file, verbose=2)
