import numpy as np
from printind.printind_function import printi
from scipy import integrate
from scipy.signal import butter, lfilter
from scipy import signal
from scipy.interpolate import interp1d

"""
NOTES / TODO
- rather transmit the 'locally integrated' spectrum than the under sampled spectrum!!
- averaging of the IMU reading (to do on the Mega side)
- save all to send as bytes
"""

################################################################################
# some global parameters #######################################################
plot_diag = True

if plot_diag:
    import matplotlib.pyplot as plt

# parameters for the band pass filtering
global_fs = 10.0
global_lowcut = 0.05
global_highcut = 0.35

global_noise_acc=(0.14*9.81*1e-3)**2

# parameters for the part of the acceleration signal to use
# avoid beginning and end of signal, fixed length in FFTs and Welch so
# that no need to transmit the frequence points by Iridium
# lets use 1024 s ~ 17 minutes
IMU_nfft = 2**13
IMU_window = "hanning"
IMU_nperseg = IMU_nfft/8.0
IMU_noverlap = IMU_nperseg/2.0
IMU_detrend = "linear"

# parameters for time
IMU_buffer = int(120*global_fs) # buffer for beginning to end
number_of_points_to_use = int(global_fs * 20 * 60)  # 17 minutes
first_IMU_point_to_use = int(global_fs + 1)
last_IMU_point_to_use = int(first_IMU_point_to_use + number_of_points_to_use + 2*IMU_buffer)
# so the points to use are [first_IMU_point_to_use:last_IMU_point_to_use]


