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
waitTime = .000005

# GPIO Init
GPIO.setwarnings(False)
GPIO.setmode(GPIO.BCM)
GPIO.setup(enablePin,GPIO.OUT)
GPIO.setup(direction,GPIO.OUT)
GPIO.setup(limit,GPIO.IN)

GPIO.output(enablePin,True)
time.sleep(waitTime)
GPIO.output(direction,False)
time.sleep(waitTime)

step.hardware_PWM(stepPin, 1000,10000)
time.sleep(4)

step.hardware_PWM(stepPin, 0, 0)
time.sleep(2)

step.stop()
