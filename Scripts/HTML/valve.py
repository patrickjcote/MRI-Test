import sys
import time
import smbus

class Valve:
    
    def __init__(self, address):
        self.bus = smbus.SMBus(1)
        self.address = address

    def write(self,cmd):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))
    
    def read(self):
        self.readArray =  self.bus.read_i2c_block_data(self.address, 3)
        print chr(self.readArray[0])+chr(self.readArray[1])+chr(self.readArray[2])
    
    def purge(self,pumpTime):
        if pumpTime < 10:
             cmd = 'P00'+str(time)
        elif pumpTime < 100:
             cmd = 'P0'+str(time)
        elif pumpTime < 1000:
             cmd = 'P'+str(time)
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))
        time.sleep(.01)
        self.read()

    def valve(self,direction):
        cmd = 'V'+str(direction)
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))

    def time(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'T'))
        time.sleep(.01)
        self.read()

    def status(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'Q'))
        time.sleep(.01)
        self.read()

    def stop(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'S'))

cmd = sys.argv[1]
value = sys.argv[2]

x = Valve(0x50)
if(cmd == "p"):
    x.purge(int(value))
elif(cmd == "v"):
    x.valve(value)
elif(cmd == "t"):
    x.time()
elif(cmd == "q"):
    x.status()
elif(cmd == "s"):
    x.stop()
