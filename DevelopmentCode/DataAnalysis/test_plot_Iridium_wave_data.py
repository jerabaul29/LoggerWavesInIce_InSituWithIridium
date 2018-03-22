from load_Iridium_wave_data import load_data
from plot_Iridium_wave_data import plot_data

example_file = "/home/jrlab/Desktop/Git/IridiumData_Svalbard2018/300234066333770/Thu_22_Mar_2018_13:36:52_+0000.bin"

dict_data = load_data(example_file)

plot_data(dict_data)
