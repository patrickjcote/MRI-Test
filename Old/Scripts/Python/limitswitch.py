import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)

GPIO.setup(26,GPIO.IN)

while True:
    print GPIO.input(26)
