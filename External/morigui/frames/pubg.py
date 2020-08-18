import tkinter as tk
import base64
from termcolor import colored




class PublishGlobal():

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.load()

    def load(self):
        self.pub_label = tk.Label(self.frame, text="Publish Global Message")
        self.pub_label.pack()
     
        self.Mqtt_pub = tk.Button(self.frame)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = lambda: self.publishGlobal()
        self.Mqtt_pub.pack({"side": "right"}, fill=tk.X, expand=True)

        self.pub_cmd = tk.Entry(self.frame, bd=1)
        self.pub_cmd.pack(fill=tk.X, expand=True, padx=5) 

    def publishGlobal(self):
        text = self.pub_cmd.get()
        self.mqtthost.publishGlobal(text)
        print("Published \"" + text + "\" to esp/rec")
        self.pub_cmd.delete(0, 'end')