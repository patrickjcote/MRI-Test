#!/usr/bin/python
import sys
import time
import RPi.GPIO as GPIO

# Use BCM GPIO references
# instead of physical pin numbers
GPIO.cleanup()
GPIO.setmode(GPIO.BCM)

enablePin = 16
direction = 20
step = 21

# Define GPIO signals to use
StepPins = [enablePin,direction,step]

for pin in StepPins:
    GPIO.setup(pin,GPIO.OUT)
    GPIO.output(pin, False)
    print str(pin) + " Enabled"

waitTime = .000010
pulseDown = .000450
pulseUp = .000005
StepCounter = 0

GPIO.output(step,False)
GPIO.output(enablePin,True)
time.sleep(waitTime)
GPIO.output(direction,True)
time.sleep(waitTime)

# Start main loop
while True:

    print StepCounter

    GPIO.output(step, True)
    time.sleep(pulseUp)  
    GPIO.output(step, False)

    StepCounter += 1

    time.sleep(pulseDown)

    if StepCounter == 5000:
        GPIO.output(enablePin,False)
        break
