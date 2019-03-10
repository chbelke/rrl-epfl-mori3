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
import matplotlib.pyplot as plt
from termcolor import colored
import threading



class MqttHost(threading.Thread):

   def __init__(self):
      threading.Thread.__init__(self)
      self.clientName = "boss"
      self.mqttServer = "192.168.0.50";
      self.mqttPort = 1883;
      self.Connected = False
      self.numberModules = 0
      self.macDict = {}
      self.idDict = {}
      self.macOrder = []
      self.data = []
      self.macCallTime = time.time()
      self.verCallTime = time.time()
      self.stayAlive = True
      self.version = 0.50
      self.event = threading.Event()

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
      topic = msg.topic.rsplit('/')
      if(topic[0]=="esp"):
         if(topic[-1] != "pub"):
            return
         # print(colored(msg.topic, 'yellow') + ", " + colored(msg.payload.decode('UTF-8')))
         espNum = topic[1]
         msgld = msg.payload.decode('UTF-8')
         pyld = msgld.rsplit(' ')
         cmd = pyld[0]

         if(pyld[0] == 'DST:'):
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
            if self.macDict.get(mac) is None:
               self.macDict[mac] = espNum
               self.macOrder.append(mac)
               self.numberModules += 1
               self.idDict[espNum] = mac
               self.data.append([])
               for i in range(len(data)):
                  self.data[i].append([])
               if(len(self.data)>1):
                  self.data[-1] = [[] for _ in range(len(data[0]))]

         elif(pyld[0] == 'VER:'):
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

         elif(pyld[0] == 'ERR:'):
            print(colored(msgld, 'red'))

         elif(pyld[0] == 'INFO:'):   
            print(colored(msgld, 'yellow'))         

   def publishGlobal(self, message):
      self.client.publish("esp/rec",message)

   def publishLocal(self, addr, message):
      self.client.publish("esp/{}/rec".format(addr),message)

   def getNumberConnected(self):
      return self.numberModules

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
            print(self.macDict)
            print(self.macOrder)
             # calcResults(args.fileName)
            print("")
         self.event.wait(0.5)
 
   def exit(self):
      self.client.disconnect()
      self.client.loop_stop()
      self.event.set()
