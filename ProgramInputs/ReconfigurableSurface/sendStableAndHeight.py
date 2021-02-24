'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Used to command Mori from a program external to the gui.

Writes to GUI to topic Ext/+/l and Ext/+/g for local publish and global publish.

**********************************************************************************
'''


import serial, time
import datetime
import sys, select, os
from termcolor import colored
import numpy as np

import paho.mqtt.client as mqtt
import threading

import tkinter as tk

import names

def main():
    startTime = time.time()
    relayToMatlab()    



# Most basic example of how to integrate an external program
# Sends "hello" every two seconds to the global topic
def relayToMatlab():
    mqttClient = MqttHost() #Create MqttHost class (see below)
    mqttClient.run()        #Run mqttClass on it's own thread
    mqttClient.publishGlobal("hello")   #Call mqttClient to publish

    loopTime = time.time()  #every 2 secongs, publish hello
    while(True):
        if time.time() - loopTime > .250:
            mqttClient.publishToMatlab(mqttClient.getAverageAngle(), "angle")   #Call mqttClient to publish
            mqttClient.publishToMatlab(mqttClient.getStableStatus(), "stable")   #Call mqttClient to publish
            loopTime = time.time()    

#Host for running programs (Keep similar)
class MqttHost(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        
        self.clientName = "Extern"  #The name of the program itself
        self.mqttServer = "192.168.1.2"; #important - linked to the computer
        self.mqttPort = 1883;   #Keep same
        self.Connected = False  

        self.stable_flag = False
        self.module_dict = {}
        self.module_list =  ["Gina", "Hiro", "Itchy", "Jerry", "Karen", "Lucille"]
        self.initializeModuleDict()

        self.client = mqtt.Client(self.clientName)
        self.client.on_connect = self.on_connect    #links functions to their occorance
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message 
        print("Server IP: " + self.mqttServer)


    def initializeModuleDict(self):
        for module in self.module_list:
            self.module_dict[module] = {}
            self.module_dict[module]["stable"] = False
            self.module_dict[module]["angle"] = [1800, 1800, 1800]
            
            #Edges 2 and 3 are connected to neighbours (can update)
            self.module_dict[module]["connected"] = [1,2] # Edges from 0,1,2

        # self.module_dict["Itchy"]["connected"] = [2]
        # self.module_dict["Jerry"]["connected"] = [1]
        # self.module_dict["Lucille"]["connected"] = [2]
        # self.module_dict["Gina"]["connected"] = [1]



    # Loop which handles the MQTT functions
    def run(self):
        print("Connecting...")
        self.client.connect(self.mqttServer, self.mqttPort)   
        self.client.loop_start()       


    # The callback for when the client receives a CONNACK response from the server.
    def on_connect(self, client, userdata, flags, rc):
        print(colored("Connected with result code "+str(rc),'green'))
        self.Connected = True

        # Uncomment below to subscribe to the topics
        self.client.subscribe("esp/pub")
        self.client.subscribe("+/p")


    def on_disconnect(self, client, userdata, rc):
        print(colored("Computer disconnected from MQTT", 'red'))
        self.Connected = False


    # The callback for when a PUBLISH message is received from the server.
    def on_message(self, client, userdata, msg):
        # print(msg.topic, msg.payload)
        pyld, espNum = self.splitMessage(msg)
        espNum = names.convertFromLetter(espNum)
        espNum = names.checkId(espNum)
        # interpretMessage(self, self.wifi_host, pyld, espNum)
        cmd = pyld[0]
        if cmd == "SB:":
            self.updateStableFlag(espNum, pyld)
        if cmd == "REQ:":
            if pyld[1] == "AN":
                self.updateRequestAngles(espNum, pyld)
        if cmd == "DLG:":
            self.updateAnglesFromInfo(espNum, pyld)


    def updateStableFlag(self, espNum, pyld):
        stable = (pyld[1] == '1')
        # print(espNum, pyld, stable)
        self.module_dict[espNum]["stable"] = stable


    def updateRequestAngles(self, espNum, pyld):
        angles = [None]*3
        for i in range(0,3):
            angles[i] = int(pyld[i+2])
        self.module_dict[espNum]["angle"] = angles
        # print("RAN: ", espNum, self.module_dict[espNum]["angle"])


    def updateAnglesFromInfo(self, espNum, pyld):
        pld_str = pyld[1]
        num_hex = int(len(pyld[1]))
        values = [None]*int((num_hex)/2)
        for i in range(int(num_hex/2)):
            values[i] = int(pld_str[i*2:i*2+2], 16)

        angles = [0]*3
        i = 5;
        try:
            if values[0] & 0b00000001:    #angles
                for j in range(0,3):
                    angles[j] = values[i]*256 + values[i+1]
                    i += 2
            self.module_dict[espNum]["angle"] = angles
            # print("DLG: ", espNum, self.module_dict[espNum]["angle"])
        except:
            print(colored("IN TRACEBACK", 'red'))
            traceback.print_exc()        


    def publishGlobal(self, message):
        self.client.publish("Ext/g",message)


    #addr is in the form of:
    #   1) The name of the Mori
    #   2) The first letter of the Mori
    #   3) The ID of the ESP chip
    def publishLocal(self, msg, addr):
        self.client.publish("Ext/{}/l".format(addr), msg)


    def publishToMatlab(self, msg, addr):
        self.client.publish("matlab/{}".format(addr), msg)
 

    def exit(self):
        self.client.disconnect()
        self.client.loop_stop()


    def splitMessage(self, msg):
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


    def getAverageAngle(self):
        relevant_angles = []
        for module in self.module_list:
        # for module in ["Jerry", "Karen", "Lucille"]:
            relevant_angles.append([ang for i, ang in enumerate(self.module_dict[module]["angle"]) if i in self.module_dict[module]["connected"]])
        flat_angles = np.array(relevant_angles).ravel()
        flat_angles = np.concatenate(relevant_angles).ravel()
        print(relevant_angles)
        print(flat_angles)
        avg = np.mean(flat_angles)
        st_dev = np.std(flat_angles)
        print("Average Ang: ", avg)
        print("Average std: ", st_dev)
        if abs(st_dev) > 20:
            self.stable_flag = False
        else:
            self.stable_flag = True
        return avg



    def getStableStatus(self):
        # return True
        if self.stable_flag == False:
            return False
        # for module in ["Jerry", "Karen", "Lucille"]:
        for module in self.module_list:
            if self.module_dict[module]["stable"] == False:
                return False
        return True
        return all(self.module_dict[module]["stable"] for module in self.module_list)



# Keep at bottom (runs program)
if __name__ == '__main__':
    sys.exit(main())