import sensor
import time
from pyb import UART, LED

# SETTINGS
exposure = 12000
widSize = 240

# Camera Set Up
sensor.reset()
sensor.set_pixformat(sensor.RGB565)
sensor.set_framesize(sensor.QVGA)
sensor.skip_frames(time=2000)
sensor.set_auto_gain(False)
sensor.set_auto_whitebal(False)
sensor.set_auto_exposure(False, exposure_us=exposure)
sensor.set_windowing((widSize,widSize))

clock = time.clock()

# UART
uart = UART(3, 115200, timeout_char=100)

# Thresholds | blue : yellow : Orange |

t =[(54, 100, -128, -5, -128, -11),(48, 100, -128, 127, 43, 127),(0, 100, 3, 127, 30, 127)]

while True:

    # Camera Data
    img = sensor.snapshot()
    blobs = sorted(img.find_blobs(t,  pixels_threshold=20, area_threshold=20, merge=True), key=lambda blob: blob.area())
    data = [[-1,-1,-1],[-1,-1,-1],[-1,-1,-1]]
    for blob in blobs:
        if(blob.code() < 3):
            data[blob.code()] = [blob.cx(),blob.cy(),min(int(blob.area()/100),255)]
            img.draw_rectangle(blob.x(),blob.y(),blob.w(),blob.h())

    # Sending Data
    uart.writechar(200)
    uart.writechar(122)
    uart.writechar(data[2][0])
    uart.writechar(data[2][1])
    uart.writechar(data[2][2])
    uart.writechar(data[1][0])
    uart.writechar(data[1][1])
    uart.writechar(data[1][2])
    uart.writechar(data[0][0])
    uart.writechar(data[0][1])
    # uart.writechar(data[0][2]) <-- Add in later
    print(data)
