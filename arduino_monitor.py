#!/usr/bin/env python3

import serial
import os
from datetime import datetime
import argparse

# Setup
parser = argparse.ArgumentParser(description='Process data from serial port sent from the arduino.')
parser.add_argument('--serial', dest='serial_name', action='store_const', default='/dev/ttyUSB0', help='serial port where arduino data can be read from. On *nix it is found in ')

ser = serial.Serial(parser.serial_name, baudrate=9600, timeout=1)

# Loop
while 1:
    text_file = open('data/data-log_' + str(datetime.now()) + '.txt', 'w')
    for i in range(0, 25200):
        arduinoData = ser.readline()
        a = str(arduinoData)
        print(a)
        text_file.write(a)
        text_file.flush()
