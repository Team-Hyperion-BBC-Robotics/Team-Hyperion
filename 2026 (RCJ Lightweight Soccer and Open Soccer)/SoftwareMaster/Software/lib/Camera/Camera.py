import sensor
import time
from pyb import UART, LED

# SETTINGS
exposure = 12000
widSize = 120

# Camera Set Up
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QQVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=exposure)
sensor.set_windowing((widSize,widSize))

clock = time.clock()

# UART
uart = UART(3, 115200, timeout_char=100)

# Thresholds | Yellow : Blue : Orange |
t =[(49, 100, -29, 127, 49, 127),(38, 100, -30, 127, -128, -14),(38, 100, -30, 127, -128, -14)]

while True:

    # Camera Data
    img = sensor.snapshot()
    data = [[-1,-1],[-1,-1],[-1,-1]]
    blobs = img.find_blobs(t,  pixels_threshold=10, area_threshold=10, merge=True)
    blobs = sorted(blobs, key=lambda blob: -blob.area())
    for blob in blobs:
        data[blob.code()] = [blob.cx(),blob.cy()]
        #img.draw_cross(blob.cx(),blob.cy(),(255,255,255),10,2)

    # Sending Data
    uart.writechar(200)
    uart.writechar(122)
    for i in range(3):
        uart.writechar(data[i][0])
        uart.writechar(data[i][1])
