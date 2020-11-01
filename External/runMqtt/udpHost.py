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
from Settings import names
import runMqtt.operations.getMac as getMac


class UDPListener():
    def __init__(self, udp):
        self.udp = udp
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
            data, addr = self.listen_sock.recvfrom(128) 
            # print('received', data, 'from', addr)
            if(addr[0] == '127.0.0.1'):
                continue
            try:
                pyld, espNum =splitMessageUDP(self.wifi_host, data, addr)
                if espNum is not None:
                    interpretMessage(self, self.wifi_host, pyld, espNum)
                elif data[:3].decode('UTF-8') == "NM:":
                    self.udp.updateIPlist(addr[0], data)
                else:
                    self.udp.askForName(addr)
            except:
                print(colored('received: '+ str(data) + 'from' + str(addr), 'red'))
                traceback.print_exc() 

    def exit(self):
        self.listen_sock.close()



class UDPHost():

    def __init__(self, wifi_host):
        self.wifi_host = wifi_host
        
        self.udp_list = []
        self.udp_port = 50001
        self.udp_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.ask_list = {}

        self.usp_rec = UDPListener(self)
        

    def write(self, message, addr):
        ip_addr = (self.wifi_host.IPDict[addr], self.udp_port)
        self.transmit(message, ip_addr)
      

    def transmit(self, message, ip_addr):
        if len(message) == 0:
            return
        print(ip_addr, message)
        if isinstance(message, bytearray):
            sent = self.udp_sock.sendto(message, ip_addr)
        else:
            sent = self.udp_sock.sendto(message.encode(), ip_addr)


    def exit(self):
        self.usp_rec.exit()
        self.udp_sock.close()


    def askForName(self, ip_addr):
        if ip_addr[0] in self.wifi_host.IPDict:
            return
        if (ip_addr not in self.ask_list or 
            time.time() - self.ask_list[ip_addr] > 1):
            self.transmit("nam", ip_addr)
            self.ask_list[ip_addr] = time.time()


    def updateIPlist(self, ip_addr, data):
        msg = data.decode('UTF-8')
        msg = msg.rsplit(' ') 
        letter = msg[1].rsplit('/')[0]
        mac = msg[2]
        espNum = names.convertFromLetter(letter)

        getMac.getMac(self.wifi_host, ['MAC:', mac],espNum)

        if espNum not in self.wifi_host.UDPDict:
            self.wifi_host.UDPDict.append(espNum)
            self.wifi_host.setUDPDict(self.wifi_host.UDPDict)
            self.wifi_host.espUDP(espNum)        

        if self.wifi_host.IPDict.get(espNum) is None:
            self.wifi_host.IPDict[espNum] = ip_addr
            self.wifi_host.EPDict[ip_addr] = espNum
            self.wifi_host.setIPDict(self.wifi_host.IPDict, self.wifi_host.EPDict)

        if ip_addr in self.ask_list:
            self.ask_list.pop(ip_addr)

