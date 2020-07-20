import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class StartStop(tk.Frame):

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.load()


    def load(self):
        self.Stop = tk.Button(self.frame)
        self.Stop["text"] = "Stop MQTT"
        self.Stop["fg"]   = "red"
        self.Stop["command"] = lambda: self.quitButton()
        self.Stop.pack(side="left")

        self.Mqtt = tk.Button(self.frame)
        self.Mqtt["text"] = "Start MQTT"
        self.Mqtt["fg"]   = "green"
        self.Mqtt["command"] = lambda: self.runMqtt()
        self.Mqtt.pack(side="right")


    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()


    def runMqtt(self):
        if self.mqtthost.getMqttStatus():
            print("MQTT Already Running")
        else:
            print("Starting MQTT")
            self.mqtthost.run()


