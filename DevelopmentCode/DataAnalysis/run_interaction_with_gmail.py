import interact_with_gmail

# local path on the computer to the github repo containing the binary data
# path_to_repo_Irdium_data = '/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/'
# YOU SHOULD USE YOUR OWN GITHUB REPO HERE
path_to_repo_Irdium_data = 

# username and password for the gmail address from which read the Iridium messages
# username = raw_input("enter gmail username: ")
# password = raw_input("enter gmail password: ")

# YOU SHOULD USE YOUR OWN GOOGLE ACCOUNT FORWARDING HERE, NOT MINE!!!
# username = "iridium.uio.math.mech"
# password = "hJ742@))+=h"
username = "universityosloiridiumdatacopy"
password = "gJJ75@))1"

iridium_reader = interact_with_gmail.IridiumEmailReader(username, password, path_to_repo_Irdium_data, verbose=5)
iridium_reader.automatic_interaction()
