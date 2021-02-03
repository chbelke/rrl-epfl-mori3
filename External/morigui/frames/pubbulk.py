import base64
import ast
import tkinter as tk
from termcolor import colored

import json

from Settings import names

MAX_INPUT = 100
ROT_MOTOR_MIN = 200
ROT_MOTOR_MAX = 900
ROT_MOTOR_SLOPE = (ROT_MOTOR_MAX-ROT_MOTOR_MIN) / MAX_INPUT

EXT_MOTOR_MIN = 200
EXT_MOTOR_MAX = 900
EXT_MOTOR_SLOPE = (EXT_MOTOR_MAX-EXT_MOTOR_MIN) / MAX_INPUT  

INPUT = 205
END_BYTE = 14

class PublishBulk():

    def __init__(self, frame, wifi_host):
        self.frame = frame
        self.wifi_host = wifi_host
        self.load()


    def load(self):
        self.pub_label = tk.Label(self.frame, text="Publish Bulk Message")
        self.pub_label.pack({"side": "top"})

        self.pub_cmd = tk.Text(self.frame, width=30, height=15, bd=1)
        self.pub_cmd.pack(fill=tk.X, expand=True, padx=5) 
     
        self.Mqtt_pub = tk.Button(self.frame)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = lambda: self.PublishBulk()
        self.Mqtt_pub.pack({"side": "left"}, fill=tk.X, expand=True)

        self.Mqtt_clear = tk.Button(self.frame)
        self.Mqtt_clear["text"] = "Clear",
        self.Mqtt_clear["command"] = lambda: self.ClearBulk()
        self.Mqtt_clear.pack({"side": "right"}, fill=tk.X, expand=True)


    def PublishBulk (self):
        text = self.pub_cmd.get("1.0",'end-1c')
        text = json.loads(text, object_pairs_hook=self.value_resolver)
        for esp in text:
            for cmd in text[esp]:
                if 'global' in esp.lower():
                    print("Global: ", cmd)
                    self.wifi_host.publishGlobal(cmd)                    
                elif type(cmd) is dict:
                    for key in cmd:
                        if key.lower() == 'shape':
                            self.InterpretShape(text, key, esp)
                        elif key.lower() == 'wifiedge':
                            self.SetEdge(text, key, esp)
                        elif type(text[esp][key]) is str:
                            self.wifi_host.publishLocal(cmd, names.checkName(esp))                    
                elif type(cmd) is str:
                    if cmd.startswith("ping"):
                        num = 1
                        splitText = cmd.split("ping",1)
                        if (splitText[1].strip().isnumeric()):
                            num = int(splitText[1])
                        self.wifi_host.pingHandler.setPingCount(names.checkName(esp), num)
                        continue
                    self.wifi_host.publishLocal(cmd, names.checkName(esp))

    def ClearBulk(self):
        self.pub_cmd.delete("1.0", 'end')


    def SetEdge(self, text, key, esp):
        message = bytearray(str.encode("rel "))
        alloc = 0b11010111  #CMD Handle + 23
        edge = text[esp][key]
        message.append(alloc)
        message.append(int(edge))
        message.append(END_BYTE)

        self.wifi_host.publishLocal(message, names.checkName(esp))


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
                        follow_bytes.append(int(ext))
                        numbytes = 1

            if 'ang' in cmds:
                angles = tuple(cmds['ang'].split(', '))
                for i, ang in enumerate(angles):
                    if ang.isdigit():
                        alloc = alloc | (0b00000100 >> i)
                        ang = ROT_MOTOR_MIN + ROT_MOTOR_SLOPE*(float(ang))
                        follow_bytes.append(int(ang))
                        numbytes = 2

            message = bytearray(str.encode("rel "))
            message.append(INPUT)
            message.append(alloc)
            for cmd_bytes in follow_bytes:
                message.extend(cmd_bytes.to_bytes(numbytes, byteorder='big'))
            message.append(END_BYTE)
            print(message)
            self.wifi_host.publishLocal(message, names.checkName(esp))


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
            self.wifi_host.publishLocal(message, names.checkName(esp))
        

        if 'drive' in cmds:
            print(colored('Drive functionality not implimented', 'red'))
            return
        return

    def value_resolver(self, pairs):
        print(pairs)
        unique_modules = set([i[0] for i in pairs])
        new_dict = {}
        print(unique_modules)
        for module in unique_modules:
            values = [v for k, v in pairs if k == module]
            new_dict[module] = values
        return dict(new_dict)


class restore_value(json.JSONEncoder):
    def encode(self, o):
        if isinstance(o, dict):
            return '{%s}' % ', '.join(': '.join((json.encoder.py_encode_basestring(k), self.encode(v))) for k, v in o.items())
        if isinstance(o, list):
            return '{%s}' % ', '.join('"value": %s' % self.encode(v) for v in o)
        return super().encode(o)