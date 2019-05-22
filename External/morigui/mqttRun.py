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
import sys, select, os
# from argparse import ArgumentParser
import argparse
import numpy as np
import paho.mqtt.client as mqtt
import copy
# from scipy.optimize import minimize, least_squares
# import matplotlib.pyplot as plt
from termcolor import colored
import threading



class MqttHost(threading.Thread):

   def __init__(self):
      threading.Thread.__init__(self)
      self.clientName = "boss"
      self.mqttServer = "192.168.0.50";
      # self.mqttServer = "localhost"
      self.mqttPort = 1883;
      self.Connected = False
      self.numberModules = 0
      self.macDict = {}
      self.idDict = {}
      self.macOrder = []
      self.coTimeDict = {}
      self.data = []
      self.macCallTime = time.time()
      self.verCallTimeDict = {}
      self.verCalledDict = {}
      self.stayAlive = True
      self.version = 0.50
      self.event = threading.Event()
      self.moriShapeDict = {}

      print("Server IP: " + self.mqttServer)
      self.client = mqtt.Client(self.clientName)
      self.client.on_connect = self.on_connect
      self.client.on_message = self.on_message  


   # The callback for when the client receives a CONNACK response from the server.
   def on_connect(self, client, userdata, flags, rc):
      print(colored("Connected with result code "+str(rc),'green'))
      # Subscribing in on_connect() means that if we lose the connection and
      # reconnect then subscriptions will be renewed.
      # self.client.subscribe("$SYS/#")
      self.Connected = True
      self.client.subscribe("esp/pub")
      print("Subscribing to esp publishers")
      self.client.subscribe("esp/+/pub")
      self.client.publish("esp/rec","mac")
      print("Calling for MAC addresses")
      self.macCallTime = time.time()


   # The callback for when a PUBLISH message is received from the server.
   def on_message(self, client, userdata, msg):
      topic = msg.topic.rsplit('/') #example: split esp/00215A97/pub into [esp, 00215A97, pub]
      if(topic[0]=="esp"):
         if(topic[-1] != "pub"): #index -1 is last index of the list
            return

         #print(colored(msg.topic, 'yellow') + ", " + colored(msg.payload.decode('UTF-8')))


         espNum = topic[1]
         msgld = msg.payload.decode('UTF-8')
         pyld = msgld.rsplit(' ')
         cmd = pyld[0]
         #print(colored("Payload = ","green") + pyld[0])

         if(pyld[0] == 'DST:'):
            #print(colored(msg.topic, 'yellow') + ", " + colored(msg.payload.decode('UTF-8')))
            macaddr = pyld[1][:-1]
            rssi = int(pyld[2])
            if(macaddr in self.macDict):  #check if we know other mac
               localMacPos = self.macOrder.index(macaddr)
               foreignMacPos = self.macOrder.index(self.idDict[espNum])
               self.data[localMacPos][foreignMacPos].append(rssi)
               print('RSSI of ['+ macaddr + '] according to [' + self.idDict[espNum] + '] is: ' + rssi)
            else:
               if time.time() - self.macCallTime > 3.0:
                  print(macaddr + " is not found. Calling all for MAC addresses.")
                  self.client.publish("esp/rec","mac")
                  # print("Calling all for MAC address".format(macaddr))
                  self.macCallTime = time.time()

         elif(pyld[0] == 'MAC:'):
            print(colored("MAC address recieved: " + pyld[1], 'green'))
            mac = pyld[1].replace(":", "")
            mac = mac.lower()
            mac = mac[:1] + 'e' + mac[2:] #Hack
            #print("New MAC = " + mac)
            if self.macDict.get(mac) is None:
               self.macDict[mac] = espNum
               #self.macOrder.append(mac)
               self.numberModules += 1
               self.idDict[espNum] = mac
               self.data.append([])
               for i in range(len(data)):
                  self.data[i].append([])
               if(len(self.data)>1):
                  self.data[-1] = [[] for _ in range(len(data[0]))]
            if self.moriShapeDict.get(espNum) is None: #Check if the ESP shape has already been asked once to initialize the shape array
               self.moriShapeDict[espNum] = [200, 200, 200, 0, 0, 0]

         elif(pyld[0] == 'VER:'):
            mac = pyld[1].replace(":", "")
            mac = mac.lower()
            mac = mac[:1] + 'e' + mac[2:] #Hack
            if self.verCallTimeDict.get(mac) is None:
               self.verCallTimeDict[mac] = time.time()
               self.verCalledDict[mac] = False

            if self.verCalledDict.get(mac) == False: #Check if the version has already been asked a short while ago (if it has: wait a bit)
               self.verCallTimeDict[mac] = time.time()

               print(colored(msg.topic, 'yellow') + ", " + colored(msg.payload.decode('UTF-8')))

               espVer = float(pyld[1])
               if(espVer == self.version):
                  print("ESP/{} Software up to date".format(espNum))
                  self.client.publish("esp/{}/rec".format(espNum),"vg")   #version good
               elif(espVer < self.version):
                  print("ESP/{} Software out date".format(espNum))
                  print("ESP/{}: {}".format(espNum,espVer))
                  print("Curr: {}".format(self.version))
                  self.client.publish("esp/{}/rec".format(espNum),"vb")   #version bad
                  
               elif(espVer > self.version):
                  print("Kevin is an idiot and forgot to update this to match newest version")
                  print("ESP/{}: {}".format(espNum,espVer))
                  print("Curr: {}".format(self.version))        
                  self.client.disconnect()
                  self.client.loop_stop()
                  sys.exit()

               self.verCalledDict[mac] = True

            elif time.time() - self.verCallTimeDict.get(mac) > 3:
               self.verCalledDict[mac] = False

         elif(pyld[0] == 'ERR:'):
            print(colored(msgld, 'red'))

         elif(pyld[0] == 'INFO:'): 
            print(colored(msgld, 'yellow'))   

         elif(pyld[0] == 'ON:'):
            #print("ESP " + espNum + " is connected")
            mac = pyld[1].replace(":", "")
            mac = mac.lower()
            mac = mac[:1] + 'e' + mac[2:] #Hack
            self.coTimeDict[mac] = time.time()
            if not (self.macDict.get(mac) is None) and not (mac in self.macOrder): #Check if the ESP has been referenced and offline
               print(colored("New ESP connected", "green"))
               self.macOrder.append(mac)

         elif(pyld[0] == 'SHAPE:'):
            print(pyld)
            mac = pyld[1].replace(":", "")
            mac = mac.lower()
            mac = mac[:1] + 'e' + mac[2:] #Hack
            #print(espNum)
            if not (self.macDict.get(mac) is None):  #Check if the ESP has been referenced
               if self.moriShapeDict.get(espNum) is None: #Check if the ESP shape has already been asked once to initialize the shape array
                  self.moriShapeDict[espNum] = [200, 200, 200, 0, 0, 0]
               #print(pyld[2])
               for i in range(2,len(pyld)):
                  self.moriShapeDict.get(espNum)[i-2] = int(pyld[i])
                  #print(pyld[2].find('1',i+2))
                  #if pyld[2].find('1',i+2) == i+2: #Only change the shape values that have been sent by the ESP (the values that have changed)
                  #   self.moriShapeDict.get(espNum)[i] = int(pyld[i+3])
                  #   print(self.moriShapeDict.get(espNum))
                  #print(self.moriShapeDict.get(espNum))


   def publishGlobal(self, message):
      self.client.publish("esp/rec",message)

   def publishLocal(self, addr, message):
      #print(addr)
      self.client.publish("esp/{}/rec".format(addr),message)

   def getNumberConnected(self):
      #print(self.macOrder)
      for i in range(len(self.macOrder)-1, -1, -1): #Go through the array  from top to bottom to avoid out ouf range errors
         #print(colored("ESP ", "blue") + self.macOrder[i] + "last connexion time = " + colored(time.time() - self.coTimeDict.get(self.macOrder[i]), "blue") + " ago")
         if time.time() - self.coTimeDict.get(self.macOrder[i]) > 3:
            print(colored("ESP " + self.macOrder[i] + " lost", "red"))
            del self.macOrder[i]
      self.numberModules = len(self.macOrder)
      #print(self.macOrder)
      return self.numberModules

   def getMoriShape(self, espNum):
      #print("Mori " + mac + " shape: " + moriShapeDict.get(mac))
      return self.moriShapeDict.get(espNum)

   def getMoriIds(self):
      return self.macDict

   def getMoriOrder(self):
      return self.macOrder

   def run(self):
      startTime = time.time()
      loopTime = time.time()
      print("Connecting...")

      self.client.connect(self.mqttServer, self.mqttPort)   
      self.client.loop_start()       
      while not self.event.is_set():
         if time.time() - loopTime > 10:
            
            loopTime = time.time()
            print("Time Elapsed: {:.3f}".format(loopTime-startTime))
            # self.client.publish("esp/rec","mac")
            print("")
            print(colored("Referenced ESPs : ","yellow"))
            print(self.macDict)
            #print(self.macOrder)
            # calcResults(args.fileName)
            print("")
         self.event.wait(0.5)
 
   def exit(self):
      self.client.disconnect()
      self.client.loop_stop()
      self.event.set()
