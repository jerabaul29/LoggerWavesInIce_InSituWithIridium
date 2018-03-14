import numpy as np
import matplotlib.pyplot as plt
import struct
import os

home = os.path.expanduser("~")

dataDir = "/home/jrlab/Downloads/"
f1 = 488
f2 = f1+1
file1 = 'F{:05d}_B'.format(f1)
file2 = 'F{:05d}_B'.format(f2)
inFile = "300234066333770-1.bin"

# some frequency stuff
fmin = 0.05
fmax = 0.25
nfreq = 25
freq = np.exp(np.linspace(np.log(fmin), np.log(fmax), nfreq))

# some format stuff
fmt_hdr = '<' + 'f'*10
fmt_array = '<' + 'h'*nfreq
fmt_all = '<' + 'f'*10 + 'h'*nfreq*6

#read in data

with open(dataDir + inFile, 'rb') as f:
    bin_data = f.read()

data = struct.unpack(fmt_all, bin_data)

SWH = data[0]
T_z0 = data[1]
Hs = data[2]
T_z = data[3]
a0_max = data[4]
a1_max = data[5]
b1_max = data[6]
a2_max = data[7]
b2_max = data[8]
R_max = data[9]
a0 = np.array(data[10:35])
a1 = np.array(data[35:60])
b1 = np.array(data[60:85])
a2 = np.array(data[85:110])
b2 = np.array(data[110:135])
R = np.array(data[135:160])

print 'a0_max is {} type'.format(type(a0_max))
print 'a0 is {} type'.format(type(a0))
print a0

print 'SWH = {0:.3f} and Hs = {1:.3f}'.format(SWH,Hs)
print 'T_z0 = {0:.2f} and T_z = {1:.2f}'.format(T_z0, T_z)

max_val = 2**15 - 1.0

a0_proc = a0 * a0_max / max_val
a1_proc = a1 * a1_max / max_val
b1_proc = b1 * b1_max / max_val
a2_proc = a2 * a2_max / max_val
b2_proc = b2 * b2_max / max_val
R_proc = R * R_max / max_val

noise = (0.24*9.81e-3)**2 * ( (2*np.pi*freq)**(-4) )

plt.figure()
plt.plot(freq, a0_proc, '-o')
plt.plot(freq, noise, 'k--')
plt.yscale('log')
plt.xlim([fmin, fmax])

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
plt.show()
