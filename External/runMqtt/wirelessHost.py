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
from runMqtt.Utils import *


#udp/w/p100/i192.168.0.53/m01 00111111 0900090009000000-1200120 150


class WirelessHost(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        

        self.numberModules = 0
        self.macDict = {}           # {'mac': ['Role', 'ID']}
        self.idDict = {}            # {'ID': 'mac'}
        self.macOrder = []          # ['mac', 'mac', mac]
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
        self.noWifiDict = {}

        self.pingDict = {}
        self.pingCount = {}

        self.version = 0.50

        self.connMatrix = {}

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

                print("------- Connections -------")
                for node in self.connMatrix:
                    print(node, self.connMatrix[node])

                print()
                print(self.noWifiDict)
                
                
                loopTime = time.time()
            self.event.wait(0.25)        



    def publishGlobal(self, msg):
        self.mqttClient.publishGlobal(msg)
        for hub in self.UDPDict:
            self.udpClient.write(msg, hub)        


    def publishLocal(self, msg, addr):
        if addr in self.noWifiDict:
            self.publishThroughHub(msg, addr)
        if addr in self.UDPDict:
            self.udpClient.write(msg, addr)        
        else:
            self.mqttClient.publishLocal(msg, addr)


    def publishThroughHub(self, msg, addr):
        if addr not in self.noWifiDict:
            self.updateNoWifiDict(addr)
        if self.noWifiDict[addr][2] == False:
            print(colored("ERROR: no path to hub for " + addr, 'red'))
            return
        msg = self.buildRelayMessage(addr, msg)
        host = self.noWifiDict[addr][0]

        self.publishLocal(msg, host)

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
                self.espLost(self.macDict[self.macOrder[i]][1])
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
        self.connMatrix[espNum] = [0, 0, 0]

    def updateConnection(self, espNum, edge, neighbour):
        if int(edge) > 3:
            return
        try:
            self.connMatrix[espNum][int(edge)] = neighbour
        except KeyError:
            return

    def getConnMatrix(self):
        return self.connMatrix


    def shortestPath(self, target):
        path = []
        for hub in self.HubDict:
            tmp = BFS(self, hub, target, self.connMatrix)
            if len(path) == 0:
                path = tmp
            elif len(tmp) < len(path):
                path = tmp
        edge_path = []
        if path != False:
            for i in range(len(path)-1):
                edge = self.connMatrix[path[i]].index(path[i+1])
                edge_path.append(edge)
        return path, edge_path


    def buildRelayMessage(self, espNum, msg):
        msg = bytearray(str.encode(msg))
        msg_len = len(msg)
        esp_alloc = 0b00100000 | (msg_len)
        msg_len += 1   #esp_alloc
        msg_len += len(self.noWifiDict[espNum][2])    #rel
        msg_len += 3    #rel

        message = bytearray(str.encode("rel "))
        for i, edge in enumerate(self.noWifiDict[espNum][1]):
            alloc = int(0b11100000)
            print(i, edge)
            print(int(edge))
            alloc = alloc | int(edge)
            message.append(alloc)
            if i ==0:
                message.append(int(msg_len))

        message.append(0b11100011)      #Relay to ESP
        message.append(esp_alloc)      #ESP command to interpret + length
        message.extend(msg)
        message.append(0b00001110)      #14 (esp end)
        message.append(0b00101010)      #42 (coupling end)
            
        print(msg_len)
        print(message)
        for i in message:
            print(hex(i), end=' ')
        print()

        return message


    def espUDP(self, espNum):
        print(colored("UDP enabled for: " + espNum, 'green'))
        self.macDict.get(self.idDict[espNum])[0] = "UDP"

    def espLost(self, espNum):
        print(colored("ESP " + espNum + " lost", "red"))
        self.macDict.get(self.idDict[espNum])[0] = "Lost"

    def espMQTT(self, espNum):
        self.macDict.get(self.idDict[espNum])[0] = "WiFi"


    def updateNoWifiDict(self, espNum):
        path, edge_path = self.shortestPath(espNum)
        target_hub = path[0]
        self.noWifiDict[espNum] = [target_hub, edge_path, path]

    def espLostCheck(self, espNum):
        try:
            if self.macDict.get(self.idDict[espNum])[0] == "Lost":
                return True
            return False
        except KeyError:
            return False



    def exit(self):
        self.mqttClient.exit()
        self.udpClient.exit()
        self.event.set()