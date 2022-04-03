import cv2 as cv
from math import atan, atan2, cos, sin, sqrt, pi, hypot, asin
import numpy as np
import subprocess
import serial
import time
from ms_customvision import prediction

def serial_port_setup():
    ser = serial.Serial(
    port='/dev/ttyACM0',\
    baudrate=9600,\
    parity=serial.PARITY_NONE,\
    stopbits=serial.STOPBITS_ONE,\
    bytesize=serial.EIGHTBITS,\
    timeout=None)

    return ser

def get_center_coords(image):

    # Load the image
    img = cv.imread(image)

    # Was the image there?
    if img is None:
        print("Error: File not found")
        exit(0)

    #cv.imshow('Input Image', img)

    # Convert image to grayscale
    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)

    # Convert image to binary
    _, bw = cv.threshold(gray, 50, 255, cv.THRESH_BINARY | cv.THRESH_OTSU)

    # Find all the contours in the thresholded image
    contours, _ = cv.findContours(bw, cv.RETR_LIST, cv.CHAIN_APPROX_NONE)

    for i, c in enumerate(contours):

        # Calculate the area of each contour
        area = cv.contourArea(c)

        # Ignore contours that are too small or too large
        if area < 3700 or 100000 < area:
            continue

        rect = cv.minAreaRect(c)
        box = cv.boxPoints(rect)
        box = np.int0(box)

        cv.drawContours(img,[box],0,(0,0,255),2)
        cv.imwrite("new.jpg", img)
        #cv.imshow('Output Image', img)
        #time.sleep(3)
        #cv.destroyAllWindows()

        # Retrieve the key parameters of the rotated bounding box
        center = (int(rect[0][0]),int(rect[0][1]))
        width = int(rect[1][0])
        height = int(rect[1][1])
        angle = int(rect[2])

    return int(rect[0][0]), int(rect[0][1])

def get_rotate_angle(x, y):

    img_height = 1380
    #x_robot = 550
    x_robot = 588
    #y_robot = 0

    x_object = x - x_robot
    y_object = img_height -y

    angle = int(np.degrees(atan(y_object / x_object)))
    return angle

def get_tilt_angle(dist):

    h = 639
    a = 733
    #a = 500
    if (dist > a):
        b = dist - a
        c = hypot(h, b)
        angle = 175 - int(np.degrees(asin(b / c))) - 5

    else:
        b = a - dist
        c = hypot(h, b)
        angle = int(np.degrees(asin(b / c)))
        angle = 90 + angle

    return angle

def get_distance(x, y):

    img_height = 1380
    #x_robot = 550
    x_robot = 588

    x_object = x - x_robot
    y_object = img_height -y

    distance = int(sqrt(y_object*y_object + x_object*x_object))
    return distance

def main():

    offset = 0
    buff = ""
    serport = serial_port_setup()
    print("[INFO] Waiting for event from manipulator...")

    while True:

        while True:
            oneByte = serport.read(1)
            if oneByte == b"\r":
                break
            else:
                buff += oneByte.decode("ascii", "ignore")

        index = buff.find("button event")
        if index >= 0:
            print("[INFO] Button event from manipulator!")
            print("[INFO] Take a photo...")

            error = 1
            while(error):
                subprocess.run("fswebcam --no-banner -d /dev/video0 -r 1920x1080 --jpeg 100 new.jpg", shell=True)
                prediction_result = prediction()
                image = cv.imread("new.jpg", 0)
                if cv.countNonZero(image) == 0:
                    print("[ERROR] Image is black!")
                else:
                    error = 0

            cv.imshow('Input Image', image)

            x_coord, y_coord = get_center_coords("new.jpg")
            angle = get_rotate_angle(x_coord, y_coord)
            dist = get_distance(x_coord, y_coord)
            tilt_angle = get_tilt_angle(dist)
            print(x_coord, y_coord, angle, (180-angle+offset), dist, tilt_angle)

            #serport.write(("angle=" + str(180-angle+offset)).encode())
            serport.write((str(180-angle+offset)).encode())
            fin0A = b'\x0A'
            serport.write(fin0A)

            time.sleep(0.3)
            serport.write((str(tilt_angle)).encode())
            fin0A = b'\x0A'
            serport.write(fin0A)

            time.sleep(0.3)

            serport.write(prediction_result.encode())
            fin0A = b'\x0A'
            serport.write(fin0A)

            buff = ""

            while True:
                oneByte = serport.read(1)
                if oneByte == b"\r":
                    break
                else:
                    buff += oneByte.decode("ascii", "ignore")

            print(buff)

            buff = ""

            print("[INFO] Waiting for event from manipulator...")
            continue


if __name__ == "__main__":
	main()
