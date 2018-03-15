import numpy as np
from compute_statistics import WaveStatistics
import os


# synthetic_signal = False
#
# if synthetic_signal:
#
#    ################################################################################
#    # parameters to generate the synthetic signal
#    frequency = 0.15
#    amplitude = 0.4
#    number_of_points = 25 * 60 * global_fs
#    T_sampling = 1 / global_fs
#    omega = 2 * np.pi *frequency
#    g = 9.81
#    wavenumber = omega**2 / g
#    ################################################################################
#
#    # generate the synthetic signal
#    time_basis = T_sampling * np.arange(0, number_of_points)
#    elevation = amplitude * np.sin(omega * time_basis)
#    acceleration_Z = -omega*omega*elevation
#    pitch = amplitude*wavenumber*np.cos(omega * time_basis)
#    roll = np.zeros_like(pitch)
#
#    acceleration = np.zeros((acceleration_Z.shape[0], 3))
#    acceleration[:, 0] = pitch
#    acceleration[:, 1] = roll
#    acceleration[:, 2] = acceleration_Z
#
#    fileDir = '/home/graigorys/Data/WOICE/pi_logger/ResultAnalyzis/'
#    inFile = 'test_synthetic.csv'
#    filename = fileDir + inFile
#    np.savetxt(filename, acceleration, delimiter=',')
#
# else:
#
#    # test real data
#    fileDir = '/home/graigorys/Data/WOICE/pi_logger/ResultAnalyzis/'
#    inFile = 'test_real.csv'

home = os.path.expanduser("~")

use_old_data = False

if use_old_data :

    dataDir = home + '/Data/WOICE/SvalbardPolarsyssel2017/Data_IMU_20170424_parsed/1/'
    inDir = home + '/Data/WOICE/pi_logger/ResultAnalyzis/'
    f1 = 470
    f2 = f1 + 1
    file1 = 'F{:05d}_B'.format(f1)
    file2 = 'F{:05d}_B'.format(f2)
    inFile = 'test_{0:05d}_{1:05d}.csv'.format(f1, f2)

    with open(inDir + inFile, 'w') as fo:
        with open(dataDir + file1, 'r') as f1:
            fo.write(f1.read())
        with open(dataDir + file2, 'r') as f2:
            f2.readline()
            fo.write(f2.read())
        
else :

    inDir = home + '/Data/WOICE/pi_logger/'
    inFile = 'F00205_B'

# process
instance_compute_statistics = WaveStatistics(path_in=inDir, filename=inFile, verbose=5)
instance_compute_statistics.perform_all_processing()
instance_compute_statistics.writeData()
# instance_compute_statistics.save_all_results()
