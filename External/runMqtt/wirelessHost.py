'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Most recent handler of

Scans for modules with SSID "helloThere", and then relays the measured signal 
strength and corresponding MAC address over the network.

Used in conjunction with wifiHandler.py, distanceRanger.py, getData.py, and
mqttAnalyzer.py

Before flashing to ESP8266, please change clientName, publishName, and recieveName
to their appropriate values.

**********************************************************************************
'''


import serial, time
import datetime
import sys, select, os, traceback
import numpy as np
import copy
import threading
import socket
import struct

import paho.mqtt.client as mqtt
from termcolor import colored

from runMqtt.recieveMessage import *
from runMqtt.mqttHost import MqttHost
from runMqtt.udpHost import UDPHost


#udp/w/p100/i192.168.0.53/m01 00111111 0900090009000000-1200120 150


class WirelessHost(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        

        self.numberModules = 0
        self.macDict = {}
        self.idDict = {}
        self.macOrder = []
        self.coTimeDict = {}
        self.data = []
        self.macCallTime = time.time()
        self.verCallTimeDict = {}
        self.verCalledDict = {}
                
        self.moriShapeDict = {}
        self.leaderFollowerDict = {}
        self.leaderFollowerOrder = []
        self.controllerDict = {}
        self.controllerOrder = []


        self.IPDict = {}
        self.EPDict = {}
        self.UDPDict = []
        self.HubDict = []
        self.noWifiDict = []

        self.pingDict = {}
        self.pingCount = {}

        self.version = 0.50

        self.connMatrx = []

        self.mqttClient = MqttHost(self)
        self.udpClient = UDPHost(self)


    def run(self):
        print("Wireless Host: ", threading.get_ident())
        startTime = time.time()
        loopTime = time.time()

        self.mqttClient.run()
        self.udpClient.usp_rec.run()

        # while True:
        #     self.checkPing()

        while not self.event.is_set():
            if time.time() - loopTime > 2:
                print("\nTime Elapsed: {:.2f}".format(loopTime-startTime))
                print(colored("Referenced ESPs : ","yellow"), end=' ')
                print(self.macDict, end="\n\n")
                loopTime = time.time()

                for i in range(len(self.connMatrx)):
                    print(self.connMatrx[i])
                print()
            self.event.wait(0.25)        



    def publishGlobal(self, msg):
        self.mqttClient.publishGlobal(msg)
        for hub in self.UDPDict:
            self.udpClient.write(msg, hub)        


    def publishLocal(self, msg, addr):
        if addr in self.UDPDict:
            self.udpClient.write(msg, addr)        
        else:
            self.mqttClient.publishLocal(msg, addr)


    def updateConnected(self):
        if self.macOrder is None:
            return 0

        if not self.coTimeDict:
            return 0

        for i in range(len(self.macOrder)-1, -1, -1): #Go through the array  from top to bottom to avoid out ouf range errors
            if(self.macDict[self.macOrder[i]][0] == "Lost"):
                continue
            tmpTime = self.coTimeDict.get(self.macOrder[i])
            if not tmpTime:
                return 0
            if time.time() - tmpTime > 4: #Consider ESP disonnected if no message has been received in the last 4 seconds
                print(colored("ESP " + self.macOrder[i] + " lost", "red"))
                self.macDict[self.macOrder[i]][0] = "Lost"
        self.numberModules = len(self.macOrder)


    def getNumberConnected(self):
        return self.numberModules


    def checkPing(self):
        for esp in pingCount.keys():
            if pingCount[esp] <= 0:
                continue
            if pingRecieved[esp]:
                #save ping esp, ts, data
                #ping again
                pingRecieved[esp] = False
                pingCount[esp] -= 1
            elif getTsPingDict[esp] - time.perf_counter() > 1:
                print("TIMEOUT", esp)
                pingRecieved[esp] = False
                #save timeout
                pingCount[esp] -= 1
            if pingCount[esp] == 0:
                #save data
                print("hello")
        return


    def setIPDict(self, IPDict, EPDict): #good example of how we can implement ping with getters and setters
        self.IPDict = IPDict
        self.EPDict = EPDict


    def setUDPDict(self, UDPDict):
        self.UDPDict = UDPDict

    def getHubDict(self):
        return self.HubDict

    def setHubDict(self, HubDict):
        self.HubDict = HubDict

    def getNoWifiDict(self):
        return self.noWifiDict

    def setNoWifiDict(self, noWifiDict):
        self.noWifiDict = noWifiDict

    def getUDPDict(self):
        return self.UDPDict        

    def setCoTimeDict(self, coTimeDict):
        self.coTimeDict = coTimeDict


    def getMoriShape(self):
        return self.moriShapeDict

    def setMoriShapeDict(self, moriShapeDict):
        self.moriShapeDict = moriShapeDict


    def getEspIds(self):
        return self.macDict

    def setMacDict(self, macDict):
        self.macDict = macDict

    def setIdDict(self, idDict):
        self.idDict = idDict 

    def getIdDict(self):
        return self.idDict 


    def getEspOrder(self):
        return self.macOrder

    def setMacOrder(self, macOrder):
        self.macOrder = macOrder


    def getLeaderIds(self):
        return self.leaderFollowerDict

    def getLeaderOrder(self):
        return self.leaderFollowerOrder

    def getControllerIds(self):
        return self.controllerDict

    def getControllerOrder(self):
        return self.controllerOrder

    def getMqttStatus(self):
        return self.mqttClient.Connected

    def setPingDict(self, number, timestamp, data):
        # print("added following keys and values:\n key = {}\n timestamp = {}\n data = {}".format(number, timestamp, data))
        self.pingDict[number] = {}
        self.pingDict[number]["timestamp"] = timestamp
        self.pingDict[number]["data"] = data

    def getTsPingDict(self, number):
        return self.pingDict[number]["timestamp"]

    def getDataPingDict(self, number):
        return self.pingDict[number]["data"]

    def addConnection(self, espNum):
        self.connMatrx.append([espNum, 0, 0, 0])

    def updateConnection(self, espNum, edge, neighbour):
        if int(edge)+1 > 3:
            return
        for i, connId in enumerate(self.connMatrx):
            if connId[0] == espNum:
                row = int(i)
                break
        if 'row' in locals():
            self.connMatrx[row][int(edge)+1] = neighbour

    def getConnMatrix(self):
        return self.connMatrx


    def exit(self):
        self.mqttClient.exit()
        self.udpClient.exit()
        self.event.set()