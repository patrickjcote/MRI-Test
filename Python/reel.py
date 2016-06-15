import smbus
import time

class Reel:
    
    def __init__(self, address):
        self.bus = smbus.SMBus(1)
        self.address = address

    def write(self,cmd):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))
    
    def read(self):
        self.readArray =  self.bus.read_i2c_block_data(self.address, 3)
        print chr(self.readArray[0])+chr(self.readArray[1])+chr(self.readArray[2])
    
    def rd(self,depth):
        if depth < 10:
             cmd = 'RD00'+str(depth)
        elif depth < 100:
             cmd = 'RD0'+str(depth)
        elif depth < 1000:
             cmd = 'RD'+str(depth)
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))

    def cd(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'CD'))
        time.sleep(.01)
        self.read()

    def q(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'Q'))
        time.sleep(.01)
        self.read()

    def pu(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'PU'))

    def s(self):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,'S'))

    def status(self):
        print('Current depth:')
        self.cd()
        print('Status Code:')
        self.q()
