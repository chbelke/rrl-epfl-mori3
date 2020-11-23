import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class PingLog(tk.Frame):

    def __init__(self, frame, wireless_host):
        self.frame = frame
        self.wireless_host = wireless_host
        self.load()


    def load(self):
        self.waitButton = tk.Button(self.frame)
        self.waitButton["text"] = "Sequential"
        self.waitButton["command"] = lambda: self.toggleWaitForBulk()
        self.waitButton.pack({"side": "right"})

        self.pingButton = tk.Button(self.frame)
        self.pingButton["text"] = "Off"
        self.pingButton["fg"]   = "red"
        self.pingButton["command"] = lambda: self.togglePingLog()
        self.pingButton.pack({"side": "right"})

        self.listMoriLabel = tk.Label(self.frame, text="Ping Logging:")
        self.listMoriLabel.pack({"side": "left"})


    def togglePingLog(self):
        if self.pingButton['text'] == "Off":
            self.wireless_host.pingHandler.createPingFile()
            self.pingButton.configure(text="On", fg="green")
        else:
            self.wireless_host.pingHandler.closePingFile()
            self.pingButton.configure(text="Off", fg="red")


    def toggleWaitForBulk(self):
        if self.waitButton['text'] == "Sequential":
            self.wireless_host.pingHandler.waitForAll(True)
            self.waitButton.configure(text="Batch")
        else:
            self.wireless_host.pingHandler.waitForAll(False)
            self.waitButton.configure(text="Sequential")
