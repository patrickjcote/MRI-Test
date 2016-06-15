import smbus


class REEL:
    
    def __init__(self):
        self.bus = smbus.SMBus(1)
        self.address = 0x48

    def write(self,cmd):
        self.bus.write_i2c_block_data(self.address, 0 , map(ord,cmd))
    
    def read(self):
        self.readArray =  self.bus.read_i2c_block_data(self.address, 3)
        print chr(self.readArray[0])+chr(self.readArray[1])+chr(self.readArray[2])
