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

import pyrealsense2 as rs
# from pykalman import KalmanFilter


BATTERY_WIDTH_MIN = 7
BATTERY_WIDTH_MAX = 15
BATTERY_HEIGHT_MIN = 3
BATTERY_HEIGHT_MAX = 7

BATTERY_WIDTH_MIN = 20
BATTERY_WIDTH_MAX = 30
BATTERY_HEIGHT_MIN = 12
BATTERY_HEIGHT_MAX = 18

BATTERY_RATIO = 30
RECTANGLE_MERGE_BOUND = 20

LOWER_COLOUR = np.array([70, 50, 0], dtype=np.uint8)
UPPER_COLOUR = np.array([85,255,255], dtype=np.uint8)

LED_RING_SIZE_MIN = 20
LED_RING_SIZE_MAX = 30

INIT_STEPS = 25

camera_w = 1920
camera_h = 1080

camera_w = int(1920)
camera_h = int(1080)

scale_percent = 75

BATTERY_WIDTH_MIN = int(BATTERY_WIDTH_MIN * scale_percent / 100)
BATTERY_WIDTH_MAX = int(BATTERY_WIDTH_MAX * scale_percent / 100)
BATTERY_HEIGHT_MIN = int(BATTERY_HEIGHT_MIN * scale_percent / 100)
BATTERY_HEIGHT_MAX = int(BATTERY_HEIGHT_MAX * scale_percent / 100)

LED_RING_SIZE_MIN = int(LED_RING_SIZE_MIN * scale_percent / 100)
LED_RING_SIZE_MAX = int(LED_RING_SIZE_MAX * scale_percent / 100)


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

    if args.realsense == True:
        vc = rs.pipeline()
        config = rs.config()        
        config.enable_stream(rs.stream.color, camera_w, camera_h, rs.format.bgr8, 30)
        vc.start(config)
        camera_params = None

    else:
        vc = None


    startTime = time.time()
    tracked_modules = {}

    if args.mqtt == True:
        mqttClient = MqttHost()
        mqttClient.run()


    # for i in range(INIT_STEPS):
    while(True):
        img = extractImage(args, vc, camera_params)
        batteries = getMoriPoints(img)
        tracked_modules = runKalmanOnBatteries(img, batteries, tracked_modules)
        key = cv2.waitKey(20)
        if key == 27: # exit on ESC
            break

    print("Done!")
    while(True):
        key = cv2.waitKey(20)
        if key == 27: # exit on ESC
            break


    while(True):
        img = extractImage(args, vc, camera_params)
        mori_vertices = getMoriPoints(img)
        continue
        

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
    elif args.realsense == True:
        img = getRealsenseImage(vc, camera_params)
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


def getRealsenseImage(vc, camera_params):
    while True:
        frames = vc.wait_for_frames()
        # depth_frame = frames.get_depth_frame()
        color_frame = frames.get_color_frame()    
        if color_frame:
            break
    img = np.asanyarray(color_frame.get_data())

    w = int(img.shape[1] * scale_percent / 100)
    h = int(img.shape[0] * scale_percent / 100)
    dim = (w, h)
      
    # resize image
    img = cv2.resize(img, dim, interpolation = cv2.INTER_AREA)    

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
        leds = findVertexLEDs(img, battery)
        # vertices = generateMoriVertices(leds)
        # mori_vertices.append(vertices)
    cv2.waitKey(20)        
    return batteries
    return mori_vertices


def detectBatteries(img):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    canny_output = detectCannyEdges(hsv)
    contours, _ = cv2.findContours(canny_output, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)


    drawing = np.zeros((canny_output.shape[0], canny_output.shape[1], 3), dtype=np.uint8)

    rectangle_list = getRectangleList(drawing, contours, hsv, canny_output)

    rectangle_list = pruneRectangleList(rectangle_list, drawing)

    for ele in rectangle_list:
        box = cv2.boxPoints(ele)
        box = np.intp(box) #np.intp: Integer used for indexing (same as C ssize_t; normally either int32 or int64)
        cv2.drawContours(img, [box], 0, (0, 0, 255), 1)        

    
    cv2.rectangle(drawing, (0, 0), (BATTERY_WIDTH_MIN, BATTERY_HEIGHT_MIN), (0, 0, 255), 2)
    cv2.rectangle(drawing, (0, 0), (BATTERY_WIDTH_MAX, BATTERY_HEIGHT_MAX), (255, 0, 0), 2)

    # cv2.imshow('img_colour', hsv)

    cv2.imshow('img_contour', drawing)
    # cv2.imshow('img', img)
    # cv2.imshow('mask', canny_output)


    return rectangle_list


def detectCannyEdges(hsv):
    mask = cv2.inRange(hsv, LOWER_COLOUR, UPPER_COLOUR)
    img_colour = cv2.bitwise_and(hsv,hsv, mask=mask)    
    # hsv = cv2.blur(img_colour, (3,3))
    hsv = cv2.blur(img_colour, (2,2))
    # hsv = img_colour

    threshold = 100

    kernel = np.ones((3,3), dtype=np.uint8)
    # kernel = np.ones((4,4), dtype=np.uint8)
    closing = cv2.morphologyEx(hsv, cv2.MORPH_CLOSE, kernel)

    return cv2.Canny(hsv, threshold, threshold * 2)


def getRectangleList(drawing, contours, hsv, canny_output):
    minRect = [None]*len(contours)
    for i, c in enumerate(contours):
        minRect[i] = cv2.minAreaRect(c)
    rectangle_list = []

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
        rectangle_list.append(rectangle)
    return rectangle_list


