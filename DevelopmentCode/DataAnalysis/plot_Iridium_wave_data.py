import numpy as np
import matplotlib.pyplot as plt
from load_Iridium_wave_data import expand_raw_variables


def plot_data(dict_data, outpout_fig=None):
    freq = dict_data["raw_data"]["freq"]
    noise = (0.14 * 9.81e-3)**2 * ((2 * np.pi * freq)**(-4))
    dict_data["theretical_noise"] = (0.14 * 9.81e-3)**2 * ((2 * np.pi * freq)**(-4))

    (SWH, T_z0, Hs, T_z, freq, fmin, fmax, nfreq, a0_proc, a1_proc, a2_proc, b1_proc, b2_proc, R_proc) = expand_raw_variables(dict_data)

    plt.figure()
    plt.plot(freq, dict_data["raw_data"]["a0_proc"], '-o')
    plt.plot(freq, noise, 'k--')
    plt.yscale('log')
    plt.ylabel(r'$S \, / \,\mathrm{m}^2 \mathrm{Hz}^{-1}$')
    plt.xlabel(r'$\mathrm{f} \, / \, \mathrm{Hz}$')
    plt.xlim([fmin, fmax])

    if outpout_fig is not None:
        plt.savefig(outpout_fig)

    plt.figure()
    plt.subplot(221)
    plt.plot(freq, a1_proc, '-o')
    plt.xlim([fmin, fmax])
    plt.subplot(222)
    plt.plot(freq, b1_proc, '-o')
    plt.xlim([fmin, fmax])
    plt.subplot(223)
    plt.plot(freq, a2_proc, '-o')
    plt.xlim([fmin, fmax])
    plt.subplot(224)
    plt.plot(freq, b2_proc, '-o')
    plt.xlim([fmin, fmax])

    plt.figure()
    plt.plot(freq, R_proc, '-o')
    plt.xlim([fmin, fmax])
    plt.ylabel(r'$ \mathrm{R}$')
    plt.xlabel(r'$\mathrm{f} \, / \, \mathrm{Hz}$')

    plt.show()
