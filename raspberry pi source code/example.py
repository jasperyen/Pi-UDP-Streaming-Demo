from picamera.array import PiRGBArray
from picamera import PiCamera
import time

import py_handler
import numpy as np

width = 1280
height = 720

camera = PiCamera()
camera.resolution = (width, height)
camera.framerate = 30
rawCapture = PiRGBArray(camera, size=(width, height))
time.sleep(0.1)

#sender = py_handler.PyStreamHandler(width, height, 60, "192.168.3.103", 17788, 65000)
sender = py_handler.PyStreamHandler(width, height, 60, "192.168.3.103", 17788, "192.168.3.100", 17788, 65000)

for frame in camera.capture_continuous(rawCapture, format="bgr", use_video_port=True):
    image = frame.array

    sender.sendBGRImage(image)

    rawCapture.truncate(0)
