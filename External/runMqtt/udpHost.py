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

from termcolor import colored

from runMqtt.recieveMessage import *


class UDPListener():
    def __init__(self, udp):
        self.udp_port = udp.udp_port
        self.wifi_host = udp.wifi_host
        self.thread = None
        self.listen_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.listen_sock.bind(('',self.udp_port)) #绑定端口binding the port

    def run(self):
        self.thread_terminate = False
        self.thread = threading.Thread(target=self.listen)
        self.thread.daemon = True
        self.thread.start()

    def listen(self):
        run = True
        while run:
            # print('waiting...')
            data, addr = self.listen_sock.recvfrom(1024) 
            # print('received:',data,'from',addr)
            if(addr[0] == '127.0.0.1'):
                # print("Call from home")
                continue
            pyld, espNum =splitMessageUDP(self.wifi_host, data, addr)
            if espNum is not None:
                interpretMessage(self, self.wifi_host, pyld, espNum)
            else:
                self.wifi_host.publishGlobal("mac")
                # print("getting IP")

    def exit(self):
        self.listen_sock.close()


class UDPHost():

    def __init__(self, wifi_host):
        self.wifi_host = wifi_host
        
        self.udp_list = []
        self.udp_port = 50001
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.usp_rec = UDPListener(self)
        

    def write(self, message, addr):
        print(message, addr)
        print(self.wifi_host.IPDict[addr])
        ip_addr = (self.wifi_host.IPDict[addr], self.udp_port)
        sent = self.udp_sock.sendto(message.encode(), ip_addr)


    def exit(self):
        self.usp_rec.exit()
        self.udp_sock.close()