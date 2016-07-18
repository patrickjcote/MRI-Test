import serial
import sys

fileName = sys.argv[1]

#set read flag to "Reading"
f=open('r.flag','w')
f.write('R')
f.close()

#clear current data file
f=open('current.txt','w')
f.write('')
f.close()

port = serial.Serial("/dev/ttyUSB0", baudrate=1200, timeout=3.0)
while True:
    f=open('current.txt','a+')
    f2=open("logs/"+fileName,'a+')
    rcv = port.read(100)
    f.write(rcv)
    f2.write(rcv)
    f.close()
    f2.close()
    f3=open('r.flag','r')
    s = f3.read();
    f3.close()
    if s[0] == 'X':
        break
print 'Exit'

