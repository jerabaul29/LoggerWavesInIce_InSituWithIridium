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

# parameters for the band pass filtering
global_lowcut = 0.04
global_highcut = 0.5
global_fs = 5.0

# parameters for the part of the acceleration signal to use
# avoid beginning and end of signal, fixed length in FFTs and Welch so
# that no need to transmit the frequence points by Iridium
number_of_points_to_use = int(global_fs * 60 * 20)  # 20 minutes
first_IMU_point_to_use = int(global_fs * 1)
last_IMU_point_to_use = int(first_IMU_point_to_use + number_of_points_to_use)
# so the points to use are [first_IMU_point_to_use:last_IMU_point_to_use]

# under sampling of the Fourier spectra
global_under_sampling = 2
################################################################################


class ButterFiltering(object):
    """A class to perform bandpass filtering using Butter filter."""

    def __init__(self, lowcut=global_lowcut, highcut=global_highcut, fs=global_fs, order=5):
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
        self.load_LSM9DS0_data()
        self.extract_vertical_acceleration()
        self.compute_vertical_elevation()
        self.compute_SWH()
        self.compute_elevation_spectrum()
        self.find_valid_index_PSD_WS()
        self.compute_wave_spectrum_moments()
        self.compute_spectral_properties()
        self.reduce_wave_spectrum()

    def load_LSM9DS0_data(self):
        """Load LSM9DS0 IMU data.
        Data is ordered in each line as:
        ACCX, ACCY, ACCZ, GYRX, GYRY, GYRZ, MAGX, MAGY, MAGZ"""

        self.data_I = np.genfromtxt(self.path_in + '/' + self.filename, delimiter=';')

        if self.verbose > 3:
            printi(str(self.data_I))

        self.ACCX = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 0]
        self.ACCY = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 1]
        self.ACCZ = self.data_I[first_IMU_point_to_use:last_IMU_point_to_use, 2]

    def extract_vertical_acceleration(self):
        """Extract vertical acceleration."""

        # find direction of mean acceleration: this is the vertical
        mean_acc_x = np.mean(self.ACCX)
        mean_acc_y = np.mean(self.ACCY)
        mean_acc_z = np.mean(self.ACCZ)

        mean_acc_norm = np.sqrt(mean_acc_x**2 + mean_acc_y**2 + mean_acc_z**2)

        if self.verbose > 0:
            printi("mean_acc_norm = " + str(mean_acc_norm))
            printi("mean_acc_x = " + str(mean_acc_x))
            printi("mean_acc_y = " + str(mean_acc_y))
            printi("mean_acc_z = " + str(mean_acc_z))

        # project acceleration on vertical
        self.acc_vertical = (mean_acc_x * self.ACCX + mean_acc_y * self.ACCY + mean_acc_z * self.ACCZ) / mean_acc_norm
        self.acc_vertical = self.acc_vertical - np.mean(self.acc_vertical)

    def compute_vertical_elevation(self):
        """integrate twice vertical acceleration to get vertical elevation"""

        # band pass filter to use
        bandpass_instance = ButterFiltering(fs=self.fs)

        # filter and integrate, 1
        acc_z_filtered = bandpass_instance.filter_data(self.acc_vertical)
        vel_z = integrate.cumtrapz(acc_z_filtered, dx=1.0 / self.fs)

        # filter and integrate, 2
        vel_z_filtered = bandpass_instance.filter_data(vel_z)
        pos_z = integrate.cumtrapz(vel_z_filtered, dx=1.0 / self.fs)

        # filter
        self.pos_z_filtered = bandpass_instance.filter_data(pos_z)

        if self.verbose > 3:
            printi("self.pos_z_filtered = " + str(self.pos_z_filtered))
            printi("max pos_z_filtered: " + str(np.max(self.pos_z_filtered)))
            printi("min pos_z_filtered: " + str(np.min(self.pos_z_filtered)))

    def compute_SWH(self):
        """Compute SWH using double integration of vertical acceleration."""

        # SWH
        self.SWH = 4.0 * np.std(self.pos_z_filtered)

        if self.verbose > 1:
            printi("SWH = " + str(self.SWH))

    def compute_elevation_spectrum(self):
        """Compute elevation spectrum from the double integrated vertical acceleration."""

        # Fourier spectrum using FFT
        self.S_FFT = np.fft.fft(self.pos_z_filtered)
        self.f_FFT = np.fft.fftfreq(self.pos_z_filtered.shape[-1])

        # power spectrum using the Welch method
        # duration of a segment for the Welch method in minutes
        time_duration_segment = 5
        self.f_PSD_WS, self.S_PSD_WS = signal.welch(self.pos_z_filtered, fs=self.fs, nperseg=self.fs * 60 * time_duration_segment)

    def find_valid_index_PSD_WS(self):
        """Find the limiting index that are within global_lowcut global_highcut."""

        self.index_min_PSD_WS = -1
        self.index_max_PSD_WS = -1

        for current_index in range(self.f_PSD_WS.shape[0]):
            crrt_value = self.f_PSD_WS[current_index]

            if self.index_min_PSD_WS == -1:
                if crrt_value > global_lowcut:
                    self.index_min_PSD_WS = current_index

            if self.index_max_PSD_WS == -1:
                if crrt_value > global_highcut:
                    self.index_max_PSD_WS = current_index

    def compute_wave_spectrum_moments(self):
        """Compute the moments of the wave spectrum."""

        omega = 2 * np.pi * self.f_PSD_WS

        # note: integrate only on the 'valid' part of the spectrum
        self.M0 = integrate.trapz(self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS], omega[self.index_min_PSD_WS: self.index_max_PSD_WS])
        self.M1 = integrate.trapz(self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS] * (omega[self.index_min_PSD_WS: self.index_max_PSD_WS]), omega[self.index_min_PSD_WS: self.index_max_PSD_WS])
        self.M2 = integrate.trapz(self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS] * (omega[self.index_min_PSD_WS: self.index_max_PSD_WS]**2), omega[self.index_min_PSD_WS: self.index_max_PSD_WS])
        self.M3 = integrate.trapz(self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS] * (omega[self.index_min_PSD_WS: self.index_max_PSD_WS]**3), omega[self.index_min_PSD_WS: self.index_max_PSD_WS])
        self.M4 = integrate.trapz(self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS] * (omega[self.index_min_PSD_WS: self.index_max_PSD_WS]**4), omega[self.index_min_PSD_WS: self.index_max_PSD_WS])

        if self.verbose > 2:
            printi("M0 = " + str(self.M0))
            printi("M1 = " + str(self.M1))
            printi("M2 = " + str(self.M2))
            printi("M3 = " + str(self.M3))
            printi("M4 = " + str(self.M4))

    def compute_spectral_properties(self):
        """Compute SWH and the peak period, both zero up-crossing and peak-to-peak,
        from spectral moments."""

        self.Hs = np.sqrt(self.M0) * 4.0 / np.sqrt(2 * np.pi)
        self.T_z = 2.0 * np.pi * np.sqrt(self.M0 / self.M2)
        self.T_c = 2.0 * np.pi * np.sqrt(self.M2 / self.M4)

        if self.verbose > 1:
            printi("Hs = " + str(self.Hs))
            printi("T_z = " + str(self.T_z))
            printi("T_c = " + str(self.T_c))

    def reduce_wave_spectrum(self):
        """A rediced wave spectrum, so that easier to transmit (less information).
        Reduction in information is obtained by restraining the frequency domain,
        reducing to 8 bits per frequency"""

        # spectrum between the cutoff frequencies, under sample
        limited_frequencies_spectrum = self.S_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS: global_under_sampling]
        self.limited_frequencies_frequencies = self.f_PSD_WS[self.index_min_PSD_WS: self.index_max_PSD_WS: global_under_sampling]

        # max value in the limited spectrum
        self.max_value_limited_spectrum = np.max(limited_frequencies_spectrum)

        # total number of points in the limited spectrum
        n_points_limited_spectrum = limited_frequencies_spectrum.shape[0]

        # discretize as uint8
        # build array
        self.array_discretized_spectrum = np.zeros((n_points_limited_spectrum, ), dtype=np.uint8)
        # add all points
        for current_index in range(n_points_limited_spectrum):
            current_value = limited_frequencies_spectrum[current_index]
            current_value_as_uint8 = np.uint8(current_value * 255 / self.max_value_limited_spectrum)
            self.array_discretized_spectrum[current_index] = current_value_as_uint8

        if self.verbose > 3:
            printi("array_discretized_spectrum = " + str(self.array_discretized_spectrum))

    def save_all_results(self):
        # save as text files ---------------------------------------------------
        np.savetxt(self.path_in + '/' + self.filename + '_SWH.csv', np.array([self.SWH]))

        spectral_properties = np.array([self.Hs, self.T_z, self.T_c], dtype=np.float16)
        np.savetxt(self.path_in + '/' + self.filename + '_spectral_properties.csv', spectral_properties)

        np.savetxt(self.path_in + '/' + self.filename + '_limited_frequencies_frequencies.csv', self.limited_frequencies_frequencies)

        # save as byte string for what should be sent by iridium ---------------
        with open(self.path_in + '/' + self.filename + '_SWH.bdat', 'wb') as f:
            f.write(np.array([self.SWH], dtype=np.float16).tostring())

        with open(self.path_in + '/' + self.filename + '_binary_red_spectrum.bdat', 'wb') as f:
            f.write(self.array_discretized_spectrum.tostring())

        with open(self.path_in + '/' + self.filename + '_spectral_properties.bdat', 'wb') as f:
            f.write(spectral_properties.tostring())

        with open(self.path_in + '/' + self.filename + '_max_value_limited_spectrum.bdat', 'wb') as f:
            f.write(np.array(self.max_value_limited_spectrum, dtype=np.float16).tostring())
