import numpy as np
from printind.printind_function import printi
from scipy import integrate
from scipy.signal import butter, lfilter
from scipy import signal
from scipy.interpolate import interp1d
from matplotlib.mlab import find
import matplotlib.pyplot as plt
import sys
import os
import struct

"""
NOTES / TODO
- rather transmit the 'locally integrated' spectrum than the under sampled spectrum!!
- averaging of the IMU reading (to do on the Mega side)
- save all to send as bytes
"""

################################################################################
# some global parameters #######################################################

# parameters for the band pass filtering

global_fs = 10.0
global_lowcut = 0.05
global_highcut = 0.25

global_noise_acc = (0.14 * 9.81 * 1e-3)**2

# parameters for the part of the acceleration signal to use
# avoid beginning and end of signal, fixed length in FFTs and Welch so
# that no need to transmit the frequence points by Iridium
# lets use 1024 s ~ 17 minutes
IMU_nfft = 2**13
IMU_window = "hanning"
IMU_nperseg = IMU_nfft / 8.0
IMU_noverlap = IMU_nperseg / 2.0
IMU_detrend = "constant"

# parameters for time

IMU_buffer = int(120 * global_fs)  # buffer for beginning to end
number_of_points_to_use = int(global_fs * 20 * 60)  # 17 minutes
first_IMU_point_to_use = int(global_fs + 1)
last_IMU_point_to_use = int(first_IMU_point_to_use + number_of_points_to_use + 2 * IMU_buffer)
# so the points to use are [first_IMU_point_to_use:last_IMU_point_to_use]


# under sampling of the Fourier spectra
global_downsample_length = 25
dfreq = (global_highcut - global_lowcut) / global_downsample_length

# global_nbit resolution for spectra
global_nbit = 16
################################################################################

# some basic functions


def DownSampleNbit(freq_all, sig_all, freq_red, downsample=global_downsample_length, nbit=global_nbit):
    """A function to downsample a signal and convert to 16-bit integer"""

    # make a dictionary for bit conversion
    nbit_convert = {
        8: np.int8,
        16: np.int16,
        32: np.int32,
        64: np.int64,
    }
    # check if nbit exists in nbit_convert and exit if it does not
    if nbit not in nbit_convert:
        sys.exit('Can not convert to {} bit integer'.format(nbit))

    # for a signed nbit integer so make it plus/minus 2**(nbit-1) -1
    max_val = 2**(nbit - 1) - 1
    #sig_red1, freq_red1 = signal.resample(sig_all, int(downsample * 1.3), t=freq_all)
    #f_int = interp1d(freq_red1, sig_red1, kind='slinear')
    ### Don't use resample but use a linear interpolation instead
    f_int = interp1d(freq_all, sig_all, kind='slinear')
    sig_red_int = f_int(freq_red)
    sig_red_max = np.max([sig_red_int.max(), -sig_red_int.min()])

    sig_red_16bit = nbit_convert[nbit](max_val * sig_red_int / sig_red_max)
    return sig_red_16bit, sig_red_max

def nan_helper(y):
    """Helper to handle indices and logical indices of NaNs.

    Input:
        - y, 1d numpy array with possible NaNs
    Output:
        - nans, logical indices of NaNs
        - index, a function, with signature indices= index(logical_indices),
          to convert logical indices of NaNs to 'equivalent' indices
    Example:
        >>> # linear interpolation of NaNs
        >>> nans, x= nan_helper(y)
        >>> y[nans]= np.interp(x(nans), x(~nans), y[~nans])
    """

    return np.isnan(y), lambda z: z.nonzero()[0]

class BandPass(object):
    """A class to perform bandpass filtering using Butter filter."""

    def __init__(self, lowcut=global_lowcut, highcut=global_highcut, fs=global_fs, order=2):
        """lowcut, highcut and fs are in Hz."""
        nyq = 0.5 * fs
        low = lowcut / nyq
        high = highcut / nyq
        self.b, self.a = butter(order, [low, high], btype='band')

    def filter_data(self, data):
        """filter the data."""

        result = lfilter(self.b, self.a, data)

        return(result)