def pruneRectangleList(rectangle_list, drawing):
    purge_list = []
    new_rectangles = []
    for i, rectangle in enumerate(rectangle_list):
        for j, other_rectangle in enumerate(rectangle_list):
            if rectangle == other_rectangle:
                continue
            if getDistanceXY(rectangle[0], other_rectangle[0]) < 10:
                loc = (np.mean([rectangle[0][0], other_rectangle[0][0]]),
                        np.mean([rectangle[0][1], other_rectangle[0][1]]))
                h_w = (np.max([rectangle[1][0], other_rectangle[1][0]]),
                        np.max([rectangle[1][1], other_rectangle[1][1]]))
                ang = np.mean([rectangle[2], other_rectangle[2]])
                tmp = (loc, h_w, ang)

                box = cv2.boxPoints(tmp)
                box = np.intp(box) #np.intp: Integer used for indexing (same as C ssize_t; normally either int32 or int64)
                cv2.drawContours(drawing, [box], 0, (0, 0, 255), 1)
                purge_list.append(i)
                purge_list.append(j)
                new_rectangles.append(tmp)

    for ele in sorted(set(purge_list), reverse = True):  
        del rectangle_list[ele] 

    rectangle_list.extend(set(new_rectangles))
    
    return rectangle_list


def runKalmanOnBatteries(img, batteries, tracked_modules):
    tracked_modules = updateTracked_modules(batteries, tracked_modules)
    for module in tracked_modules:
        print(tracked_modules[module]["battery_position"])
        pos_x = int(tracked_modules[module]["battery_position"][0][0])
        pos_y = int(tracked_modules[module]["battery_position"][0][1])
        cv2.putText(img, str(module), (pos_x, pos_y), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)
    cv2.imshow('img', img)
    return tracked_modules


def updateTracked_modules(batteries, tracked_modules):
    if len(batteries) == 0:# and len(tracked_modules) == 0:
        return tracked_modules

    err_matrix = np.ones((len(tracked_modules), len(batteries)))*1000
    
    for i in range(len(tracked_modules)):
        if "battery_position" not in tracked_modules[i].keys():
            continue
        pos = tracked_modules[i]["battery_position"]
        for j, battery in enumerate(batteries):
            err_pos = getDistanceXY(pos[0], battery[0])
            err_size = getDistanceXY(pos[1], battery[1])
            err_rot = np.abs(pos[2] - battery[2])
            err_matrix[i, j] = err_pos + err_size + err_rot

    print(err_matrix)

    closest_batteries = [None]*len(tracked_modules)
    purge_tracked = []
    for i in range(len(tracked_modules)):
        tmp_smallest = (-1, 1000) # [ID, value]
        for j, battery in enumerate(batteries):
            if err_matrix[i,j] < 10.0:
                if err_matrix[i,j] < tmp_smallest[1]:
                    tmp_smallest = (j, err_matrix[i,j])
        for key, value in tracked_modules[i].items():
            if "battery_position" not in tracked_modules[i].keys():
                continue
        if tmp_smallest[0] == -1:
            purge_tracked.append(i)
        else:
            closest_batteries[i] = tmp_smallest[0]
            tracked_modules[i]["battery_pos_old"] = tracked_modules[i]["battery_position"]
            tracked_modules[i]["battery_position"] = batteries[tmp_smallest[0]]
            tracked_modules[i]["error"] = tmp_smallest[1]

    print(closest_batteries)

    used_batteries = [bat for bat in set(closest_batteries)]
    unused_batteries = [bat for bat in range(len(batteries)) if bat not in used_batteries]

    print("used, unused")
    print(used_batteries,unused_batteries)
    print("tracked before", tracked_modules)
    # print("batteries", batteries)

    for purge in purge_tracked:
        del tracked_modules[purge]

    for i in range(len(batteries)):
        if i not in tracked_modules:
            tracked_modules[i] = {}        
            tracked_modules[i]["battery_position"] = batteries[unused_batteries.pop(0)]
            print(tracked_modules[i]["battery_position"])
            tracked_modules[i]["confirmed"] = False

    # for i in range(len(tracked_modules)):
    #     if "battery_position" in tracked_modules[i].keys():
    #         continue

    print("tracked after", tracked_modules)

    return tracked_modules



def getDistanceXY(point1, point2):
    return np.sqrt((point2[0]-point1[0])**2 + (point2[1]-point1[1])**2)


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
    if rect_size[1] / rect_size[0] > BATTERY_RATIO:
        return False

    return True


def findVertexLEDs(img, battery):
    mask = generateMaskFromBattery(img, battery)
    leds = findLEDs(img, battery, mask)
    return


def generateMaskFromBattery(img, battery):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    mask = np.zeros((img.shape[0], img.shape[1], 1), dtype=np.uint8)
    cv2.circle(mask, (int(battery[0][0]), int(battery[0][1])), LED_RING_SIZE_MAX, 255, -1)
    cv2.circle(mask, (int(battery[0][0]), int(battery[0][1])), LED_RING_SIZE_MIN, 0, -1)
    cv2.imshow('mask', mask)    
    return mask


def findLEDs(img, battery, mask):
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)
    screened_img = cv2.bitwise_and(hsv,hsv, mask=mask)   
    cv2.imshow('img_colour', screened_img)
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
        help='connects to a pseye',
        action='store_true')
    parser.add_argument('-r', '--realsense',
        help='connects to a realsense',
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