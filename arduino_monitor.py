#!/usr/bin/env python3

import serial
import os
from datetime import datetime

# Setup
ser = serial.Serial('/dev/ttyUSB0', baudrate=9600, timeout=1)

# Loop
while 1:
    text_file = open('data/data-log_' + str(datetime.now()) + '.txt', 'w')
    for i in range(0, 25200):
        arduinoData = ser.readline()
        a = str(arduinoData)
        print(a)
        text_file.write(a)
        text_file.flush()
