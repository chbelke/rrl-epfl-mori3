import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class StartStop(tk.Frame):

    def __init__(self, frame, mqtthost, emergencyFrame):
        self.frame = frame
        self.emergencyFrame = emergencyFrame
        self.mqtthost = mqtthost
        self.load()


    def load(self):
        self.Stop = tk.Button(self.frame)
        self.Stop["text"] = "STOP"
        self.Stop["fg"]   = "red"
        self.Stop["command"] = lambda: self.emergencyStop()
        self.Stop.pack(side="left", fill=tk.X, expand=True)

        self.Mqtt = tk.Button(self.frame)
        self.Mqtt["text"] = "Preset"
        self.Mqtt["fg"]   = "green"
        self.Mqtt["command"] = lambda: self.picReset()
        self.Mqtt.pack(side="left", fill=tk.X, expand=True)


    def emergencyStop(self):
        print(colored("EMERGENCY STOP", "red"))
        self.mqtthost.disableExternalInput()
        self.mqtthost.publishGlobal("stp")
        self.emergencyFrame.inputFlagButton.configure(text="Off", fg="red")


    def picReset(self):
        self.mqtthost.publishGlobal("pre")


