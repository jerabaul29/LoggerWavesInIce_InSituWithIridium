import parser_logger

path_in = '/media/jrlab/3DAE-4639/'
path_out = '/media/jrlab/3DAE-4639/'
filename = 'F00397'
verbose = 1

parser_instance = parser_logger.Parser_logger(path_in, path_out, verbose)
parser_instance.load_file(path_in + filename)
parser_instance.parse_current_data(path_out + filename)

import numpy as np

np.genfromtxt(path_out + filename + '_B', delimiter=',', skip_header=1)
