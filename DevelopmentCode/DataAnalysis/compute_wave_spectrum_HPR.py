import numpy as np

"""
a program to calculate directional spectrum from HPR. a copy of the code provided by Graig. Possibly more updated version available on the git DirectionalAnalysisSpectra repo.
"""

global g
g = 9.81  # acceleration due to gravity


class DirectionalSpectra(object):
    '''A program to calculate directional spectra from Fourier coefficients'''

    def __init__(self, a0, a1, a2, b1, b2, R, freqs, ndir):
        self.freqs = freqs
        self.a0 = a0
        self.a1 = a1
        self.a2 = a2
        self.b1 = b1
        self.b2 = b2
        self.R = R
        self.k = self.R * ((2 * np.pi * self.freqs)**2) / g
        self.k0 = (2 * np.pi * self.freqs)**2 / g
        nfreqs = np.size(freqs)
        #  df = 2*np.pi / ndir
        self.dirs = np.linspace(-np.pi, np.pi, ndir)
        #  self.dirs = np.linspace(0, 2*np.pi - df, ndir)
        #  self.SDIR = np.zeros((nfreqs, ndir))

    def FEM(self):
        '''calculate directional spectra with Fourier'''
        #  weights = np.array([1.0, 1.0])/np.pi
        weights = np.array([2. / 3., 1. / 6.]) / np.pi
        nf = np.size(self.freqs)
        nd = np.size(self.dirs)
        a02 = self.a0.repeat(nd).reshape((nf, nd))
        a12 = self.a1.repeat(nd).reshape((nf, nd))
        b12 = self.b1.repeat(nd).reshape((nf, nd))
        a22 = self.a2.repeat(nd).reshape((nf, nd))
        b22 = self.b2.repeat(nd).reshape((nf, nd))
        t2 = self.dirs.repeat(nf).reshape((nd, nf)).T
        # calculate directional spectrum
        self.S = 0.5 * a02 + weights[0] * (a12 * np.cos(t2) + b12 * np.sin(t2)) + \
            weights[1] * (a22 * np.cos(2 * t2) + b22 * np.sin(2 * t2))

    def MLM(self):
        '''Calculate directional spectrum using IMLM method'''
        # first define matrix components in frequencies
        M11 = self.a0
        M22 = 0.5 * (self.a0 + self.a2)
        M33 = 0.5 * (self.a0 - self.a2)
        M12 = 0.0 - 1j * self.a1
        M13 = 0.0 - 1j * self.b1
        M23 = 0.5 * self.b2
        M21 = np.conj(M12)
        M32 = np.conj(M23)
        M31 = np.conj(M13)

        # go through directional spectrum
        nf = np.size(self.freqs)
        nd = np.size(self.dirs)
        E = 1j * np.zeros((nf, nd))
        D = np.zeros((nf, nd))
        S = self.a0.repeat(nd).reshape((nf, nd))
        G = 1j * np.zeros((3, nd))
        G[0, :] = 1.0
        G[1, :] = 1j * np.cos(self.dirs)
        G[2, :] = 1j * np.sin(self.dirs)

        # cycle through frequencies
        for ff, freq in enumerate(self.freqs):
            M = np.matrix('{} {} {} ; {} {} {} ; {} {} {}'.format(M11[ff], M12[ff], M13[ff],
                                                                  M21[ff], M22[ff], M23[ff], M31[ff], M32[ff], M33[ff]))
            invM = np.array(np.linalg.inv(M))
            # iterate over dimensions
            for n in range(3):
                for m in range(3):
                    E[ff, :] = E[ff, :] + invM[m, n] * G[n, :] * np.conj(G[m, :])

            # start iterative procedure
            E0 = 1.0 / E[ff, :]
            E0 = E0 / np.trapz(E0, self.dirs)

            D[ff, :] = E0

        # define some parameters
        self.D = D
        self.S = S * self.D

    def IMLM(self, gamma=0.1, beta=1.0, alpha=0.1, miter=100):
        '''Calculate directional spectrum using IMLM method'''
        # first define matrix components in frequencies
        M11 = self.a0
        M22 = 0.5 * (self.a0 + self.a2)
        M33 = 0.5 * (self.a0 - self.a2)
        M12 = 0.0 - 1j * self.a1
        M13 = 0.0 - 1j * self.b1
        M23 = 0.5 * self.b2 + 0.0 * 1j
        M21 = np.conj(M12)
        M32 = np.conj(M23)
        M31 = np.conj(M13)

        # go through directional spectrum
        nf = np.size(self.freqs)
        nd = np.size(self.dirs)
        E = 1j * np.zeros((nf, nd))
        D = np.zeros((nf, nd))
        S = self.a0.repeat(nd).reshape((nf, nd))
        G = 1j * np.zeros((3, nd))
        G[0, :] = 1.0
        G[1, :] = 1j * np.cos(self.dirs)
        G[2, :] = 1j * np.sin(self.dirs)

        # cycle through frequencies
        for ff, freq in enumerate(self.freqs):
            M = np.matrix('{} {} {}; {} {} {}; {} {} {}'.format(M11[ff], M12[ff], M13[ff],
                                                                M21[ff], M22[ff], M23[ff], M31[ff], M32[ff], M33[ff]))
            invM = np.array(np.linalg.inv(M))
            # iterate over dimensions
            for n in range(3):
                for m in range(3):
                    E[ff, :] = E[ff, :] + invM[m, n] * G[n, :] * np.conj(G[m, :])

            # start iterative procedure
            E0 = 1.0 / E[ff, :]
            E0 = E0 / np.trapz(E0, x=self.dirs)

            ee = np.copy(E0)
            tt = np.copy(E0)

            expG = 1j * np.zeros((3, 3, nd))
            ixps = np.matrix(1j * np.zeros((3, 3)))
            # cycle through iterations
            for it in range(miter):
                for n in range(3):
                    for m in range(3):
                        expG[m, n, :] = ee * G[n, :] * np.conj(G[m, :])
                        ixps[m, n] = np.trapz(expG[m, n, :], x=self.dirs)

                invcps = np.array(np.linalg.inv(ixps.T))
                Sftmp = np.zeros((nd,))
                for n in range(3):
                    for m in range(3):
                        xtemp = invcps[m, n] * G[n, :] * np.conj(G[m, :])
                        Sftmp = Sftmp + xtemp

                tt_old = np.copy(tt)
                tt = 1.0 / Sftmp
                tt = tt / np.trapz(tt, x=self.dirs)
                ei = gamma * ((E0 - tt) + alpha * (tt - tt_old))
                ee = ee + ei
                ee = ee / np.trapz(ee, x=self.dirs)

            # write to directional spectra
            D[ff, :] = np.real(ee)

        # define some parameters
        self.D = D
        self.S = S * self.D

    def spread(self):
        '''quick calculation of spread'''
        c1 = np.sqrt((self.a1 / self.a0)**2 + (self.b1 / self.a0)**2)
        c2 = np.sqrt((self.a2 / self.a0)**2 + (self.b2 / self.a0)**2)
        # calculate spread
        self.sigma1 = np.sqrt(2.0 * (1 - c1))
        self.sigma2 = np.sqrt(0.5 * (1 - c2))
        self.sigma = np.sqrt(-2.0 * np.log(c1))

    def theta(self):
        ''' quick calculation mean direction'''
        self.theta = np.arctan(self.b1 / self.a1)

    def Hm0(self):
        ''' calculate significant waveheight'''
        self.Hm0 = 4.0 * np.sqrt(np.trapz(self.a0, x=self.freqs))
