'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Tracks Mori via external camera, sends updates for them to connect.

**********************************************************************************
'''


import time
import sys, select, os

import cv2
import numpy as np
import threading
import argparse
from termcolor import colored


BATTERY_WIDTH_MIN = 20
BATTERY_WIDTH_MAX = 60
BATTERY_HEIGHT_MIN = 20
BATTERY_HEIGHT_MAX = 60



def main(args):
    if args.mqtt == True:
        import paho.mqtt.client as mqtt

    if args.target == True:
        selected_mori, target_mori = getWhichModulesToConnect()

    if args.camera == True:
        vc = cv2.VideoCapture(0)
        if vc.isOpened(): # try to get the first frame
            # rval, frame = vc.read()
            print("Camera is available")
        camera_params = getPSEYEUndistortParameters()
        cv2.namedWindow('img')
        cv2.namedWindow('mask')
        cv2.namedWindow('img_colour')


    else:
        vc = None


    startTime = time.time()

    if args.mqtt == True:
        mqttClient = MqttHost()
        mqttClient.run()


    while(True):
        img = extractImage(args, vc, camera_params)
        mori_vertices = getMoriPoints(img)
        

        if selected_mori is not None:
            mori_paths = calculateTargetPath(args, mori_vertices, selected_mori, target_mori)
        else:
            mori_paths = calculateMoriPaths(args, mori_vertices)

        if args.mqtt == True:
            publishPaths(mqttClient, mori_paths)

        if args.camera == False:
            break

        couple_edges = modulesToCouple(mori_vertices)
        if couple_edges is not None:
            startCoupling(couple_edges)


def getWhichModulesToConnect():
    return


def extractImage(args, vc, camera_params):
    if args.camera == True:
        img = getCameraImage(vc, camera_params)

    else:
        img = getSavedImage()
    return img


def getCameraImage(vc, camera_params):
    while True:
        rval, img = vc.read()
        key = cv2.waitKey(20)
        if key == 27: # exit on ESC
            sys.exit()
        if rval:
            break
    img = undistortImage(img, camera_params)
    
    # tmp to find first image
    # cv2.imshow('img', img)
    # key = cv2.waitKey(20)
    # input()
    return img


def undistortImage(img, camera_params):
    #Undistorts the image
    img = cv2.undistort(img, camera_params["mtx"], camera_params["dist"], None, camera_params["newcameramtx"])
    
    #----------- Causes issues with HSV------------------#
    #sets the image bounds to the undistorted area
    # img = (img[camera_params["roi"][1]:camera_params["roi"][1]+camera_params["roi"][3], 
    #     camera_params["roi"][0]:camera_params["roi"][0]+camera_params["roi"][2]])
    return img




def getPSEYEUndistortParameters():
    mtx = np.zeros((3,3))
    mtx[0,0] = 746.4909984007513
    mtx[1,1] = 746.641656940557
    mtx[0,2] = 304.534352530265
    mtx[1,2] = 247.27130406293284
    mtx[2,2] = 1.0

    dist = np.array([-0.20507558,  0.84151962,  0.00276314, -0.00344598, -1.6605042 ],  dtype=np.float32)
    
    w = 640
    h = 480

    newcameramtx, roi = cv2.getOptimalNewCameraMatrix(mtx, dist, (w,h), 1, (w,h))
    # x, y, w, h = roi
    
    camera_params = {}
    camera_params["mtx"] = mtx
    camera_params["dist"] = dist
    camera_params["newcameramtx"] = newcameramtx
    camera_params["roi"] = roi
    return camera_params


def getSavedImage():
    return


def getMoriPoints(img):
    mori_vertices = []
    batteries = detectBatteries(img)
    for battery in batteries:
        mask = generateMaskFromBattery(battery)
        leds = findVertexLEDs(mask)
        vertices = generateMoriVertices(leds)
        mori_vertices.append(vertices)
    return mori_vertices


def detectBatteries(img):
    # grey = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # _, mask = cv2.threshold(grey, 8, 255, cv2.THRESH_BINARY)

    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    #color set from "setHSVThreshold.py"
    #Currently thresholded around green
    lower_colour= np.array([28, 40, 0], dtype=np.uint8)
    upper_colour= np.array([90,255,240], dtype=np.uint8)

    mask = cv2.inRange(hsv, lower_colour, upper_colour)
    # mask = cv2.cvtColor(mask,cv2.COLOR_BGR2GRAY)
    # ret, mask = cv2.threshold(mask, 00, 255, cv2.THRESH_BINARY)    
    print(mask[0][0:10:-1])

    img_colour = cv2.bitwise_and(img,img, mask=mask)

    cnts = cv2.findContours(mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)[-2]  # Use index [-2] to be compatible to OpenCV 3 and 4
    for c in cnts:
        x, y, w, h = cv2.boundingRect(c)

        #rectangle too smol
        if w < BATTERY_WIDTH_MIN and h < BATTERY_HEIGHT_MIN:
            continue

        #rectangle big chungus
        if w > BATTERY_WIDTH_MAX and h > BATTERY_HEIGHT_MAX:
            continue
        cv2.rectangle(img, (x, y), (x+w, y+h), (0, 255, 0), thickness = 10)

        #center of image
        level = img[y+h//2, x+w//2]
        # define range of blue color in HSV

    print("Done!")

    cv2.imshow('img', img)
    cv2.imshow('mask', mask)
    cv2.imshow('img_colour', img_colour)


    cv2.waitKey(20)
    input()
    



    detectBatteryContours(img)
    input()
    return


def detectBatteryContours(img):
    # Convert logical matrix to uint8
    gray = gray.astype(np.uint8)*255

    # Find contours
    cnts = cv2.findContours(thresh, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_NONE)[-2]  # Use index [-2] to be compatible to OpenCV 3 and 4

    # Get contour with maximum area
    c = max(cnts, key=cv2.contourArea)

    x, y, w, h = cv2.boundingRect(c)
    cv2.imshow('img', img)
    cv2.waitKey(0)
    return x, y, w, h


def generateMaskFromBattery(battery):
    return


def findVertexLEDs(mask):
    return


def generateMoriVertices(leds):
    return


def calculateMoriPaths(args, mori_vertices):
    return


def startCoupling(couple_edges):
    return


# Keep at bottom (runs program)
if __name__ == '__main__':

    parser = argparse.ArgumentParser()
    parser.add_argument('-m', '--mqtt',
        help='runs with mqtt',
        action='store_true')
    parser.add_argument('-c', '--camera',
        help='connects to a camera',
        action='store_true')
    parser.add_argument('-t', '--target',
        help='defines if a single module needs to connect to a target')
    args = parser.parse_args()    

    sys.exit(main(args))



# def getMaskImage(args):
#     if args.camera == True:
#         mask = FROM_FILE
#     else:
#         mask = FROM_OTHER_FILE
#     return mask