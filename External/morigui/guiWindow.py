from tkinter import *
from morigui.mqttRun import MqttHost
from threading import Thread
from termcolor import colored
import base64
from morigui.localSettings import *


class MoriGui(Frame):

    def say_hi(self):
        print(colored(self.paaarty,'green'))

    def runMqtt(self):
        # self.mqtthost.start()
        self.mqtthost.run()

    def publishGlobal(self):
        text = self.pub_cmd.get()
        self.mqtthost.publishGlobal(text)
        print("Published \"" + text + "\" to esp/rec")
        self.pub_cmd.delete(0, 'end')


    def publishLocal(self):
        text = self.pub_loc_entry.get()
        if(text is ""):
            print("Please write a command")
            return
        number = self.listMoriVar.get()
        if number == 'None':
            print("No Mori Selected")
            return
        self.mqtthost.publishLocal(number,text)
        print("Published \"" + text + "\" to esp/" + number + "/rec")
        self.pub_loc_entry.delete(0, 'end')
            


    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()

    def updateConnected(self):
        self.after(2000, self.updateConnected)
        tmp = self.numberConnected.get()
        if tmp != self.mqtthost.getNumberConnected():
            self.numberConnected.set(self.mqtthost.getNumberConnected())
            menu = self.listMori["menu"]
            menu.delete(0, "end")
            moriOrder = self.mqtthost.getMoriOrder()
            mori = self.mqtthost.getMoriIds()
            tmpList = []
            for i in range(len(mori)):
                tmpList.append(mori[moriOrder[i]])

            self.moriNumber = tmpList
            for mori_individ in self.moriNumber:
                menu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.listMoriVar.set(value))

    def update_option_menu(self):
        menu = self.moriNumber["menu"]
        menu.delete(0, "end")
        for string in self.listMori:
            menu.add_command(label=string, 
                             command=lambda value=string: self.listMoriVar.set(value))

    def option_select(self, *args):
        print(self.listMoriVar.get())


    def createWidgets(self):

        frame_party = Frame(self)
        frame_pubg = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        frame_listMori = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        bottomFrame = Frame(self)

    # ------------------------------Frame 1------------------------------------ #        

        self.hi_there = Button(frame_party)
        self.hi_there["text"] = "Party",
        self.hi_there["command"] = self.say_hi
        self.hi_there.pack(fill=X, padx=5, pady=5, expand=True)

    # ------------------------------Frame 2------------------------------------ #        

        self.pub_label = Label(frame_pubg, text="Publish Global Message")
        self.pub_label.pack()
     
        self.Mqtt_pub = Button(frame_pubg)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = self.publishGlobal
        self.Mqtt_pub.pack({"side": "right"}, fill=X, expand=True)

        self.pub_cmd = Entry(frame_pubg, bd=1)
        self.pub_cmd.pack(fill=X, expand=True, padx=5) 

    # ------------------------------Frame 3------------------------------------ #        

        self.listMoriLabel = Label(frame_listMori, text="Mori Available:")
        self.listMoriLabel.pack({"side": "left"})
        self.disp = Label(frame_listMori, textvariable=str(self.numberConnected))
        self.disp.pack({"side": "left"}, padx=10)

        self.listMori = OptionMenu(frame_listMori, self.listMoriVar,*self.moriNumber)
        self.listMori.pack(fill=X, expand=True, pady=5)

        self.pub_loc_entry = Entry(frame_listMori, bd=1)
        self.pub_loc_entry.pack({"side": "right"}, fill=X, padx=5, expand=True)
        self.pub_loc_button = Button(frame_listMori)
        self.pub_loc_button["text"] = "Publish",
        self.pub_loc_button["command"] = self.publishLocal
        self.pub_loc_button.pack({"side": "right"})     

    # ------------------------------Frame 4------------------------------------ #        

        self.Stop = Button(bottomFrame)
        self.Stop["text"] = "Stop"
        self.Stop["fg"]   = "red"
        self.Stop["command"] =  self.quitButton
        self.Stop.pack({"side": "right"})

        self.Mqtt = Button(bottomFrame)
        self.Mqtt["text"] = "Start",
        self.Mqtt["command"] = self.runMqtt
        self.Mqtt.pack({"side": "right"})

        frame_party.pack(fill=X)
        frame_listMori.pack()
        frame_pubg.pack()
        bottomFrame.pack(fill=BOTH, expand=True, pady=5)


    def __init__(self, master=None):
        self.mqtthost = MqttHost()
        self.mqtthost.start()
        
        self.numberConnected = IntVar()
        self.numberConnected.set(self.mqtthost.getNumberConnected())
        
        self.moriNumber = [None]
        self.listMoriVar = StringVar()
        self.listMoriVar.set(self.moriNumber[0])
        self.listMoriVar.trace('w', self.option_select)

        self.paaarty = self.partyOn()

        Thread(Frame.__init__(self, master))
        self.createWidgets()
        self.pack()
        

    def partyOn(self):
        return base64.b64decode(party).decode('UTF-8')