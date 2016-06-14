import smbus


bus = smbus.SMBus(1)
address = 0x48


bus.write_i2c_block_data(address, 0 , [ord('R'), ord('D'),ord('0'),ord('1'),ord('0')])

readArray =  bus.read_i2c_block_data(address, 3)

print chr(readArray[0])+chr(readArray[1])+chr(readArray[2])
