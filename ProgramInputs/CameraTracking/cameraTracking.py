'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Tracks Mori via external camera, sends updates for them to connect.

**********************************************************************************
'''


import serial, time
import datetime
import sys, select, os
from termcolor import colored

import threading


def main(args):
    if args.mqtt == True:
        import paho.mqtt.client as mqtt

    if args.camera == True:
        import something

    if args.target == True:
        selected_mori, target_mori = getWhichModulesToConnect()

    startTime = time.time()

    if args.mqtt == True:
        mqttClient = MqttHost()
        mqttClient.run()


    while(True):
        img = extractImage(args)
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


def extractImage(args):
    if args.camera == True:
        img = getCameraImage()
    else:
        img = getSavedImage()
    return img


def getCameraImage():
    return


def getSavedImage():
    return


def getMoriPoints(img):
    mori_vertices = []
    batteries = detectBatteres()
    for battery in batteries:
        mask = generateMaskFromBattery(battery)
        leds = findVertexLEDs(mask)
        vertices = generateMoriVertices(leds)
        mori_vertices.append(vertices)
    return mori_vertices


def detectBatteries():
    return


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