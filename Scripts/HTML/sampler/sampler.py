import sys
import time
import smbus

class Sampler:
    
    def __init__(self, address):
        self.bus = smbus.SMBus(1)
        self.address = address

    def write(self,cmd):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))
    
    def read(self):
        self.readArray =  self.bus.read_i2c_block_data(self.address, 3)
        print chr(self.readArray[0])+chr(self.readArray[1])+chr(self.readArray[2])
    
    def bottlePosition(self,bottle):
        cmd = 'Q'
        if bottle < 10:
             cmd = 'BP00'+str(bottle)
        elif bottle < 100:
             cmd = 'BP0'+str(bottle)
        elif bottle < 1000:
             cmd = 'BP'+str(bottle)
        self.write(cmd)
        time.sleep(.01)
        self.read()

    def pump(self,pumpTime,direction):
        if pumpTime < 10:
             cmd = 'P'+direction+'00'+str(pumpTime)
        elif pumpTime < 100:
             cmd = 'P'+direction+'0'+str(pumpTime)
        elif pumpTime < 1000:
             cmd = 'P'+direction+str(pumpTime)
        self.write(cmd)
        time.sleep(.01)
        self.read()
        time.sleep(.1)
    
    def i(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'I'))
        time.sleep(.01)
        self.read()
        time.sleep(.1)

    def bottleNumber(self):
        self.write('BN')
        time.sleep(.1)
        self.read()
        time.sleep(.1)
    
    def status(self):
        self.write('Q')
        time.sleep(.1)
        self.read()
        time.sleep(.1)
    
    def stop(self):
        self.write('S')

cmd = sys.argv[1]
value = sys.argv[2]

x = Sampler(0x52)
if(cmd == "bp"):
    x.bottlePosition(int(value))
elif(cmd == "bn"):
    x.bottleNumber()
elif(cmd == "pf"):
    x.pump(int(value), "F")
elif(cmd == "pr"):
    x.pump(int(value), "R")
elif(cmd == "q"):
    x.status()
elif(cmd == "s"):
    x.stop()
elif(cmd == "i"):
    x.i();
else:
    x.write(str(cmd)+str(value))

