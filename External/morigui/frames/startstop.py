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
        self.Stop["text"] = "STOP"
        self.Stop["fg"]   = "red"
        self.Stop["command"] = lambda: self.emergencyStop()
        self.Stop.pack(side="left")

        self.Mqtt = tk.Button(self.frame)
        self.Mqtt["text"] = "Preset"
        self.Mqtt["fg"]   = "green"
        self.Mqtt["command"] = lambda: self.picReset()
        self.Mqtt.pack(side="right")


    def emergencyStop(self):
        print(colored("EMERGENCY STOP", "red"))
        self.mqtthost.publishGlobal("stp")


    def picReset(self):
        self.mqtthost.publishGlobal("pre")


