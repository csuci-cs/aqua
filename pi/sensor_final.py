import serial
import os

ser = serial.Serial('/dev/ttyUSB0', baudrate =9600, timeout =1)

fileCount = 1
text_file = open("test.3.2017_12.10pm" + str(fileCount) + ".txt", 'w')
count = 0

##file_humidity = open("parsefile_humidity.txt", 'w')
##file_temp = open("parsefile_temp.txt", 'w')
##file_o2 = open("parsefile_o2.txt", 'w')
##file_co = open("parsefile_co.txt", 'w')
##file_co2 = open("parsefile_co2.txt", 'w')
##file_altitude = open("parsefile_altitude.txt", 'w')

def splitFile(text_file):
    datafile = file("position" + str(fileCount) + ".txt")
    for line in datafile:
        if "humidity():" in line:
            file_humidity.write(line)
        if "temp():" in line:
            file_temp.write(line)
        if "02():" in line:
            file_o2.write(line)
        if "co():" in line:
            file_co.write(line)
        if "co2():" in line:
            file_co2.write(line)
        if "altitude():" in line:
            file_altitude.write(line)

def dayavg(a):
    sum=0
    countx = 0
    aa=open(a)
    message = ""
    for i in aa:
        j = i.split()
        countx= countx +1
        sum += int(j[1])

        if countx == 360:
            message += "1hr avg: " + str(sum / countx) + '\n'
        if countx == 720:
            message += "2hr avg: " + str(sum / countx) + '\n'
        if countx == 1080:
            message += "3hr avg: " + str(sum / countx) + '\n'
        if countx == 1440:
            message += "4hr avg: " + str(sum / countx) + '\n'
        if countx == 1800:
            message += "5hr avg: " + str(sum / countx) + '\n'
        if countx == 2160:
            message += "6hr avg: " + str(sum / countx) + '\n'
        if countx == 2520:
            message += "7hr avg: " + str(sum / countx) + '\n'
        if countx == 2880:
            message += "8hr avg: " + str(sum / countx) + '\n'
        if countx == 3240:
            message += "9hr avg: " + str(sum / countx) + '\n'
        if countx == 3600:
            message += "10hr avg: " + str(sum / countx) + '\n'
        if countx == 3960:
            message += "11hr avg: " + str(sum / countx) + '\n'
        if countx == 4320:
            message += "12hr avg: " + str(sum / countx) + '\n'
        if countx == 4680:
            message += "13hr avg: " + str(sum / countx) + '\n'
        if countx == 5040:
            message += "14hr avg: " + str(sum / countx) + '\n'
        if countx == 5400:
            message += "15hr avg: " + str(sum / countx) + '\n'
        if countx == 5760:
            message += "16hr avg: " + str(sum / countx) + '\n'
        if countx == 6120:
            message += "17hr avg: " + str(sum / countx) + '\n'
        if countx == 6480:
            message += "18hr avg: " + str(sum / countx) + '\n'
        if countx == 6840:
            message += "19hr avg: " + str(sum / countx) + '\n'
        if countx == 7200:
            message += "20hr avg: " + str(sum / countx) + '\n'
        if countx == 7560:
            message += "21hr avg: " + str(sum / countx) + '\n'
        if countx == 7920:
            message += "22hr avg: " + str(sum / countx) + '\n'
        if countx == 8280:
            message += "23hr avg: " + str(sum / countx) + '\n'
        if countx == 8640:
            message += "24hr avg: " + str(sum / countx) + '\n'

    totalavg = sum/countx


    return(message + "Total Avg: " + str(totalavg))

def mathParse():
    mypath = 'C:\Users\zgelu\PycharmProjects\gas'
    filename = 'humidity.txt'

    f = dayavg(os.path.join(mypath, filename))

    print(f)



while 1:
    arduinoData = ser.readline()
    ##print(arduinoData)
    a = str(arduinoData)

    print (a)

    text_file.write(a)


    text_file.flush()


    count = count + 1

    ##print(count)




    ##if count == 8640:
    if count == 25200:
        ##splitFile(text_file)
        ##mathParse()
        fileCount = fileCount + 1
        text_file = open("position" + str(fileCount) + ".txt", 'w')
        count = 0




##text_file.close()

## close the serial connection and text file

##ser.close()
