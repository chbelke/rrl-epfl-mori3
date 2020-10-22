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


#udp/w/p100/i192.168.0.53/m01 00111111 0900090009000000-1200120 150


class MqttHost(threading.Thread):

    def __init__(self, wifi_host):
        threading.Thread.__init__(self)
        self.wifi_host = wifi_host
        
        self.clientName = "boss"
        # self.mqttServer = "RRLB201957";
        # self.mqttServer = "MQTT-HOST";
        self.mqttServer = "192.168.1.2";
        self.mqttPort = 1883;
        self.Connected = False

        print("Server IP: " + self.mqttServer)
        self.client = mqtt.Client(self.clientName)
        self.client.on_connect = self.on_connect
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message 

    def run(self):
        print("Connecting...")
        self.client.connect(self.mqttServer, self.mqttPort)   
        self.client.loop_start()       


    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(self, client, userdata, flags, rc):
        print(colored("Connected with result code "+str(rc),'green'))
        # self.client.subscribe("$SYS/#")
        self.Connected = True
        self.client.subscribe("esp/pub")
        print("Subscribing to esp publishers")
        self.client.subscribe("+/p")
        self.client.publish("esp/rec","mac")
        print("Calling for MAC addresses")
        self.macCallTime = time.time()


    def on_disconnect(self, client, userdata, rc):
        print(colored("Computer disconnected from MQTT", 'red'))
        self.Connected = False


    # The callback for when a PUBLISH message is received from the server.
    def on_message(self, client, userdata, msg):
        try:
            pyld, espNum = splitMessage(msg)
            interpretMessage(self, self.wifi_host, pyld, espNum)
        except:
            traceback.print_exc()


    def publishGlobal(self, message):
        self.client.publish("esp/rec",message)


    def publishLocal(self, msg, addr):
        print("{}/r".format(addr), msg)
        self.client.publish("{}/r".format(addr), msg)
 

    def exit(self):
        self.client.disconnect()
        self.client.loop_stop()
