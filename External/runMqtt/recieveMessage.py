import serial, time
import datetime
import sys, select, os, traceback
# from argparse import ArgumentParser
import argparse
import numpy as np
import paho.mqtt.client as mqtt
import copy
# from scipy.optimize import minimize, least_squares
# import matplotlib.pyplot as plt
from termcolor import colored
import threading

import runMqtt.commands as commands
import runMqtt.relayHandle as relayHandle

from Settings import names


def splitMessage(msg):
   topic = msg.topic.rsplit('/') #example: split esp/00215A97/pub into [esp, 00215A97, pub]

   if(topic[0] !="esp"):
      espNum = topic[0]
   else:
      espNum = topic[1]
   if(topic[-1] != "p"): #index -1 is last index of the list
      return


   try:
      pyld = msg.payload.decode('UTF-8')
      pyld = pyld.rsplit(' ')
   except:
      pyld = []
      pyld.append(msg.payload[0:4].decode('UTF-8'))
      pyld.append(msg.payload[5:])
   
   return pyld, espNum

def splitMessageUDP(wifi_host, msg, addr):
   try:
      msg = msg.decode('UTF-8')
      msg = msg.rsplit(' ')
      espNum = wifi_host.EPDict.get(addr[0])
      if espNum:
         return msg, espNum
      else:
         return None, None
   except:
      try:
         pyld = []
         pyld.append(msg[0:4].decode('UTF-8'))
         pyld.append(msg[5:])
         espNum = wifi_host.EPDict.get(addr[0])
         return pyld, espNum
      except:
         print(colored("IN TRACEBACK", 'red'))
         traceback.print_exc()


def recieveExternalInput(msg, wifi_host):
   if not wifi_host.getExternalInput(): #Leave if flag is not set
      return
   topic = msg.topic.rsplit('/')

   if len(topic) == 3:
      espNum = topic[1]
      publishTopic = topic[2]
   else:
      publishTopic = topic[1]

   pyld = msg.payload.decode('UTF-8')
   
   if publishTopic == 'l':
      wifi_host.publishLocal(pyld, espNum)
   elif publishTopic == 'g':
      wifi_host.publishGlobal(pyld)
   else:
      print(colored("Could not publish message: ", "red"))
      print(colored(pyld))
      print(colored("Topic:", "red"), end=" ")
      print(topic)


# The callback for when a PUBLISH message is received from the server.
def interpretMessage(self, wifi_host, pyld, espNum):
   try:
      espNum = names.convertFromLetter(espNum)
      cmd = pyld[0]
      if(self.wifi_host.espNoWifiCheck(espNum)):
         self.wifi_host.espMQTT(espNum)
      if cmd == "REL:":
         pyld, espNum = relayHandle.relayHandle(pyld, espNum)
         cmd = pyld[0]
      if(self.wifi_host.espLostCheck(espNum)):
         self.wifi_host.espMQTT(espNum)
         print(colored("ESP Reconnected: " + espNum, "green"))
      commands.commands[cmd](wifi_host, pyld, espNum)
      self.wifi_host.resetOnInterval(espNum)
   except:
      traceback.print_exc()


