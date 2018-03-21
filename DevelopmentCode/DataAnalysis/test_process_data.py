import process_data

path_to_repo = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = '300234066333770'

data_manager = process_data.DataManager(path_to_repo=path_to_repo, verbose=5)

data_manager.bundle_data_folder(folder_test)
