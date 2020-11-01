import tkinter as tk
import base64
from termcolor import colored

from Settings import names

class PublishLocal():

    def __init__(self, frame, parent, wirelesshost):
        self.frame = frame

        self.wirelesshost = wirelesshost
        self.numberConnected = parent.numberConnected

        self.moriNumber = [None]
        self.listMoriVar = tk.StringVar()
        self.listMoriVar.set(self.moriNumber[0])
        self.listMoriVar.trace('w', self.option_select)        
        self.load()

        self.frame.master.bind('<Return>', 
            lambda event=None: self.pub_loc_button.invoke())


    def load(self):
        self.frame_top = tk.Frame(self.frame)
        self.frame_local = tk.Frame(self.frame)
        self.frame_binary = tk.Frame(self.frame)

        self.listMoriLabel = tk.Label(self.frame_top, text="Mori Available:")
        self.listMoriLabel.pack({"side": "left"})
        self.disp = tk.Label(self.frame_top, textvariable=str(self.numberConnected))
        self.disp.pack({"side": "left"}, padx=10)

        self.listMori = tk.OptionMenu(self.frame_top, self.listMoriVar,*self.moriNumber)
        self.listMori.pack({"side": "top"}, fill=tk.X, expand=True, pady=5)

        self.pub_loc_label = tk.Label(self.frame_local, text="Publish Local")
        self.pub_loc_label.pack({"side": "top"})

        self.pub_loc_entry = tk.Entry(self.frame_local, bd=1)
        self.pub_loc_entry.pack({"side": "right"}, fill=tk.X, padx=5, expand=True)
        
        self.pub_loc_button = tk.Button(self.frame_local)
        self.pub_loc_button["text"] = "Publish",
        self.pub_loc_button["command"] = lambda: self.publishLocal()
        self.pub_loc_button.pack({"side": "left"})   

        self.BinaryLabel = tk.Label(self.frame_binary, text="Publish Binary")
        self.BinaryLabel.pack({"side": "top"})

        self.pub_bin_entry = tk.Entry(self.frame_binary, bd=1)
        self.pub_bin_entry.pack({"side": "right"}, fill=tk.X, padx=5, expand=True)

        self.pub_bin_button = tk.Button(self.frame_binary)
        self.pub_bin_button["text"] = "Publish",
        self.pub_bin_button["command"] = lambda: self.publishBinary()
        self.pub_bin_button.pack({"side": "left"})      

        self.frame_top.pack()
        self.frame_local.pack()
        self.frame_binary.pack()


    def option_select(self, *args):
        print(self.listMoriVar.get() + " selected.")


    def publishLocal(self):
        text = self.pub_loc_entry.get()
        if(text is ""):
            print("Please write a command")
            return
        number = self.listMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return
        if text.startswith("ping"):
            num = 1
            splitText = text.split("ping",1)
            if (splitText[1].strip().isnumeric()):
                num = int(splitText[1])
            self.wirelesshost.pingHandler.setPingCount(names.checkName(number), num)
            return
        self.wirelesshost.publishLocal(text, names.checkName(number))


    def publishBinary(self):
        text = self.pub_bin_entry.get()
        if(text == ""):
            print("Please write a command")
            return
        number = self.listMoriVar.get()
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

        print(message)
        for i in message:
            print(i, end=' ')
        print()
        self.wirelesshost.publishLocal(message, names.nameToIds[number])


    def update_menu(self, list_of_modules):
        self.moriNumber = list_of_modules
        menu = self.listMori["menu"]
        menu.delete(0, "end")

        #Create all roll down menus
        self.list_of_modules = list_of_modules
        for mori_individ in self.moriNumber:
            menu.add_command(label=[mori_individ[0], names.idsToName[mori_individ[1]]], 
                         command=lambda value=names.idsToName[mori_individ[1]]: self.listMoriVar.set(value))
