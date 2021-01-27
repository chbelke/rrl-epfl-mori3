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
import random as rng
from termcolor import colored


BATTERY_WIDTH_MIN = 30
BATTERY_WIDTH_MAX = 120
BATTERY_HEIGHT_MIN = 10
BATTERY_HEIGHT_MAX = 40

LOWER_COLOUR = np.array([78, 185, 0], dtype=np.uint8)
UPPER_COLOUR = np.array([99,255,255], dtype=np.uint8)
LOWER_COLOUR_SOFT = np.array([50,0,0], dtype=np.uint8)
UPPER_COLOUR_SOFT = np.array([150,255,255], dtype=np.uint8)

LOWER_COLOUR_SOFT = LOWER_COLOUR
UPPER_COLOUR_SOFT = UPPER_COLOUR



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
        cv2.namedWindow('img_contour')


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

    
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mask = cv2.inRange(hsv, LOWER_COLOUR, UPPER_COLOUR)
    img_colour = cv2.bitwise_and(hsv,hsv, mask=mask)    
    hsv = cv2.blur(img_colour, (3,3))

    threshold = 100

    # kernel = np.ones((3,3), dtype=np.uint8)
    kernel = np.ones((4,4), dtype=np.uint8)
    closing = cv2.morphologyEx(hsv, cv2.MORPH_CLOSE, kernel)

    canny_output = cv2.Canny(hsv, threshold, threshold * 2)

    contours, _ = cv2.findContours(canny_output, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    minRect = [None]*len(contours)
    for i, c in enumerate(contours):
        minRect[i] = cv2.minAreaRect(c)

    drawing = np.zeros((canny_output.shape[0], canny_output.shape[1], 3), dtype=np.uint8)

    for rectangle in minRect:

        # rotated rectangle
        box = cv2.boxPoints(rectangle)
        box = np.intp(box) #np.intp: Integer used for indexing (same as C ssize_t; normally either int32 or int64)
        cv2.drawContours(drawing, [box], 0, (255, 255, 255), 1)
        if not checkRectangleSize(rectangle[1]):
            continue

        mask = np.zeros((canny_output.shape[0], canny_output.shape[1], 1), dtype=np.uint8)
        cv2.fillConvexPoly(mask, box, 255)
        mean_colour = cv2.mean(hsv,mask)[0:3]
        cv2.drawContours(drawing, [box], 0, mean_colour)
    
    cv2.rectangle(drawing, (0, 0), (BATTERY_WIDTH_MIN, BATTERY_HEIGHT_MIN), (0, 0, 255), 2)
    cv2.rectangle(drawing, (0, 0), (BATTERY_WIDTH_MAX, BATTERY_HEIGHT_MAX), (255, 0, 0), 2)

    cv2.imshow('img_contour', drawing)
    cv2.imshow('img_colour', hsv)

    print("Done!")

    cv2.imshow('img', img)
    cv2.imshow('mask', canny_output)
    # cv2.imshow('img_colour', img_colour)


    cv2.waitKey(20)
    input()
    



    detectBatteryContours(img)
    input()
    return


def checkRectangleSize(rect_size):
    rect_size = np.sort(rect_size, axis=0)
    if rect_size[0] < BATTERY_HEIGHT_MIN:
        return False
    if rect_size[0] > BATTERY_HEIGHT_MAX:
        return False
    if rect_size[1] < BATTERY_WIDTH_MIN:
        return False
    if rect_size[1] > BATTERY_WIDTH_MAX:
        return False
    return True


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