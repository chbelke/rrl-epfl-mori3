import base64
import ast
import tkinter as tk
from termcolor import colored

import json



INPUT = 13
END_BYTE = 0b00001110

class PublishBinary():

    def __init__(self, frame, parent, mqtthost):
        self.frame = frame

        self.mqtthost = mqtthost
        self.numberConnected = parent.numberConnected

        self.moriNumber = [None]
        self.binMoriVar = tk.StringVar()
        self.binMoriVar.set(self.moriNumber[0])
        self.binMoriVar.trace('w', self.option_select)        
        self.load()


    def load(self):
        self.BinaryLabel = tk.Label(self.frame, text="Publish Binary")
        self.BinaryLabel.pack({"side": "top"})

        self.binaryMori = tk.OptionMenu(self.frame, self.binMoriVar,*self.moriNumber)
        self.binaryMori.pack({"side": "top"}, fill=tk.X, expand=True, pady=5)

        self.pub_bin_entry = tk.Entry(self.frame, bd=1)
        self.pub_bin_entry.pack({"side": "right"}, fill=tk.X, padx=5, expand=True)
        self.pub_bin_button = tk.Button(self.frame)
        self.pub_bin_button["text"] = "Publish",
        self.pub_bin_button["command"] = lambda: self.publishBinary()
        self.pub_bin_button.pack({"side": "right"})   


    def option_select(self, *args):
        print("ESP " + self.binMoriVar.get() + " selected.")


    def update_menu(self, list_of_modules):
        self.moriNumber = list_of_modules
        menu = self.binaryMori["menu"]
        menu.delete(0, "end")
        self.list_of_modules = list_of_modules
        for mori_individ in self.moriNumber:
            menu.add_command(label=mori_individ, 
                         command=lambda value=mori_individ[1]: self.binMoriVar.set(value))   


    def publishBinary(self):
        text = self.pub_bin_entry.get()
        if(text is ""):
            print("Please write a command")
            return
        number = self.binMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return

        message = bytearray(str.encode("rel "))

        text = text.rsplit(' ')
        for txt in text:
            if txt.isdigit():
                message.append(int(txt))
            else:
                message.extend(str.encode(txt))


        message.append(END_BYTE)

        print(message)

        self.mqtthost.publishLocal(message, number)
        # print("success?")
        # self.pub_loc_entry.delete(0, 'end')
        