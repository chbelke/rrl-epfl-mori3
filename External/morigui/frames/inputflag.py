import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class InputFlag(tk.Frame):

    def __init__(self, frame, wireless_host):
        self.frame = frame
        self.wireless_host = wireless_host
        self.load()


    def load(self):
        self.inputFlagButton = tk.Button(self.frame)
        self.inputFlagButton["text"] = "Off"
        self.inputFlagButton["fg"]   = "red"
        self.inputFlagButton["command"] = lambda: self.toggleInputFlag()
        self.inputFlagButton.pack({"side": "right"})

        self.listMoriLabel = tk.Label(self.frame, text="External Input:")
        self.listMoriLabel.pack({"side": "right"})       


    def toggleInputFlag(self):
        if self.inputFlagButton['text'] == "Off":
            self.wireless_host.enableExternalInput()
            self.inputFlagButton.configure(text="On", fg="green")
        else:
            self.wireless_host.disableExternalInput()
            self.inputFlagButton.configure(text="Off", fg="red")


