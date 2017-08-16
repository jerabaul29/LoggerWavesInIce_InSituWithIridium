import numpy as np
from compute_statistics import *

################################################################################
# parameters to generate the synthetic signal
frequency = 0.15
amplitude = 0.4
number_of_points = 25 * 60 * global_fs
T_sampling = 1 / global_fs
################################################################################

# generate the synthetic signal
time_basis = T_sampling * np.arange(0, number_of_points)
elevation = amplitude * np.sin(2 * np.pi * frequency * time_basis)
velocity = (elevation[1:] - elevation[:-1]) / T_sampling
acceleration_Z = (velocity[1:] - velocity[:-1]) / T_sampling + 9.81
acceleration_X = np.zeros(acceleration_Z.shape)
acceleration_Y = np.zeros(acceleration_Z.shape)

acceleration = np.zeros((acceleration_X.shape[0], 3))
acceleration[:, 0] = acceleration_X
acceleration[:, 1] = acceleration_Y
acceleration[:, 2] = acceleration_Z

# save the synthetic signal
filename = '/media/jrlab/673b8ab6-6426-474b-87d3-71bff0fcebc3/home/pi/Logger/ResultAnalyzis/TEST_COMPUTE_STATISTICS.CSV'
np.savetxt(filename, acceleration, delimiter=';')

# process
instance_compute_statistics = WaveStatistics(path_in='/media/jrlab/673b8ab6-6426-474b-87d3-71bff0fcebc3/home/pi/Logger/ResultAnalyzis/', filename='TEST_COMPUTE_STATISTICS.CSV', verbose=5)
instance_compute_statistics.perform_all_processing()
instance_compute_statistics.save_all_results()
