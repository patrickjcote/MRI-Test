#!/usr/bin/python
import sys
import time
import RPi.GPIO as GPIO
import pigpio


# Define pins
enablePin = 16
direction = 20
stepPin = 12
limit = 26

step = pigpio.pi()

# Define Times
waitTime = .000010
stepCount = 0
stepDir = -1
stepsToMove = 4000

time.sleep(1)

# GPIO Init
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(enablePin,GPIO.OUT)
GPIO.setup(direction,GPIO.OUT)
GPIO.setup(limit,GPIO.IN)

GPIO.output(enablePin,True)
time.sleep(waitTime)
GPIO.output(direction,True)
time.sleep(waitTime)

steps = step.callback(stepPin)

step.hardware_PWM(stepPin, 1000,10000)

while True:
    time.sleep(.1)
    if GPIO.input(limit)==0:
        stepCount=0
        GPIO.output(direction,False)
        break

while True:
    step.hardware_PWM(stepPin, 1000,10000)
    time.sleep(4)
    steps.reset_tally()
    GPIO.output(direction,True)
    time.sleep(waitTime)
    step.hardware_PWM(stepPin, 1000,10000)
    time.sleep(4)
    GPIO.output(direction,False)
    time.sleep(waitTime)
    print(GPIO.input(limit))
    if GPIO.input(limit)==0:
        stepCount=0
        GPIO.output(direction,False)
        break


step.hardware_PWM(stepPin, 0, 0)
GPIO.output(enablePin,False)
