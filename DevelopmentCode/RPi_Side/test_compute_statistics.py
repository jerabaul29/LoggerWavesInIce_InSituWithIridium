import numpy as np
from compute_statistics import WaveStatistics
from compute_statistics import global_fs


#synthetic_signal = False
#
#if synthetic_signal:
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
#else:
#    
#    # test real data
#    fileDir = '/home/graigorys/Data/WOICE/pi_logger/ResultAnalyzis/'
#    inFile = 'test_real.csv'

#inDir = '/Users/gsutherland/Data/WOICE/LanceCruise/'
#inDir = '/home/graigorys/Data/WOICE/pi_logger/LanceTest/'
#inFile = 'S1_20160502T023802.txt'
# test real data
dataDir = '/home/graigorys/Data/WOICE/SvalbardPolarsyssel2017/Data_IMU_20170424_parsed/1/'
inDir = '/home/graigorys/Data/WOICE/pi_logger/ResultAnalyzis/'
f1 = 654
f2 = f1+1
file1 = 'F{:05d}_B'.format(f1)
file2 = 'F{:05d}_B'.format(f2)
inFile = 'test_{0:05d}_{1:05d}.csv'.format(f1,f2)

data1 = np.genfromtxt(dataDir+file1, skip_header=1, delimiter=',')
data2 = np.genfromtxt(dataDir+file2, skip_header=1, delimiter=',')
data_all = np.concatenate( (data1, data2), axis=0)
# read in header
with open(dataDir+file1) as f:
    hdr = f.readline()

#write to text file
np.savetxt(inDir+inFile, data_all, fmt='%.6e', delimiter=',', header=hdr)

# process
instance_compute_statistics = WaveStatistics(path_in=inDir, filename=inFile, verbose=5)
instance_compute_statistics.perform_all_processing()
instance_compute_statistics.writeData()
#instance_compute_statistics.save_all_results()
