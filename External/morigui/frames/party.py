import tkinter as tk
import base64
from termcolor import colored

from morigui.localSettings import *


class PartyFrame():

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.paaarty = self.partyOn()
        self.load()

    def load(self):
        self.hi_there = tk.Button(self.frame)
        self.hi_there["text"] = "Party",
        self.hi_there["command"] = lambda: self.say_hi()
        self.hi_there.pack(side="right",fill=tk.X, expand=True)

    def partyOn(self):
        return base64.b64decode(party).decode('UTF-8')

    def say_hi(self):
        self.mqtthost.publishGlobal("party")
        print(colored(self.paaarty,'green'))