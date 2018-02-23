import numpy as np

nbit_convert = {
        8 : np.int8,
        16 : np.int16,
        32 : np.int32,
        64 : np.int64,
        }

a = nbit_convert[8](16)
print 'type of a should be 8bit: {}'.format(type(a))
a = nbit_convert[16](16)
print 'type of a should be 16bit: {}'.format(type(a))
a = nbit_convert[32](16)
print 'type of a should be 32bit: {}'.format(type(a))
a = nbit_convert[64](16)
print 'type of a should be 64bit: {}'.format(type(a))

if 32 not in nbit_convert:
    print 'not here'
else:
    print 'here'
