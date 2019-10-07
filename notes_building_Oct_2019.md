## build the PCB 'as usual'. Fixes:

- 3.9k -> 39k
- 5.6k -> 56k
- 1.0 uF -> 0.1 uF
- 32k, 46k, 50k -> 47k

## code for the Arduino barebone

- power controller: using commit **b76f6a5** (current master IS BROKEN).

## code for the Arduino Mega logger

- take away the Arduino from logger shield
- upload the code (location master, current commit **39c30775849a**). Make sure that you well use the extended buffer compilation flags (see the platformio.ini parameters folder). The sizes of the data and program should read something in this kind:

```
DATA:    [========  ]  81.5% (used 6673 bytes from 8192 bytes)
PROGRAM: [=         ]  10.9% (used 27732 bytes from 253952 bytes)
```

I use the platformio plugin on Visual Studio Code to compile / upload.
