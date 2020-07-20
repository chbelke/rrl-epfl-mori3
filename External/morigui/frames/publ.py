import tkinter as tk
import base64
from termcolor import colored


class PublishLocal():

    def __init__(self, frame, parent, mqtthost):
        self.frame = frame

        self.mqtthost = mqtthost
        self.numberConnected = parent.numberConnected

        self.moriNumber = [None]
        self.listMoriVar = tk.StringVar()
        self.listMoriVar.set(self.moriNumber[0])
        self.listMoriVar.trace('w', self.option_select)        
        self.load()


    def load(self):
        self.listMoriLabel = tk.Label(self.frame, text="Mori Available:")
        self.listMoriLabel.pack({"side": "left"})
        self.disp = tk.Label(self.frame, textvariable=str(self.numberConnected))
        self.disp.pack({"side": "left"}, padx=10)

        self.listMori = tk.OptionMenu(self.frame, self.listMoriVar,*self.moriNumber)
        self.listMori.pack({"side": "top"}, fill=tk.X, expand=True, pady=5)

        self.pub_loc_entry = tk.Entry(self.frame, bd=1)
        self.pub_loc_entry.pack({"side": "right"}, fill=tk.X, padx=5, expand=True)
        self.pub_loc_button = tk.Button(self.frame)
        self.pub_loc_button["text"] = "Publish",
        self.pub_loc_button["command"] = lambda: self.publishLocal()
        self.pub_loc_button.pack({"side": "right"})   


    def option_select(self, *args):
        print("ESP " + self.listMoriVar.get() + " selected.")


    def publishLocal(self):
        text = self.pub_loc_entry.get()
        if(text is ""):
            print("Please write a command")
            return
        number = self.listMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return
        self.mqtthost.publishLocal(text, number)
        self.pub_loc_entry.delete(0, 'end')


    def update_menu(self, list_of_modules):
        self.moriNumber = list_of_modules
        print(self.moriNumber)
        menu = self.listMori["menu"]
        menu.delete(0, "end")
        # moriMenu = self.moriList["menu"]
        # moriMenu.delete(0, "end")


        #Create all roll down menus
        self.list_of_modules = list_of_modules
        for mori_individ in self.moriNumber:
            menu.add_command(label=mori_individ, 
                         command=lambda value=mori_individ[1]: self.listMoriVar.set(value))                    