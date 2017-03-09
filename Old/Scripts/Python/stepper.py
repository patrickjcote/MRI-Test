#!/usr/bin/python
import sys
import time
import RPi.GPIO as GPIO


# Define pins
enablePin = 16
direction = 20
step = 21
limit = 26

# GPIO Init
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(enablePin,GPIO.OUT)
GPIO.setup(direction,GPIO.OUT)
GPIO.setup(step,GPIO.OUT)
GPIO.setup(limit,GPIO.IN)

# Define Times
waitTime = .000010
pulseDown = .000570
pulseUp = .000005

stepCount = 0
stepDir = -1

GPIO.output(step,False)
GPIO.output(enablePin,True)
time.sleep(waitTime)
GPIO.output(direction,True)
time.sleep(waitTime)

while True:
    GPIO.output(step, True)
    time.sleep(pulseUp)  
    GPIO.output(step, False)
    time.sleep(pulseDown)
    
    if GPIO.input(limit)==0:
        stepCount=0
        GPIO.output(direction,False)
        break

time.sleep(1)

stepDir = 1
count = 0
while count < 1000:

    GPIO.output(step, True)
    time.sleep(pulseUp)  
    GPIO.output(step, False)
    time.sleep(pulseDown)
    count += 1
    
while True:
    GPIO.output(step, True)
    time.sleep(pulseUp)  
    GPIO.output(step, False)
    time.sleep(pulseDown)
    
    stepCount += stepDir

    if stepCount > 10000:
        GPIO.output(enablePin,False)
        time.sleep(waitTime)
        GPIO.output(enablePin,True)
        time.sleep(waitTime)
        GPIO.output(direction,True)
        stepDir = -1

    if stepCount < 0:
        GPIO.output(enablePin,False)
        time.sleep(waitTime)
        GPIO.output(enablePin,True)
        time.sleep(waitTime)
        GPIO.output(direction,False)
        stepDir = 1
    
    if GPIO.input(limit)==0:
        stepCount=0
        GPIO.output(direction,False)
        GPIO.output(enablePin,False)
        break
