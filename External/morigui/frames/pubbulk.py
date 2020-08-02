import base64
import ast
import tkinter as tk
from termcolor import colored

import json


MAX_INPUT = 100
ROT_MOTOR_MIN = 200
ROT_MOTOR_MAX = 900
ROT_MOTOR_SLOPE = (ROT_MOTOR_MAX-ROT_MOTOR_MIN) / MAX_INPUT

EXT_MOTOR_MIN = 200
EXT_MOTOR_MAX = 900
EXT_MOTOR_SLOPE = (EXT_MOTOR_MAX-EXT_MOTOR_MIN) / MAX_INPUT  

INPUT = 205
END_BYTE = 0b00001110

class PublishBulk():

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.load()


    def load(self):
        self.pub_label = tk.Label(self.frame, text="Publish Bulk Message")
        self.pub_label.pack({"side": "top"})

        self.pub_cmd = tk.Text(self.frame, width=40, height=15, bd=1)
        self.pub_cmd.pack(fill=tk.X, expand=True, padx=5) 
     
        self.Mqtt_pub = tk.Button(self.frame)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = lambda: self.PublishBulk()
        self.Mqtt_pub.pack({"side": "bottom"}, fill=tk.X, expand=True)


    def PublishBulk (self):
        text = self.pub_cmd.get("1.0",'end-1c')
        text = json.loads(text)
        for esp in text:
            if type(text[esp]) is dict:
                for key in text[esp]:
                    if key.lower() == 'shape':
                        self.InterpretShape(text, key, esp)
            elif type(text[esp]) is str:
                self.mqtthost.publishLocal(text[esp], esp)
        self.pub_cmd.delete("1.0", 'end')


    def InterpretShape (self, text, key, esp):
        cmds = text[esp][key]      

        if 'ang' in cmds or 'ext' in cmds:
            follow_bytes = []
            alloc = 0b00000000

            if 'ext' in cmds:
                extensions = tuple(cmds['ext'].split(', '))
                for i, ext in enumerate(extensions):
                    if ext.isdigit():
                        alloc = alloc | (0b00100000 >> i)
                        ext = EXT_MOTOR_MIN + EXT_MOTOR_SLOPE*(float(ext))
                        follow_bytes.append(int(ext))

            if 'ang' in cmds:
                angles = tuple(cmds['ang'].split(', '))
                for i, ang in enumerate(angles):
                    if ang.isdigit():
                        alloc = alloc | (0b00000100 >> i)
                        ang = ROT_MOTOR_MIN + ROT_MOTOR_SLOPE*(float(ang))
                        follow_bytes.append(int(ang))


            message = bytearray(str.encode("rel "))
            message.append(INPUT)
            message.append(alloc)
            for cmd_bytes in follow_bytes:
                message.extend(cmd_bytes.to_bytes(2, byteorder='big'))
            message.append(END_BYTE)
            self.mqtthost.publishLocal(message, esp)


        if 'coup' in cmds or 'led' in cmds:
            follow_bytes = []
            alloc = 0b10000000
            print(alloc)

            if 'coup' in cmds:
                couplings = tuple(cmds['coup'].split(', '))
                for i, coup in enumerate(couplings):
                    if coup.isdigit():
                        alloc = alloc | (0b00100000 >> i)

            if 'led' in cmds:
                leds = tuple(cmds['led'].split(', '))
                for i, led in enumerate(leds):
                    if led.isdigit():
                        alloc = alloc | (0b00000100 >> i)
                        follow_bytes.append(int(led))


            message = bytearray(str.encode("rel "))
            message.append(INPUT)
            message.append(alloc)
            for cmd_bytes in follow_bytes:
                message.extend(cmd_bytes.to_bytes(1, byteorder='big'))
            message.append(END_BYTE)
            print(message)
            self.mqtthost.publishLocal(message, esp)
        

        if 'drive' in cmds:
            print(colored('Drive functionality not implimented', 'red'))
            return
        return