# under sampling of the Fourier spectra
global_downsample_length = 30
################################################################################


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
	#self.load_test_data()
        self.compute_vertical_elevation()
        self.compute_subsample()
        self.compute_SWH()
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
            printi('Time duration of record is {} minutes'.format(len(self.ACCZ)/self.fs/60))
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

        self.data_I = np.genfromtxt(self.path_in + '/' + self.filename, skip_header=1, delimiter=',')

        if self.verbose > 3:
            printi('Shape of input data is {}'.format(self.data_I.shape))
            printi('Time duration of record is {} minutes'.format(len(self.data_I)/self.fs/60))

        self.PITCH = np.deg2rad(self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 12])
        self.ROLL = np.deg2rad(self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 13])
        self.YAW = np.deg2rad(self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 11])
        self.ACCZ = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 28]

        self.pitch_det = signal.detrend(self.PITCH)
        self.roll_det = signal.detrend(self.ROLL)
        self.accz_det = signal.detrend(self.ACCZ)

        # apply filter
        ## butter = BandPass(fs=self.fs)
        ## self.pitch_det = bp.filter_data(self.pitch_det)
        ## self.roll_det = bp.filter_data(self.roll_det)
        ## self.accz_det = bp.filter_data(self.accz_det)

        if self.verbose > 3:
            printi('Mean PITCH = {}'.format(np.mean(self.PITCH)))
            printi('Mean ROLL = {}'.format(np.mean(self.ROLL)))
            printi('Mean YAW = {}'.format(np.mean(self.YAW)))
            printi('Mean ACCZ = {}'.format(np.mean(self.ACCZ)))
            printi('Std PITCH = {}'.format(np.std(self.PITCH)))
            printi('Std ROLL = {}'.format(np.std(self.ROLL)))
            printi('Std YAW = {}'.format(np.std(self.YAW)))
            printi('Std ACCZ = {}'.format(np.std(self.ACCZ)))


    def compute_vertical_elevation(self):
        '''integrate twice using fft and ifft'''

	# calculate fft, filter, and then ifft to get heights

        # suppress divide by 0 warning
        np.seterr(divide='ignore')
        
        Y = np.fft.fft(self.accz_det)

	# calculate weights before applying ifft
        freq = np.fft.fftfreq(self.accz_det.size, d=1.0/global_fs)
        weights = -1.0/( (2*np.pi*freq)**2 )
    # need to do some filtering for low frequency (from Kohout)
        f1 = 0.02
        f2 = 0.03
        Yf = np.zeros_like(Y)
        ind = np.argwhere(np.logical_and(freq>=f1,freq<=f2))
        Yf[ind] = Y[ind] * 0.5*(1 - np.cos(np.pi*(freq[ind]-f1)/(f2-f1)))*weights[ind]
        Yf[freq>f2] = Y[freq>f2]*weights[freq>f2]

	
	# apply ifft to get height
        self.elev = -np.real(np.fft.ifft(2*Yf))


    def compute_subsample(self):
        """delete first and last IMU_buffer lengths"""

        print self.pitch_det.shape
        print IMU_buffer

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

    def compute_directional_spectrum(self):
        """Calculate directional spectrum moments for direction and spread"""

        # suppress divide by 0 warning
        np.seterr(divide='ignore', invalid='ignore')

        f11, p11 = signal.csd(self.elev_proc, self.elev_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p22 = signal.csd(self.pitch_proc, self.pitch_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p33 = signal.csd(self.roll_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p12 = signal.csd(self.elev_proc, self.pitch_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p13 = signal.csd(self.elev_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)
        _, p23 = signal.csd(self.pitch_proc, self.roll_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft, return_onesided=True)

        # calculate omega, k, and k0
        omega = 2*np.pi*f11
        g = 9.81
        k0 = omega**2 / g
        k = np.sqrt( (p22 + p33) / p11 )

        # now calculate circular moments
	self.freq = f11
        self.a0 = p11
        self.a1 = -np.imag(p12) / k
        self.b1 = -np.imag(p13) / k 
        self.a2 = (p22 - p33) / (k**2)
        self.b2 = 2*np.real(p23) / (k**2)

    def find_valid_index_PSD_WS(self):
        """Find the limiting index that are within global_lowcut global_highcut."""
        
	index = np.argwhere(np.logical_and(self.freq>=global_lowcut,self.freq<=global_highcut))

        # apply limits to values
	self.freq = np.squeeze(self.freq[index])
	self.a0 = np.squeeze(self.a0[index])
	self.a1 = np.squeeze(self.a1[index])
	self.b1 = np.squeeze(self.b1[index])
	self.a2 = np.squeeze(self.a2[index])
	self.b2 = np.squeeze(self.b2[index])

	# not have negative values of self.a0
	self.a0[self.a0<0] = 0.0

    def compute_wave_spectrum_moments(self):
        """Compute the moments of the wave spectrum."""

        omega = 2 * np.pi * self.freq

	print 'min, max of freq is {}, {}'.format(self.freq.min(),self.freq.max())
	print 'a0 shape is {}'.format(self.a0.shape)
	print 'f shape is {}'.format(self.freq.shape)

        # note: integrate only on the 'valid' part of the spectrum
	
        self.M0 = integrate.trapz(self.a0, x=omega)
        self.M1 = integrate.trapz(self.a0 * (omega), x=omega)
        self.M2 = integrate.trapz(self.a0 * (omega**2), x=omega)
        self.M3 = integrate.trapz(self.a0 * (omega**3), x=omega)
        self.M4 = integrate.trapz(self.a0 * (omega**4), x=omega)
        self.MM1 = integrate.trapz(self.a0 * (omega**(-1)), x=omega)
        self.MM2 = integrate.trapz(self.a0 * (omega**(-2)), x=omega)


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
        reducing to 8 bits per frequency"""

	# reduce spectra with resample

	f0 = np.linspace(global_lowcut, global_highcut, global_downsample_length+1)
	self.freq_reduc = 0.5 * (f0[0:-1] + f0[1:])
	#fit_type='slinear'
	#fa0 = interp1d(self.freq, self.a0, kind=fit_type)
	#fa1 = interp1d(self.freq, self.a1, kind=fit_type)
	#fb1 = interp1d(self.freq, self.b1, kind=fit_type)
	#fa2 = interp1d(self.freq, self.a2, kind=fit_type)
	#fb2 = interp1d(self.freq, self.b2, kind=fit_type)
	#self.a0_reduc = fa0(self.freq_reduc)
	#self.a1_reduc = fa1(self.freq_reduc)
	#self.b1_reduc = fb1(self.freq_reduc)
	#self.a2_reduc = fa2(self.freq_reduc)
	#self.b2_reduc = fb2(self.freq_reduc)
	
	self.a0_reduc, self.freq_reduc = signal.resample(self.a0, global_downsample_length, t=self.freq)
	self.a1_reduc, _ = signal.resample(self.a1, global_downsample_length, t=self.freq)
	self.b1_reduc, _ = signal.resample(self.b1, global_downsample_length, t=self.freq)
	self.a2_reduc, _ = signal.resample(self.a2, global_downsample_length, t=self.freq)
	self.b2_reduc, _ = signal.resample(self.b2, global_downsample_length, t=self.freq)

	# calculate maximum
	self.a0_reduc_max = np.max([self.a0_reduc.max(), -self.a0_reduc.min()])
	self.a1_reduc_max = np.max([self.a1_reduc.max(), -self.a1_reduc.min()])
	self.b1_reduc_max = np.max([self.b1_reduc.max(), -self.b1_reduc.min()])
	self.a2_reduc_max = np.max([self.a2_reduc.max(), -self.a2_reduc.min()])
	self.b2_reduc_max = np.max([self.b2_reduc.max(), -self.b2_reduc.min()])
	
	# normalize by max value and change to int16
	self.a0_reduc = self.a0_reduc / self.a0_reduc_max
	self.a0_reduc = np.int16((2**15-1) * self.a0_reduc)
	self.a1_reduc = self.a1_reduc / self.a1_reduc_max
	self.a1_reduc = np.int16((2**15-1) * self.a1_reduc)
	self.b1_reduc = self.b1_reduc / self.b1_reduc_max
	self.b1_reduc = np.int16((2**15-1) * self.b1_reduc)
	self.a2_reduc = self.a2_reduc / self.a2_reduc_max
	self.a2_reduc = np.int16((2**15-1) * self.a2_reduc)
	self.b2_reduc = self.b2_reduc / self.b2_reduc_max
	self.b2_reduc = np.int16((2**15-1) * self.b2_reduc)
	
        
        if self.verbose > 3:
            printi("limited_frequencies_frequencies = " + str(self.freq_reduc))
            printi("array_discretized_spectrum = " + str(self.a0_reduc))
            printi("max a0 reduced = " + str(self.a0_reduc_max))
            printi("max a1 reduced = " + str(self.a1_reduc_max))
            printi("max a2 reduced = " + str(self.a2_reduc_max))
            printi("max b1 reduced = " + str(self.b1_reduc_max))
            printi("max b2 reduced = " + str(self.b2_reduc_max))

	if plot_diag :
	    plt.figure()
	    plt.plot(self.freq, self.a0)
	    plt.plot(self.freq_reduc, self.a0_reduc * self.a0_reduc_max / (2**(15)-1), 'r.')
	    plt.xlim([global_lowcut, global_highcut])
	    plt.yscale('linear')
	    plt.ylabel('a0')
	    plt.figure()
	    plt.plot(self.freq, self.a1)
	    plt.plot(self.freq_reduc, self.a1_reduc * self.a1_reduc_max / (2**(15)-1), 'r.')
	    plt.xlim([global_lowcut, global_highcut])
	    plt.yscale('linear')
	    plt.ylabel('a1')
	    plt.figure()
	    plt.plot(self.freq, self.b1)
	    plt.plot(self.freq_reduc, self.b1_reduc * self.b1_reduc_max / (2**(15)-1), 'r.')
	    plt.xlim([global_lowcut, global_highcut])
	    plt.yscale('linear')
	    plt.ylabel('b1')
	    plt.figure()
	    plt.plot(self.freq, self.a2)
	    plt.plot(self.freq_reduc, self.a2_reduc * self.a2_reduc_max / (2**(15)-1), 'r.')
	    plt.xlim([global_lowcut, global_highcut])
	    plt.yscale('linear')
	    plt.ylabel('a2')
	    plt.figure()
	    plt.plot(self.freq, self.b2)
	    plt.plot(self.freq_reduc, self.b2_reduc * self.b2_reduc_max / (2**(15)-1), 'r.')
	    plt.xlim([global_lowcut, global_highcut])
	    plt.yscale('linear')
	    plt.ylabel('b2')
	    plt.show()

