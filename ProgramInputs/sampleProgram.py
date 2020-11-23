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

import paho.mqtt.client as mqtt
import threading

import tkinter as tk

def main():
    startTime = time.time()
    # basicExample()    #Uncomment to send "hello" every 2 seconds
    guiExample()      #Uncomment to open an external gui window



# Most basic example of how to integrate an external program
# Sends "hello" every two seconds to the global topic
def basicExample():
    mqttClient = MqttHost() #Create MqttHost class (see below)
    mqttClient.run()        #Run mqttClass on it's own thread
    
    loopTime = time.time()  #every 2 secongs, publish hello
    while(True):
        if time.time() - loopTime > 2:
            mqttClient.publishGlobal("hello")   #Call mqttClient to publish
            loopTime = time.time()    



# Example of how to integrate the program into a Gui
# Program has two sliders to adjust the angles and extensions
# Every 5 seconds, the gui publishes "hello" as well
# guiExample class creates our root window and MQTT client
# guiExampleFrame runs the frame which can send messages
class guiExample(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        self.mqttClient = MqttHost()
        self.mqttClient.run()    
        self.start()


    def callback(self):
        self.mqttClient.exit()
        self.root.quit()


    def run(self):
        self.root = tk.Tk()
        self.root.protocol("WM_DELETE_WINDOW", self.callback)
        self.root.title("Extension")
        self.gui = guiExampleFrame(master=self.root, mqttClient=self.mqttClient)
        self.gui.sendHello()
        self.gui.mainloop()



# Runs the frame which the user interacts with
class guiExampleFrame(tk.Frame):
    def __init__(self, master=None, mqttClient=None, *args, **kwargs):
        threading.Thread(tk.Frame.__init__(self, master))
        self.master = master
        self.mqttClient = mqttClient
        self.createWidgets()
        self.pack()       


    def createWidgets(self):
        frame_angles = tk.Frame(self)   #Collect the angle widgets
        frame_edges = tk.Frame(self)    #Collect the edge widgets
        
        # Scale is our slider, button links to the "sendAngles function"
        # angle_scale is self-referenced in order to access the values later
        self.angle_scale = tk.Scale(frame_angles, from_=120, to=-120, length=300)
        angle_button = tk.Button(frame_angles, text='Send Angles', command=self.sendAngles)
        self.angle_scale.pack(side="right") #Chooses where the scale is located relative to the button
        angle_button.pack(side="left")      #Chooses where the button is located relative to the scale

        self.edges_scale = tk.Scale(frame_edges, from_=0, to=120, length=300,  orient=tk.HORIZONTAL)
        edges_button = tk.Button(frame_edges, text='Send Edges', command=self.sendEdges)
        self.edges_scale.pack(side="bottom")
        edges_button.pack(side="top")        

        frame_edges.pack(side="top")    #Assembles where the edge widges are located
        frame_angles.pack(side="bottom")


    def sendAngles(self):
        angles = self.angle_scale.get()
        angles *= -10
        angles += 1800
        print("Angles: ", angles)
        self.mqttClient.publishGlobal(angles)


    def sendEdges(self):
        edges = self.edges_scale.get()
        print("Angles: ", edges)
        self.mqttClient.publishGlobal(edges)


    #Every 5 seconds, say hello
    def sendHello(self): 
        self.after(5000, self.sendHello)
        self.mqttClient.publishGlobal("hello")        



#Host for running programs (Keep similar)
class MqttHost(threading.Thread):

    def __init__(self):
        threading.Thread.__init__(self)
        
        self.clientName = "Extern"  #The name of the program itself
        self.mqttServer = "192.168.1.2"; #important - linked to the computer
        self.mqttPort = 1883;   #Keep same
        self.Connected = False  

        self.client = mqtt.Client(self.clientName)
        self.client.on_connect = self.on_connect    #links functions to their occorance
        self.client.on_disconnect = self.on_disconnect
        self.client.on_message = self.on_message 
        print("Server IP: " + self.mqttServer)


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
        # self.client.subscribe("esp/pub")
        # self.client.subscribe("+/p")


    def on_disconnect(self, client, userdata, rc):
        print(colored("Computer disconnected from MQTT", 'red'))
        self.Connected = False


    # The callback for when a PUBLISH message is received from the server.
    def on_message(self, client, userdata, msg):
        print(msg.topic, msg.payload)


    def publishGlobal(self, message):
        self.client.publish("Ext/g",message)


    #addr is in the form of:
    #   1) The name of the Mori
    #   2) The first letter of the Mori
    #   3) The ID of the ESP chip
    def publishLocal(self, msg, addr):
        self.client.publish("Ext/{}/l".format(addr), msg)
 

    def exit(self):
        self.client.disconnect()
        self.client.loop_stop()


# Keep at bottom (runs program)
if __name__ == '__main__':
    sys.exit(main())