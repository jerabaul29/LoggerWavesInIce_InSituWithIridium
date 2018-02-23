import numpy as np
import matplotlib.pyplot as plt
import struct

dataDir = '/home/graigorys/Data/WOICE/pi_logger/ResultAnalyzis/'
f1 = 654
f2 = f1+1
file1 = 'F{:05d}_B'.format(f1)
file2 = 'F{:05d}_B'.format(f2)
inFile = 'test_{0:05d}_{1:05d}.bin'.format(f1,f2)

# some frequency stuff
fmin = 0.05
fmax = 0.25
nfreq = 25

pwr = -1
t0 = np.linspace(fmax**pwr, fmin**pwr, nfreq+1)
f0 = np.sort(t0**pwr)
freq = 0.5 * ( f0[0:-1] + f0[1:] )

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
T_c = data[4]
a0_max = data[5]
a1_max = data[6]
b1_max = data[7]
a2_max = data[8]
b2_max = data[9]
a0 = np.array(data[10:35])
a1 = np.array(data[36:61])
b1 = np.array(data[62:87])
a2 = np.array(data[88:113])
b2 = np.array(data[114:139])

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

plt.figure()
plt.semilogy(freq, a0_proc, '-o')
plt.xlim([fmin, fmax])

plt.figure()
plt.subplot(411)
plt.plot(freq, a1_proc, '-o')
plt.xlim([fmin, fmax])
plt.subplot(412)
plt.plot(freq, b1_proc, '-o')
plt.xlim([fmin, fmax])
plt.subplot(413)
plt.plot(freq, a2_proc, '-o')
plt.xlim([fmin, fmax])
plt.subplot(414)
plt.plot(freq, b2_proc, '-o')
plt.xlim([fmin, fmax])
plt.show()
