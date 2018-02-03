# AQUA
## UNIV 492 Project Code

## Raspberry PI Specification
### Hardware
When the following command, the currently used PI, outputs "Raspberry Pi 2 Model B Rev 1.1"
```bash
    cat /proc/device-tree/model
```
```
  Raspberry Pi 2 Model B Rev 1.1
```
### Operating System
```bash
    uname -a
```
```
  Linux raspberrypi 4.1.19-v7+ \#858 SMP Tues Mar 15 15:56:00 GTM 2016 armv7l GNU/Linux
```
### Python Source Code
It seems like python 2.7 was used; however, I am not sure.
The version of pyserial (imported in the sensor_final.py as serial) is 2.6.
