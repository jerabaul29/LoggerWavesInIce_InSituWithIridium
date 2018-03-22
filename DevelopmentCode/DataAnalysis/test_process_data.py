import process_data

path_to_repo = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
folder_test = '300234066333770'

data_manager = process_data.DataManager(path_to_repo=path_to_repo, verbose=5)

data_manager.bundle_data_folder(folder_test)

import datetime

date_1 = datetime.datetime.utcnow()
date_2 = datetime.datetime.utcnow()
date_3 = datetime.datetime.utcnow()

date_1
date_2

date_1 - date_2
date_2 - date_1

abs((date_1 - date_2).total_seconds())
(date_2 - date_1).total_seconds()

str(min(date_1, date_2, date_3))

abs(1)
abs(-3.4)
