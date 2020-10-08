import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class DataLog(tk.Frame):

    def __init__(self, frame, wireless_host):
        self.frame = frame
        self.wireless_host = wireless_host
        self.load()


    def load(self):
        self.dataLogButton = tk.Button(self.frame)
        self.dataLogButton["text"] = "Off"
        self.dataLogButton["fg"]   = "red"
        self.dataLogButton["command"] = lambda: self.toggleDataLog()
        self.dataLogButton.pack({"side": "right"})

        self.listMoriLabel = tk.Label(self.frame, text="Data Logging:")
        self.listMoriLabel.pack({"side": "right"})       


    def toggleDataLog(self):
        if self.dataLogButton['text'] == "Off":
            self.wireless_host.createDataLogFile()
            self.dataLogButton.configure(text="On", fg="green")
        else:
            self.wireless_host.closeDataLogFile()
            self.dataLogButton.configure(text="Off", fg="red")


