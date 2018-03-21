import interact_with_gmail

# local path on the computer to the github repo containing the binary data
path_to_repo = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'

# username and password for the gmail address from which read the Iridium messages
username = "iridium.uio.math.mech"
password = "hJ742@))+=h"

iridium_reader = interact_with_gmail.IridiumEmailReader(username, password, path_to_repo, verbose=1)
iridium_reader.read_all_incoming_messages()
iridium_reader.generate_data_all_incoming_messages()
iridium_reader.push_data()
