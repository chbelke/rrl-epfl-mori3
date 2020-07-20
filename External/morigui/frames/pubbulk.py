import tkinter as tk
import base64
from termcolor import colored

import json



class PublishBulk():

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.load()

    def load(self):
        self.pub_label = tk.Label(self.frame, text="Publish Bulk Message")
        self.pub_label.pack({"side": "top"})

        self.pub_cmd = tk.Text(self.frame, width=20, height=15, bd=1)
        self.pub_cmd.pack(fill=tk.X, expand=True, padx=5) 
     
        self.Mqtt_pub = tk.Button(self.frame)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = lambda: self.PublishBulk()
        self.Mqtt_pub.pack({"side": "bottom"}, fill=tk.X, expand=True)

    def PublishBulk (self):
        text = self.pub_cmd.get("1.0",'end-1c')
        text = json.loads(text)
        for esp in text:
            self.mqtthost.publishLocal(text[esp], esp)
        self.pub_cmd.delete("1.0", 'end')