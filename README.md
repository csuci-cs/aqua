# AQUA
## UNIV 492 Project Code

## Development Setup

The recommended development and initial testing setup consists of connecting an
Arduino directly to your computer via USB. Once you have the Arduino configured
with the Arduino IDE, use any text editor (such as https://atom.io) to work on
the code. Use the Arduino IDE to compile and upload code to the Arduino. Then
run the arduino_monitor.py from a command prompt (terminal).

To connect the arduino_monitor.py to the Arduino on \*nix systems,
* Connect the Arduino to your development computer
* Compile and upload the latest aqua.ino code to the Arduino using the Arduino IDE https://www.arduino.cc/en/Main/Software (version 1.8.\* was recently tested and worked on macOS High Sierra)
* find what the name of the serial port is being used (a quick search including your operating system and the phrase "how to find serial port for arduino on {{operating system}}" should help. Use it for the serial flag on the next step.
* run ```python arduino_monitor.py --serial=<>```. Note you do not need to use the serial flag if the default (/dev/ttyUSB0) works such as when the arduino_monitor.py is running on an the Raspberry Pi.
If everything is configured properly you should see data from the Arduino written to the console.
If this isn't working and you have tried to search online for a solution feel free to ask a team member for help and if you can't find a solution open an issue and @crhntr will try to figure out a solution.

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
