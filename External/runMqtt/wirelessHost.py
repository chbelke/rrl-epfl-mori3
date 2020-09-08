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
import random
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

        self.pingDict = {}
        self.pingCount = {}
        self.pingBusy = {}
        self.pingResults = {}
        self.newBatchFlag = True

        self.version = 0.50

        self.mqttClient = MqttHost(self)
        self.udpClient = UDPHost(self)


    def run(self):
        print("Wireless Host: ", threading.get_ident())
        startTime = time.time()
        loopTime = time.time()

        self.mqttClient.run()
        self.udpClient.usp_rec.run()

        while True:
            self.checkPing()

        while not self.event.is_set():
            if time.time() - loopTime > 2:
                print("\nTime Elapsed: {:.2f}".format(loopTime-startTime))
                print(colored("Referenced ESPs : ","yellow"), end=' ')
                print(self.macDict, end="\n\n")
                loopTime = time.time()

            self.event.wait(0.1)        


# #--------------------------------------------------------------#
# #---------------------------- UDP -----------------------------#
# #--------------------------------------------------------------#
#     def intializeUDPMulticast(self):
#         self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
#         self.udp_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
#         self.udp_sock.bind(('', self.udp_port))
#         mreq = struct.pack("=4sl", socket.inet_aton("224.51.105.104"), socket.INADDR_ANY)

#         self.udp_sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)        



    def publishGlobal(self, msg):
        self.mqttClient.publishGlobal(msg)
        for hub in self.UDPDict:
            self.udpClient.write(msg, hub)        


    def publishLocal(self, msg, addr):
        if addr in self.UDPDict:
            self.udpClient.write(msg, addr)        
        else:
            self.mqttClient.publishLocal(msg, addr)


    def getNumberConnected(self):
        # self.numberModules = len(self.macOrder)
        # return self.numberModules
        # print("IPDICT: ", self.IPDict)
        if self.macOrder is None:
            return 0

        if not self.coTimeDict:
            return 0

        for i in range(len(self.macOrder)-1, -1, -1): #Go through the array  from top to bottom to avoid out ouf range errors
            tmpTime = self.coTimeDict.get(self.macOrder[i])
            if not tmpTime:
                return 0
            if time.time() - tmpTime > 4: #Consider ESP disonnected if no message has been received in the last 4 seconds
                print(colored("ESP " + self.macOrder[i] + " lost", "red"))
                del self.macOrder[i]
        self.numberModules = len(self.macOrder)
        return self.numberModules


    # def toggleUDP(self):
    #     if self.UDPCom:
    #         self.UDPCom = False
    #         #Stop the UDP connections
    #         for i in range(len(self.controllerDict)):
    #             self.publishLocal(list(self.controllerDict)[i],"stopudp")
    #     else:
    #         self.UDPCom = True
    #     return self.UDPCom

    def checkPing(self):
        try:
            if self.newBatchFlag:
                self.dir = "Ping Data/Data/Batch_" + datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S")
                if not os.path.exists(self.dir):
                    os.mkdir(self.dir)
                self.newBatchFlag = False
                
            for esp in self.pingCount.keys():
                if self.pingCount[esp] <= 0:
                    continue

                if not self.pingBusy.setdefault(esp, False): # If the esp has no entry in the dict default to not busy
                    self.sendPing(esp)
                    self.pingBusy[esp] = True

                if self.getTsPingDict(esp) - time.perf_counter() > 1:
                    print("TIMEOUT", esp)
                    self.pingBusy[esp] = False
                    #save timeout TODO ask Kevin
                    self.pingCount[esp] -= 1
                if self.pingCount[esp] == 0:
                    print("\nHERE\n") #debugging
                    arr1 = np.array(self.pingResults[esp]["time"]) #save data to file
                    arr2 = np.array(self.pingResults[esp]["integrity"])
                    dt_string = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S")
                    np.savez(self.dir + "/PD_" + esp + "_" + dt_string, arr1, arr2)
                    print(colored("Ping data for " + esp + " saved!", "green"))
                    del self.pingBusy[esp] #remove key
                    if len(self.pingBusy) == 0: #End of batch
                        self.newBatchFlag = True
        except:
            print(colored("IN TRACEBACK", 'red'))
            traceback.print_exc()
        return


    def setIPDict(self, IPDict, EPDict):
        self.EPDict = EPDict


    def setUDPDict(self, UDPDict):
        self.UDPDict = UDPDict

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

    def setPingCount(self, number, count):
        self.pingCount[number] = count

    def resetPingBusy(self, number):
        self.pingBusy[number] = False

    def getTsPingDict(self, number):
        return self.pingDict[number]["timestamp"]

    def getDataPingDict(self, number):
        return self.pingDict[number]["data"]

    def addPingResult(self, number, time, dataIntegrity):
        if not number in self.pingResults: # no entry for the esp, initalize dict
            self.pingResults[number] = {}
            self.pingResults[number]["time"] = list()
            self.pingResults[number]["integrity"] = list()

        self.pingResults[number]["time"].append(time)
        self.pingResults[number]["integrity"].append(dataIntegrity)
        return # Ping data can't be saved without return

    def clearPingResults(self, number):
        self.pingResults[number]["time"].clear()
        self.pingResults[number]["integrity"].clear()

    def sendPing(self, number):
        #print("pinging {} with 32 bytes of data...".format(number)) # for debugging
        data = bytearray()
        for lv in range(32):  # generate 32 bytes of random data
            data.append(random.randint(0x01,0xFF)) # avoid NULL characters
        
        text = bytearray(str.encode("png ")) # build a message with a command for the esp and the random data
        text.extend(data)
        # print(text) # for debugging
        self.setPingDict(number, time.perf_counter(), data) # take note of the time and the randomly generated data
        self.publishLocal(text, number)    
 

    def exit(self):
        self.mqttClient.exit()
        self.udpClient.exit()
        self.event.set()