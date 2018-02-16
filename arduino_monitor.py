import serial
import os

# Setup
count = 0
fileCount = 1
# ttyUSB0
ser = serial.Serial('/dev/ttyUSB0', baudrate=9600, timeout=1)
text_file = open('test.3.2017_12.10pm' + str(fileCount) + '.txt', 'w')

# Loop
while 1:
    arduinoData = ser.readline()
    a = str(arduinoData)
    print (a)
    text_file.write(a)
    text_file.flush()
    count = count + 1
    if count == 25200:
        fileCount = fileCount + 1
        text_file = open('position' + str(fileCount) + '.txt', 'w')
        count = 0
