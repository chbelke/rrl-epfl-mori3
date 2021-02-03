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
import random
from termcolor import colored

w = 640
h = 480

min_dist = 40
max_dist = 50

XDIM = w
YDIM = h
windowSize = [XDIM, YDIM]
rectObs = []

GOAL_RADIUS = 10
MIN_DISTANCE_TO_ADD = 1.0
NUMNODES = 500
delta = 30.0


white = 255, 255, 255
black = 0, 0, 0
red = 0, 0, 255
green = 0, 255, 0
blue = 255, 0, 0
cyan = 0,180,105
dark_green = 0, 102, 0

currentState = 'buildTree'
count = 0

class Node(object):
    def __init__(self, point, parent):
        super(Node, self).__init__()
        self.point = point
        self.parent = parent


def main():
    cv2.namedWindow('img')
    cv2.namedWindow('mask')
    cv2.namedWindow('img_colour')
    cv2.namedWindow('img_contour') 



    startTime = time.time()


    mori_list = getTriangleLocations()

    while(True):
        img = np.ones((h, w, 3), dtype=np.uint8) * 255
        drawMoriLocations(img, mori_list)
        cv2.imshow('img', img)
        cv2.waitKey(20)  
        input()
        calculateGoals(img, mori_list)
        cv2.imshow('img', img)
        cv2.waitKey(20)  
        continue

        moveMori()


def getTriangleLocations():
    mori_list = []
    for i in range(2):
        pt1 = (random.randint(50, w-50), random.randint(50, h-50))
        while(True):
            pt2 = (random.randint(50, w-50), random.randint(50, h-50))
            dist1 = getDist(pt1, pt2)
            if dist1 > max_dist or dist1 < min_dist:
                continue
            pt3 = (random.randint(50, w-50), random.randint(50, h-50))
            dist2 =  getDist(pt1, pt3)
            if dist2 > max_dist or dist2 < min_dist:
                continue
            dist3 = getDist(pt2, pt3)
            if dist3 > max_dist or dist3 < min_dist:
                continue
            break
        mori_list.append(np.array([pt1, pt2, pt3]))

    return mori_list


def getDist(point1, point2):
    return np.sqrt((point2[0]-point1[0])**2 + (point2[1]-point1[1])**2)


def drawMoriLocations(img, mori_list):
    for i, triangle in enumerate(mori_list):
        cv2.drawContours(img, [triangle], 0, (255,0,255), -1)
        cv2.putText(img, str(i), getCentroid(triangle), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 0, 0), 2)


def getCentroid(tri):
    x = int(round((tri[0,0] + tri[1,0] + tri[2,0]) / 3, 2))
    y = int(round((tri[0,1] + tri[1,1] + tri[2,1]) / 3, 2))
    return (x,y)


def calculateGoals(img, mori_list):
    runRRT_Star(img, mori_list)
    return


# Adapted from:
# https://github.com/s880367/RRT/blob/master/rrt_star.py
def runRRT_Star(img, mori_list):
    global count, currentState
    
    initPoseSet = False
    initialPoint = Node(getCentroid(mori_list[0]), None)
    goalPoseSet = False
    goalPoint = Node(getCentroid(mori_list[1]), None)
    print(mori_list[0])

    nodes = []
    nodes.append(initialPoint)


    while (count < NUMNODES):
        count = count+1
        if count < NUMNODES:
            foundNext = False
            while foundNext == False:
                rand = get_random_clear()
                rand2 = get_random_clear()
                rand3 = get_random_clear()
                parentNode = nodes[0]
                for p in nodes:
                    if (dist(p.point,rand) <= dist(parentNode.point,rand)) and (dist(p.point,rand2) <= dist(parentNode.point,rand2)) and (dist(p.point,rand3) <= dist(parentNode.point,rand3)):
                        newPoint = step_from_to(p.point,rand)
                        newPoint2 = step_from_to(p.point,rand2)
                        newPoint3 = step_from_to(p.point,rand3)
                        if collides(newPoint) == False and collides(newPoint2) == False and collides(newPoint3) == False:
                            parentNode = p
                            foundNext = True

            newnode = step_from_to(parentNode.point,rand)
            newnode2 = step_from_to(parentNode.point,rand2)
            newnode3 = step_from_to(parentNode.point,rand3)
            nodes.append(Node(newnode, parentNode))
            nodes.append(Node(newnode2, parentNode))
            nodes.append(Node(newnode3, parentNode))
            cv2.line(img,parentNode.point,newnode, blue)
            cv2.line(img,parentNode.point,newnode2, green)
            cv2.line(img,parentNode.point,newnode3, dark_green)

            if point_circle_collision(newnode, goalPoint.point, GOAL_RADIUS) or point_circle_collision(newnode2, goalPoint.point, GOAL_RADIUS) or point_circle_collision(newnode3, goalPoint.point, GOAL_RADIUS):
                currentState = 'goalFound'

                goalNode = nodes[len(nodes)-1]
                print("GOAL!")

    while(True):
        if currentState == 'goalFound':
            currNode = goalNode.parent
            # print("Goal Reached")           
            while currNode.parent != None:
                cv2.line(img,currNode.point,currNode.parent.point,red, 3)
                currNode = currNode.parent

        cv2.imshow('img', img)
        cv2.waitKey(1)  


def dist(p1,p2):     #distance between two points
    return np.sqrt((p1[0]-p2[0])*(p1[0]-p2[0])+(p1[1]-p2[1])*(p1[1]-p2[1]))


def get_random_clear():
    while True:
        p = random.random()*XDIM, random.random()*YDIM
        return p
        noCollision = collides(p)
        if noCollision == False:
            return p


def collides(p):    #check if point collides with the obstacle
    for rect in rectObs:
        if rect.collidepoint(p) == True:
            return True
    return False


def step_from_to(p1,p2):
    if getDist(p1,p2) < delta:
        return (int(p2[0]), int(p2[1]))
    else:
        theta = np.arctan2(p2[1]-p1[1],p2[0]-p1[0])
        return int(p1[0] + delta*np.cos(theta)), int(p1[1] + delta*np.sin(theta))


def point_circle_collision(p1, p2, radius):
    distance = dist(p1,p2)
    if (distance <= radius):
        return True
    return False


# Keep at bottom (runs program)
if __name__ == '__main__':
    sys.exit(main())



# def getMaskImage(args):
#     if args.camera == True:
#         mask = FROM_FILE
#     else:
#         mask = FROM_OTHER_FILE
#     return mask