class WaveStatistics(object):
    """A class to compute statistics about the waves for the LSM9DS0."""

    def __init__(self, path_in, filename, verbose=0, fs=global_fs):
        self.path_in = path_in
        self.filename = filename
        self.verbose = verbose
        self.fs = fs

    def perform_all_processing(self):
        self.load_VN100_data()
        # self.load_test_data()
        self.compute_vertical_elevation()
        self.compute_subsample()
        self.compute_SWH()
        self.compute_zerocrossing()
        self.compute_directional_spectrum()
        self.find_valid_index_PSD_WS()
        self.compute_wave_spectrum_moments()
        self.compute_spectral_properties()
        self.reduce_wave_spectrum()

    def load_test_data(self):
        """ load in synthetic data"""

        self.data_I = np.genfromtxt(self.path_in + '/' + self.filename, delimiter=',')

        self.PITCH = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 0]
        self.ROLL = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 1]
        self.ACCZ = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 2]

        self.PITCH_mean = np.mean(self.PITCH)
        self.ROLL_mean = np.mean(self.ROLL)
        self.ACCZ_mean = np.mean(self.ACCZ)
        self.PITCH_std = np.std(self.PITCH)
        self.ROLL_std = np.std(self.ROLL)
        self.ACCZ_std = np.std(self.ACCZ)

        self.pitch_det = signal.detrend(self.PITCH)
        self.roll_det = signal.detrend(self.ROLL)
        self.accz_det = signal.detrend(self.ACCZ)

        if self.verbose > 3:
            printi('Time duration of record is {} minutes'.format(len(self.ACCZ) / self.fs / 60))
            printi('Mean PITCH = {}'.format(self.PITCH_mean))
            printi('Mean ROLL = {}'.format(self.ROLL_mean))
            printi('Mean ACCZ = {}'.format(self.ACCZ_mean))
            printi('Std PITCH = {}'.format(self.PITCH_std))
            printi('Std ROLL = {}'.format(self.ROLL_std))
            printi('Std ACCZ = {}'.format(self.ACCZ_std))

    def load_VN100_data(self):
        """Load in VN100 IMU data.
        Data is ordered in each line as"
        MagX, MagY, MagZ, AccX, AccY, AccZ, GyroX, GyroY, GyroZ,
        Temp, Pres, Yaw, Pitch, Roll,
        DCM1, DCM2, DCM3, DCM4, DCM5, DCM6, DCM7, DCM8, DCM9,
        MagNED1, MagNED2, MagNED3, AccNED1, AccNED2, ACCNED3"""

        # declare a few global variables that might need to be changed in this function
        global last_IMU_point_to_use
        global num_points_to_use

        # read line by line and detect errors
        bad_rec = 0
        good_rec = 0
        pitch = np.NaN * np.ones([int(last_IMU_point_to_use + first_IMU_point_to_use),])
        roll = np.NaN * np.ones_like(pitch)
        accz = np.NaN * np.ones_like(pitch)
        if self.verbose > 3:
            printi('last IMU index is {}'.format(last_IMU_point_to_use))

        # open file
        with open(self.path_in + '/' + self.filename, 'r') as f:
            f.readline() # read header
            ind = 0
            for line in f:
                dat = np.fromstring(line, sep=",")
                if len(dat) == 30:
                    pitch[ind] = dat[12]
                    roll[ind] = dat[13]
                    accz[ind] = dat[28]
                    ind = ind + 1
                    good_rec = good_rec + 1
                else:
                    pitch[ind] = np.NaN
                    roll[ind] = np.NaN
                    accz[ind] = np.NaN
                    ind = ind + 1
                    bad_rec = bad_rec + 1
                if ind >= last_IMU_point_to_use + first_IMU_point_to_use - 1:
                    if self.verbose > 3 :
                        printi("index is {}".format(ind))
                    break
        if self.verbose > 3:
            printi('Total number of records are {}'.format(ind))
            printi('Number of good records are {}'.format(good_rec))
            printi('Number of bad records are {}'.format(bad_rec))

        # check that there are enough records
        if ind < last_IMU_point_to_use + first_IMU_point_to_use - 1 :
            if self.verbose > 3:
                printi('Only {} records but require {}. Changing param length'.
                        format(ind,last_IMU_point_to_use + first_IMU_point_to_use))
            # change the parameters to fit this new length
            last_IMU_point_to_use = int(ind)
            num_points_to_use = last_IMU_point_to_use - 2*IMU_buffer - first_IMU_point_to_use
            if self.verbose > 3:
                printi('last IMU index changed to {}'.format(last_IMU_point_to_use))
                printi('Calculating spectrum from {} minutes'.format(num_points_to_use/(global_fs*60)))


        # if NaNs exist then linearly interpolate
        if bad_rec > 0 :
            nans, x = nan_helper(pitch)
            pitch[nans] = np.interp(x(nans), x(~nans), pitch[~nans])
            roll[nans] = np.interp(x(nans), x(~nans), roll[~nans])
            accz[nans] = np.interp(x(nans), x(~nans), accz[~nans])
            
        self.PITCH = np.deg2rad(pitch[first_IMU_point_to_use:last_IMU_point_to_use])
        self.ROLL = np.deg2rad(roll[first_IMU_point_to_use:last_IMU_point_to_use])
        self.ACCZ = accz[first_IMU_point_to_use:last_IMU_point_to_use]

        # simple detrend - remove mean
        self.pitch_det = signal.detrend(self.PITCH, type=IMU_detrend)
        self.roll_det = signal.detrend(self.ROLL, type=IMU_detrend)
        self.accz_det = signal.detrend(self.ACCZ, type=IMU_detrend)

        ## # apply filter
        ## butter = BandPass(fs=self.fs, lowcut=0.04, highcut=0.5)
        ## self.pitch_det = butter.filter_data(self.PITCH)
        ## self.roll_det = butter.filter_data(self.ROLL)
        ## self.accz_det = butter.filter_data(self.ACCZ)

        if self.verbose > 3:
            printi('Mean PITCH = {}'.format(np.mean(self.PITCH)))
            printi('Mean ROLL = {}'.format(np.mean(self.ROLL)))
            printi('Mean ACCZ = {}'.format(np.mean(self.ACCZ)))
            printi('Std PITCH = {}'.format(np.std(self.PITCH)))
            printi('Std ROLL = {}'.format(np.std(self.ROLL)))
            printi('Std ACCZ = {}'.format(np.std(self.ACCZ)))
            plt.figure()
            plt.plot(self.ACCZ, label='ACCZ')
            plt.plot(self.accz_det, label='ACCZ_det')
            plt.legend()
            plt.figure()
            plt.plot(self.PITCH, label='PITCH')
            plt.plot(self.ROLL, label='ROLL')
            plt.plot(self.pitch_det, label='PITCH_det')
            plt.plot(self.roll_det, label='ROLL_det')
            plt.legend()

    def compute_vertical_elevation(self):
        '''integrate twice using fft and ifft'''

        # calculate fft, filter, and then ifft to get heights

        # suppress divide by 0 warning
        np.seterr(divide='ignore')

        Y = np.fft.fft(self.accz_det)

        # calculate weights before applying ifft
        freq = np.fft.fftfreq(self.accz_det.size, d=1.0 / global_fs)
        weights = -1.0 / ((2 * np.pi * freq)**2)
        # need to do some filtering for low frequency (from Kohout)
        f1 = 0.03
        f2 = 0.04
        Yf = np.zeros_like(Y)
        ind = np.argwhere(np.logical_and(freq >= f1, freq <= f2))
        Yf[ind] = Y[ind] * 0.5 * (1 - np.cos(np.pi * (freq[ind] - f1) / (f2 - f1))) * weights[ind]
        Yf[freq > f2] = Y[freq > f2] * weights[freq > f2]

        # apply ifft to get height
        self.elev = -np.real(np.fft.ifft(2 * Yf))

    def compute_subsample(self):
        """delete first and last IMU_buffer lengths"""

        self.pitch_proc = self.pitch_det[IMU_buffer:-IMU_buffer]
        self.roll_proc = self.roll_det[IMU_buffer:-IMU_buffer]
        self.elev_proc = self.elev[IMU_buffer:-IMU_buffer]
        self.accz_proc = self.accz_det[IMU_buffer:-IMU_buffer]

        if self.verbose > 3:
            printi("self.elev = " + str(self.elev_proc))
            printi("max elev: " + str(np.max(self.elev_proc)))
            printi("min elev: " + str(np.min(self.elev_proc)))

    def compute_SWH(self):
        """Compute SWH using double integration of vertical acceleration."""

        # SWH
        self.SWH = 4.0 * np.std(self.elev_proc)

        if self.verbose > 1:
            printi("SWH = " + str(self.SWH))

    def compute_zerocrossing(self):
        """Compute zero-crossing from time series"""

        # estimate zero crossing from timeseries
        indices = find((self.elev_proc[1:] >= 0) & (self.elev_proc[:-1] < 0))

        # More accurate, using linear interpolation to find intersample
        # zero-crossings
        crossings = [i - self.elev_proc[i] / (self.elev_proc[i + 1] - self.elev_proc[i]) for i in indices]
        # calculate mean period by averaging indices over total indices / divided by fs
        self.T_z0 = np.mean(np.diff(crossings) / self.fs)

        if self.verbose > 1:
            printi('T_z0 = {}'.format(self.T_z0))

    def compute_directional_spectrum(self):
        """Calculate directional spectrum moments for direction and spread"""

        # suppress divide by 0 warning
        np.seterr(divide='ignore', invalid='ignore')

        f11, p11 = signal.csd(self.elev_proc, self.elev_proc, fs=self.fs, nperseg=IMU_nperseg,
                              noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p22 = signal.csd(self.pitch_proc, self.pitch_proc, fs=self.fs, nperseg=IMU_nperseg,
                            noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p33 = signal.csd(self.roll_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg,
                            noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p12 = signal.csd(self.elev_proc, self.pitch_proc, fs=self.fs, nperseg=IMU_nperseg,
                            noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p13 = signal.csd(self.elev_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg,
                            noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p23 = signal.csd(self.pitch_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg,
                            noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)

        # calculate omega, k, and k0
        omega = 2 * np.pi * f11
        g = 9.81
        k0 = omega**2 / g
        self.k = np.sqrt((p22 + p33) / p11)
        self.R = self.k / k0

        # now calculate circular moments
        self.freq = f11
        self.a0 = p11
        self.a1 = -np.imag(p12) / self.k
        self.b1 = -np.imag(p13) / self.k
        self.a2 = (p22 - p33) / (self.k**2)
        self.b2 = 2 * np.real(p23) / (self.k**2)

    def find_valid_index_PSD_WS(self):
        """Find the limiting index that are within global_lowcut global_highcut."""

        index = np.argwhere(np.logical_and(self.freq >= global_lowcut - dfreq, self.freq <= global_highcut + dfreq))

        # apply limits to values
        self.freq = np.squeeze(self.freq[index])
        self.a0 = np.squeeze(self.a0[index])
        self.a1 = np.squeeze(self.a1[index])
        self.b1 = np.squeeze(self.b1[index])
        self.a2 = np.squeeze(self.a2[index])
        self.b2 = np.squeeze(self.b2[index])
        self.R = np.squeeze(self.R[index])

        # make sure self.a0 is positive
        noise = global_noise_acc * ((2 * np.pi * self.freq)**(-4))
        self.a0[self.a0 < noise] = noise[self.a0 < noise]

    def compute_wave_spectrum_moments(self):
        """Compute the moments of the wave spectrum."""

        omega = 2 * np.pi * self.freq

        # note: integrate only on the 'valid' part of the spectrum

        self.M0 = integrate.trapz(self.a0, x=omega)
        self.M1 = integrate.trapz(self.a0 * (omega), x=omega)
        self.M2 = integrate.trapz(self.a0 * (omega**2), x=omega)
        self.M3 = integrate.trapz(self.a0 * (omega**3), x=omega)
        self.M4 = integrate.trapz(self.a0 * (omega**4), x=omega)
        self.MM1 = integrate.trapz(self.a0 * (omega**(-1)), x=omega)
        self.MM2 = integrate.trapz(self.a0 * (omega**(-2)), x=omega)

        if self.verbose > 1:
            printi('min, max of freq is {}, {}'.format(self.freq.min(), self.freq.max()))
            printi('f shape is {}'.format(self.freq.shape))

    def compute_spectral_properties(self):
        """Compute SWH and the peak period, both zero up-crossing and peak-to-peak,
        from spectral moments."""

        self.Hs = np.sqrt(self.M0) * 4.0 / np.sqrt(2 * np.pi)
        self.T_z = 2.0 * np.pi * np.sqrt(self.M0 / self.M2)
        self.T_c = 2.0 * np.pi * np.sqrt(self.M2 / self.M4)
        self.T_p = 1.0 / self.freq[np.argmax(self.a0)]

        if self.verbose > 2:
            printi('Hs (from M0) = {}'.format(self.Hs))
            printi('T_z = {}'.format(self.T_z))
            printi('T_c = {}'.format(self.T_c))
            printi('T_p = {}'.format(self.T_p))

    def reduce_wave_spectrum(self):
        """A reduced wave spectrum, so that easier to transmit (less information).
        Reduction in information is obtained by restraining the frequency domain,
        reducing to 16 bits per frequency"""

        # reduce spectra with resample
        # pwr = -1
        # t0 = np.linspace(global_highcut**pwr, global_lowcut**pwr, global_downsample_length+1)
        # f0 = np.sort(t0**pwr)
        f1 = np.log(global_lowcut)
        f2 = np.log(global_highcut)
        self.freq_reduc = np.exp(np.linspace(f1, f2, global_downsample_length))
        # f0 = np.linspace(global_lowcut, global_highcut, global_downsample_length+1)
        # self.freq_reduc = 0.5 * (f0[0:-1] + f0[1:])

        self.a0_reduc, self.a0_reduc_max = DownSampleNbit(self.freq, self.a0,
                                                          self.freq_reduc)
        self.a1_reduc, self.a1_reduc_max = DownSampleNbit(self.freq, self.a1,
                                                          self.freq_reduc)
        self.b1_reduc, self.b1_reduc_max = DownSampleNbit(self.freq, self.b1,
                                                          self.freq_reduc)
        self.a2_reduc, self.a2_reduc_max = DownSampleNbit(self.freq, self.a2,
                                                          self.freq_reduc)
        self.b2_reduc, self.b2_reduc_max = DownSampleNbit(self.freq, self.b2,
                                                          self.freq_reduc)
        self.R_reduc, self.R_reduc_max = DownSampleNbit(self.freq, self.R,
                                                        self.freq_reduc)

        # normalize by max value and change to int16
        norm_val = 2**(global_nbit - 1) - 1

        M0_reduc = integrate.trapz(self.a0_reduc * self.a0_reduc_max / norm_val, x=2 * np.pi * self.freq_reduc)
        Hs_reduc = np.sqrt(M0_reduc) * 4.0 / np.sqrt(2 * np.pi)

        if self.verbose > 3:
            printi('limited_frequencies_frequencies = {}'.format(self.freq_reduc))
            printi('limited periods = {}'.format(1 / self.freq_reduc))
            printi("array_discretized_spectrum = " + str(self.a0_reduc))
            printi("max a0 reduced = " + str(self.a0_reduc_max))
            printi("max a1 reduced = " + str(self.a1_reduc_max))
            printi("max a2 reduced = " + str(self.a2_reduc_max))
            printi("max b1 reduced = " + str(self.b1_reduc_max))
            printi("max b2 reduced = " + str(self.b2_reduc_max))
            printi('Hs reduced = {}'.format(Hs_reduc))

        if self.verbose > 3:
            plt.figure()
            plt.plot(self.freq, self.a0)
            plt.plot(self.freq_reduc, self.a0_reduc * self.a0_reduc_max / norm_val, '-ro')
            plt.plot(self.freq, global_noise_acc * ((2 * np.pi * self.freq)**(-4)), 'k--')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('log')
            plt.ylabel('a0')

            plt.figure()
            plt.plot(self.freq, self.a1)
            plt.plot(self.freq_reduc, self.a1_reduc * self.a1_reduc_max / norm_val, '-ro')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('linear')
            plt.ylabel('a1')

            plt.figure()
            plt.plot(self.freq, self.b1)
            plt.plot(self.freq_reduc, self.b1_reduc * self.b1_reduc_max / norm_val, '-ro')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('linear')
            plt.ylabel('b1')

            plt.figure()
            plt.plot(self.freq, self.a2)
            plt.plot(self.freq_reduc, self.a2_reduc * self.a2_reduc_max / norm_val, '-ro')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('linear')
            plt.ylabel('a2')

            plt.figure()
            plt.plot(self.freq, self.b2)
            plt.plot(self.freq_reduc, self.b2_reduc * self.b2_reduc_max / norm_val, '-ro')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('linear')
            plt.ylabel('b2')

            plt.figure()
            plt.plot(self.freq, self.R)
            plt.plot(self.freq_reduc, (self.R_reduc * self.R_reduc_max / norm_val), '-ro')
            plt.xlim([global_lowcut, global_highcut])
            plt.yscale('linear')
            plt.ylabel('R')

            plt.show()

    def writeData(self, path_output=None):
        '''write the data to a data structure file'''

        if path_output is None:
            basename = os.path.splitext(self.filename)[0]
            total_path = self.path_in + '/' + basename[:-2] + '.bin'
        else:
            total_path = path_output

        if self.verbose > 0:
            print("save binary data as {}".format(total_path))

        with open(total_path, 'wb') as f:
            fmt_hdr = '<' + 'f' * 10
            f.write(struct.pack(fmt_hdr, self.SWH, self.T_z0, self.Hs, self.T_z,
                                self.a0_reduc_max, self.a1_reduc_max, self.b1_reduc_max,
                                self.a2_reduc_max, self.b2_reduc_max, self.R_reduc_max))
            # create a new format
            # now I need to write arrys
            fmt_array = '<' + 'h' * global_downsample_length
            f.write(struct.pack(fmt_array, *self.a0_reduc))
            f.write(struct.pack(fmt_array, *self.a1_reduc))
            f.write(struct.pack(fmt_array, *self.b1_reduc))
            f.write(struct.pack(fmt_array, *self.a2_reduc))
            f.write(struct.pack(fmt_array, *self.b2_reduc))
            f.write(struct.pack(fmt_array, *self.R_reduc))
