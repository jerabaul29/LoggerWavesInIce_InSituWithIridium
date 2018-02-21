import numpy as np
from printind.printind_function import printi
from scipy import integrate
from scipy.signal import butter, lfilter
from scipy import signal

"""
NOTES / TODO
- rather transmit the 'locally integrated' spectrum than the under sampled spectrum!!
- averaging of the IMU reading (to do on the Mega side)
- save all to send as bytes
"""

################################################################################
# some global parameters #######################################################
global_debug = False

plot_diag = True

if plot_diag:
    import matplotlib.pyplot as plt

# parameters for the band pass filtering
global_fs = 10.0
global_lowcut = 0.05
global_highcut = 0.3

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
global_under_sampling = 4
number_of_downsamples = 1
################################################################################


class ButterFiltering(object):
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

    def __init__(self, path_in, filename, verbose=0, fs=global_fs, debug=global_debug):
        self.path_in = path_in
        self.filename = filename
        self.verbose = verbose
        self.fs = fs
        self.debug = debug

    def perform_all_processing(self):
        self.load_VN100_data()
	#self.load_test_data()
        self.compute_vertical_elevation()
        self.compute_subsample()
        self.compute_SWH()
        self.compute_directional_spectrum()
	self.compute_elevation_spectrum()
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

        self.PITCH_mean = np.mean(self.PITCH)
        self.ROLL_mean = np.mean(self.ROLL)
        self.YAW_mean = np.mean(self.YAW)
        self.ACCZ_mean = np.mean(self.ACCZ)

        self.pitch_det = signal.detrend(self.PITCH)
        self.roll_det = signal.detrend(self.ROLL)
        self.accz_det = signal.detrend(self.ACCZ)

        # apply filter
        ## butter = ButterFiltering(fs=self.fs)
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
        ind = np.where(np.logical_and(freq>=f1,freq<=f2))
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

        if plot_diag:
            plt.figure()
            plt.plot(np.arange(len(self.elev_proc))/self.fs,self.elev_proc)
            plt.ylabel('elev')
            plt.figure()
            plt.plot(np.arange(len(self.accz_proc))/self.fs,self.accz_proc)
            plt.ylabel('accz')
            plt.figure()
            plt.plot(np.arange(len(self.pitch_proc))/self.fs,np.rad2deg(self.pitch_proc), label='pitch')
            plt.plot(np.arange(len(self.roll_proc))/self.fs,np.rad2deg(self.roll_proc), label='roll')
            plt.legend()

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
        a0 = p11 / np.pi
        a1 = -np.imag(p12) / (k * np.pi)
        b1 = -np.imag(p13) / (k * np.pi)
        a2 = (p22 - p33) / (k**2 * np.pi)
        b2 = 2*np.real(p23) / (k**2 * np.pi)
        m1 = np.sqrt ( (a1/a0)**2 + (b1/a0)**2 )
        spread = np.sqrt( 2*(1-m1) )
        theta = np.arctan(b1/a1)
        R = k/k0

        # set values in object
        self.f_PSD_WS = f11
        self.S_PSD_WS = p11
        self.spread = spread
        self.theta = theta
        self.R = R

        if plot_diag:
            weights = (2 * np.pi * f11)**(-2)
            noise_gyr = (0.0035 * np.pi / 180.0)**2
            noise = noise_gyr * weights
            plt.figure()
            plt.plot(f11, p22, label='pitch')
            plt.plot(f11, p33, label='roll')
            plt.plot(f11, noise, 'k--', label='noise')
            plt.axvline(x=global_lowcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.axvline(x=global_highcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.xlim([0.01, 0.5])
            plt.yscale('log')
            plt.legend()
            weights = (2 * np.pi * f11)**(-4)
            plt.figure()
            plt.plot(f11, p11, label='elev')
            plt.plot(f11, global_noise_acc*weights, 'k--', label='noise')
            plt.axvline(x=global_lowcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.axvline(x=global_highcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.xlim([.01, 0.5])
            plt.yscale('log')
            plt.legend()
            plt.figure()
            plt.plot(f11, np.real(p12), label='real(p12)')
            plt.plot(f11, np.real(p13), label='real(p13)')
            plt.xlim([.01, 0.5])
            plt.yscale('linear')
            plt.axvline(x=global_lowcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.axvline(x=global_highcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.legend()
            plt.figure()
            plt.plot(f11, np.imag(p12), label='imag(p12)')
            plt.plot(f11, np.imag(p13), label='imag(p13)')
            plt.xlim([.01, 0.5])
            plt.yscale('linear')
            plt.axvline(x=global_lowcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.axvline(x=global_highcut, color=[0.5,0.5,0.5], linestyle='-')
            plt.legend()



    def compute_elevation_spectrum(self):
        """Compute elevation spectrum from the double integrated vertical acceleration."""

        # Fourier spectrum using FFT
        self.S_FFT = np.fft.fft(self.elev_proc)
        self.f_FFT = np.fft.fftfreq(self.elev_proc.size, d=1/self.fs)
        self.f_PSD_h, self.S_PSD_h = signal.welch(self.elev_proc, fs=self.fs, nperseg=IMU_nperseg, \
                noverlap=IMU_noverlap, nfft=IMU_nfft)

    def find_valid_index_PSD_WS(self):
        """Find the limiting index that are within global_lowcut global_highcut."""
        
        ind1 = np.argmin(np.abs(self.f_PSD_WS - global_lowcut))
        ind2 = np.argmin(np.abs(self.f_PSD_WS - global_highcut))

        # apply limits to values
        self.f_PSD_WS = self.f_PSD_WS[ind1:ind2]
        self.S_PSD_WS = self.S_PSD_WS[ind1:ind2]
        self.spread = self.spread[ind1:ind2]
        self.theta = self.theta[ind1:ind2]
        self.R = self.R[ind1:ind2]

        # set values below noise floor to 0
        weights = (2 * np.pi * self.f_PSD_WS)**(-4)
        self.S_PSD_WS = self.S_PSD_WS - global_noise_acc*weights
        self.S_PSD_WS[self.S_PSD_WS < 0.0] = 0.0
        
        ind1 = np.argmin(np.abs(self.f_PSD_h - global_lowcut))
        ind2 = np.argmin(np.abs(self.f_PSD_h - global_highcut))
        self.f_PSD_h = self.f_PSD_h[ind1:ind2]
        self.S_PSD_h = self.S_PSD_h[ind1:ind2]

        # subtract noise and set negative values to zero
        weights = (2 * np.pi * self.f_PSD_h)**(-4)
        self.S_PSD_h = self.S_PSD_h - global_noise_acc*weights
        self.S_PSD_h[self.S_PSD_h < 0.0] = 0.0

        if self.verbose > 2:
            printi("Smax = " + str(np.max(self.S_PSD_WS)))
            printi("R(Smax) = " + str(self.R[np.argmax(self.S_PSD_WS)]))
            printi("theta(Smax) = " + str(np.rad2deg(self.theta[np.argmax(self.S_PSD_WS)])))
            printi("spread(Smax) = " + str(self.spread[np.argmax(self.S_PSD_WS)]))


    def compute_wave_spectrum_moments(self):
        """Compute the moments of the wave spectrum."""

        omega = 2 * np.pi * self.f_PSD_WS

        # note: integrate only on the 'valid' part of the spectrum
        self.M0 = integrate.trapz(self.S_PSD_WS, omega)
        self.M1 = integrate.trapz(self.S_PSD_WS * (omega), omega)
        self.M2 = integrate.trapz(self.S_PSD_WS * (omega**2), omega)
        self.M3 = integrate.trapz(self.S_PSD_WS * (omega**3), omega)
        self.M4 = integrate.trapz(self.S_PSD_WS * (omega**4), omega)
        self.MM1 = integrate.trapz(self.S_PSD_WS * (omega**(-1)), omega)
        self.MM2 = integrate.trapz(self.S_PSD_WS * (omega**(-2)), omega)

        if self.verbose > 2:
            printi("M0 = " + str(self.M0))
            printi("M1 = " + str(self.M1))
            printi("M2 = " + str(self.M2))
            printi("M3 = " + str(self.M3))
            printi("M4 = " + str(self.M4))
            printi("MM1 = " + str(self.MM1))
            printi("MM2 = " + str(self.MM2))

    def compute_spectral_properties(self):
        """Compute SWH and the peak period, both zero up-crossing and peak-to-peak,
        from spectral moments."""

        self.Hs = np.sqrt(self.M0) * 4.0 / np.sqrt(2 * np.pi)
        self.T_z = 2.0 * np.pi * np.sqrt(self.M0 / self.M2)
        self.T_c = 2.0 * np.pi * np.sqrt(self.M2 / self.M4)
        self.T_p = 1.0 / self.f_PSD_WS[np.argmax(self.S_PSD_WS)]
        M02 = integrate.trapz(self.S_PSD_h, 2*np.pi*self.f_PSD_h)
        Hs2 = np.sqrt(M02) * 4.0 / np.sqrt(2 * np.pi)

        if self.verbose > 1:
            printi("Hs(from M0 acc) = " + str(self.Hs))
            printi("Hs(from M0 elev) = " + str(Hs2))
            printi("T_z = " + str(self.T_z))
            printi("T_c = " + str(self.T_c))
            printi("T_p = " + str(self.T_p))

    def reduce_wave_spectrum(self):
        """A rediced wave spectrum, so that easier to transmit (less information).
        Reduction in information is obtained by restraining the frequency domain,
        reducing to 8 bits per frequency"""

        S0 = self.S_PSD_WS
        f0 = self.f_PSD_WS
        for i in range(number_of_downsamples):
            num = len(f0) / global_under_sampling
            S0, f0 = signal.resample(S0, num, t=f0)
        
        self.array_discretized_spectrum = S0
        self.limited_frequencies_frequencies = f0
        self.max_value_limited_spectrum = np.max(self.array_discretized_spectrum)

        # normalize by max value and change to uint8
        # first set all negative values to 0
        self.array_discretized_spectrum[self.array_discretized_spectrum < 0] = 0.0
        self.array_discretized_spectrum = self.array_discretized_spectrum / self.max_value_limited_spectrum
        self.array_discretized_spectrum = np.uint8(255 * self.array_discretized_spectrum)

        # also reduce theta, R, and spread
        f0 = self.f_PSD_WS
        theta0 = self.theta
        R0 = self.R
        spread0 = self.spread
        for i in range(number_of_downsamples):
            num = len(f0) / global_under_sampling
            theta0, f0 = signal.resample(theta0, num, t=f0)
            R0 = signal.resample(R0, num)
            spread0 = signal.resample(spread0, num)

        self.theta_reduced = theta0
        self.spread_reduced = spread0
        self.R_reduced = R0

        if self.verbose > 3:
            printi("limited_frequencies_frequencies = " + str(self.limited_frequencies_frequencies))
            printi("array_discretized_spectrum = " + str(self.array_discretized_spectrum))
            printi("max reduced = " + str(self.max_value_limited_spectrum))
            printi("number of limited frequencies = {}".format(len(self.limited_frequencies_frequencies)))
            printi("number of non-zero amplitudes = {}".format(len(np.argwhere(self.array_discretized_spectrum>0))))

    def save_all_results(self):

        if plot_diag:
            # first plot spec
            noise_spec = global_noise_acc / ( (2*np.pi*self.f_PSD_WS)**4 )
            plt.figure()
            plt.plot(self.f_PSD_WS,self.S_PSD_WS,'k-')
            plt.plot(self.limited_frequencies_frequencies, self.max_value_limited_spectrum*self.array_discretized_spectrum/255.0, 'r.')
	    plt.xlim([global_lowcut, global_highcut])
            plt.yscale('log')

            plt.figure()
            plt.plot(self.f_PSD_WS, np.rad2deg(self.spread))
            plt.plot(self.limited_frequencies_frequencies, np.rad2deg(self.spread_reduced), 'r.')
            plt.ylabel('spread')

            plt.figure()
            plt.plot(self.f_PSD_WS, self.R)
            plt.plot(self.limited_frequencies_frequencies, self.R_reduced, 'r.')
            plt.ylabel('R')
            #plt.ylim([0,2])

            plt.figure()
            plt.plot(self.f_PSD_WS, np.rad2deg(self.theta))
            plt.plot(self.limited_frequencies_frequencies, np.rad2deg(self.theta_reduced), 'r.')
            plt.ylabel('theta')
            plt.show()

        # save as text files ---------------------------------------------------
        np.savetxt(self.path_in + '/' + self.filename + '_SWH.csv', np.array([self.SWH]))

        spectral_properties = np.array([self.Hs, self.T_z, self.T_c, self.T_p], dtype=np.float16)
        np.savetxt(self.path_in + '/' + self.filename + '_spectral_properties.csv', spectral_properties)

        np.savetxt(self.path_in + '/' + self.filename + '_limited_frequencies_frequencies.csv', \
                self.limited_frequencies_frequencies)

        # save as byte string for what should be sent by iridium ---------------
        with open(self.path_in + '/' + self.filename + '_SWH.bdat', 'wb') as f:
            f.write(np.array([self.SWH], dtype=np.float16).tostring())

        with open(self.path_in + '/' + self.filename + '_binary_red_spectrum.bdat', 'wb') as f:
            f.write(self.array_discretized_spectrum.tostring())

        with open(self.path_in + '/' + self.filename + '_spectral_properties.bdat', 'wb') as f:
            f.write(spectral_properties.tostring())

        with open(self.path_in + '/' + self.filename + '_max_value_limited_spectrum.bdat', 'wb') as f:
            f.write(np.array(self.max_value_limited_spectrum, dtype=np.float16).tostring())
