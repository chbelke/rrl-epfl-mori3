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
import random

from termcolor import colored

from runMqtt.recieveMessage import *
from Settings import names


class PingHandler(threading.Thread):

    def __init__(self, wifi_host):
        threading.Thread.__init__(self)
        self.event = threading.Event()
        self.wifi_host = wifi_host
        self.pingDict = {}
        self.pingCount = {}
        self.pingBusy = {}
        self.pingResults = {}
        self.newBatchFlag = True        
        self.ping_thread = None
        self.dir = "PingData/Data/"
        self.dataLogFile=None
        self.waitForBulk = False
        self.randList = list(range(0x01,0xFF))
        self.randList.remove(14)
        self.randList.remove(42)
        print(self.randList)
        
    def run(self):
        self.thread_terminate = False
        self.ping_thread = threading.Thread(target=self.checkPing)
        self.ping_thread.daemon = True
        self.ping_thread.start()    


    def checkPing(self):
        run=True
        while not self.event.is_set():
            i = 1
            try:
                for esp in self.pingCount.keys():
                    if self.pingCount[esp] <= 0:
                        continue

                    if not esp in self.pingBusy: # if it's the first ping we're sending
                        self.sendPing(esp)
                        self.pingBusy[esp] = True
                    elif not self.pingBusy[esp]: # we already sent a ping and we're no longer busy so the ping returned
                        self.pingCount[esp] -= 1 # decrement
                        if self.pingCount[esp] > 0:
                            if self.waitForBulk and not all(busy == False for busy in self.pingBusy.values()):
                                continue
                            self.sendPing(esp)
                            self.pingBusy[esp] = True
                    elif time.perf_counter() - self.getTsPingDict(esp) > 2:
                        print(colored("TIMEOUT: "+ names.idsToName[esp], "red"))
                        self.pingBusy[esp] = False
                        self.addPingResult(esp, np.inf, False)
                        self.pingCount[esp] -= 1 # decrement

                    if self.pingCount[esp] == 0:
                        # arr1 = np.array(self.pingResults[esp]["time"]) #save data to file
                        # arr2 = np.array(self.pingResults[esp]["integrity"])
                        # dtString = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S")
                        # if self.newBatchFlag:
                        #     self.dir = "Ping Data/Data/Batch_" + dtString
                        #     if not os.path.exists(self.dir):
                        #         os.makedirs(self.dir)
                        #         #print("\nNew batch, Made directory: " + self.dir) #debug
                        #     self.newBatchFlag = False
                        #     #print("Batch flag set to false\n") #debug
                        # np.savez(self.dir + "/PD_" + esp + "_" + dtString, arr1, arr2)
                        print(colored("Ping data for " + esp + " saved!", "green"))
                        # self.pingBusy.pop(esp) #remove key
                        # if len(self.pingBusy) == 0: #End of batch
                        #     self.newBatchFlag = True
                            #print("\nBatch flag set to true") #debug
            except:
                print(colored("IN TRACEBACK", 'red'))
                traceback.print_exc()
            self.event.wait(0.5)    



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

    def waitForAll(self, value):
        self.waitForBulk = value


    def addPingResult(self, number, time, dataIntegrity):
        if not number in self.pingResults: # no entry for the esp, initalize dict
            self.pingResults[number] = {}
            self.pingResults[number]["time"] = list()
            self.pingResults[number]["integrity"] = list()

        self.pingResults[number]["time"].append(time)
        self.pingResults[number]["integrity"].append(dataIntegrity)

        txt = str(number) + ", " + str(time) + ", " + str(dataIntegrity) + "\n"
        self.writePingFile(txt)
        return # Ping data can't be saved without return


    def clearPingResults(self, number):
        self.pingResults[number]["time"].clear()
        self.pingResults[number]["integrity"].clear()


    def sendPing(self, number):
        #print("pinging {} with 32 bytes of data...".format(number)) # for debugging
        data = bytearray()
        for lv in range(32):  # generate 32 bytes of random data
        # for lv in range(16):  # generate 32 bytes of random data
            # data.append(random.randint(0x01,0xFF)) # avoid NULL characters
            data.append(random.choice(self.randList)) # avoid NULL characters
        
        text = bytearray(str.encode("png ")) # build a message with a command for the esp and the random data
        text.extend(data)
        # print(text) # for debugging
        self.setPingDict(number, time.perf_counter(), data) # take note of the time and the randomly generated data
        self.wifi_host.publishLocal(text, number)            


    def createPingFile(self):
        if not os.path.exists(self.dir):
            os.makedirs(self.dir)
        dtString = datetime.datetime.now().strftime("%Y-%m-%d_%H%M%S")
        self.fileName = self.dir + dtString + ".csv"
        self.pingLogFile=open(self.fileName, 'w')
        header = "Name, Time, Integrity\n"
        self.writePingFile(header)


    def writePingFile(self, line):
        try:
            self.pingLogFile.write(line)
        except AttributeError:
            return
        except ValueError:
            return

    def closePingFile(self):
        self.pingLogFile.close()            

    def exit(self):
        self.usp_rec.exit()
        self.udp_sock.close()