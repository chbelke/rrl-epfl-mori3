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


def splitMessage(msg):
   print("Here: ", msg.payload)
   topic = msg.topic.rsplit('/') #example: split esp/00215A97/pub into [esp, 00215A97, pub]

   if(topic[0] !="esp"):
      return
   if(topic[-1] != "pub"): #index -1 is last index of the list
      return

   espNum = topic[1]

   try:
      pyld = msg.payload.decode('UTF-8')
      pyld = pyld.rsplit(' ')
   except:
      pyld = []
      pyld.append(msg.payload[0:4].decode('UTF-8'))
      pyld.append(msg.payload[5:])
      # if start == "VBS:":
      #    end = bytearray.fromhex(msg.payload[4:-1]).decode()
      #    print("end: ", end)
      #    pyld = [start, end].flatten()
      #    print(pyld)

   
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
      print(colored("IN TRACEBACK", 'red'))
      traceback.print_exc()


# The callback for when a PUBLISH message is received from the server.
def interpretMessage(self, wifi_host, pyld, espNum):
   try:
      cmd = pyld[0]
      commands.commands[cmd](wifi_host, pyld, espNum)
   except:
      traceback.print_exc()


