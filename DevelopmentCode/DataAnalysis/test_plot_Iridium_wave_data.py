from load_Iridium_wave_data import load_data
from plot_Iridium_wave_data import plot_data

example_file = "/home/jrlab/Downloads/300234066333770-50.bin"

dict_data = load_data(example_file)

plot_data(dict_data)
