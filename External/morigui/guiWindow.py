from tkinter import *
from morigui.mqttRun import MqttHost
from threading import Thread
from termcolor import colored
import base64
from morigui.localSettings import *
import threading


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
            print("No Mori selected")
            return
        self.mqtthost.publishLocal(number,text)
        print("Published \"" + text + "\" to esp/" + number + "/rec")
        self.pub_loc_entry.delete(0, 'end')

    def publishCommunication(self):
        text = self.interMoriComEntry.get()
        if(text is ""):
            print("Please write a command")
            return

        sender = self.senderListMoriVar.get()
        receiver = self.receiverListMoriVar.get()
        if sender == 'None':
            print("No Mori selected to send")
            return
        if receiver == 'None':
            print("No Mori selected to receive")
            return

        text = "c" + receiver + text

        self.mqtthost.publishLocal(sender,text)
        print(colored("ESP " + sender + " will send ", "yellow") + text + colored(" to ESP " + receiver, "yellow"))
        self.interMoriComEntry.delete(0, 'end')

    def publishShapeCommand(self):
        startByte = "01"
        mode = "00"
        allocationByte = mode
        dataBytes = ""
        endByte = "150"
        allocation = 0;

        number = self.shapeCommandListMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return

        for i in range(len(self.moriShapeSlider)):
            print(self.moriShapeSlider[i].get())
            if self.moriShapeSlider[i].get() != self.currentMoriShape.get(self.shapeCommandListMoriVar.get())[i]:
                allocationByte = allocationByte + "1"
                if(self.moriShapeSlider[i].get() > 9 and self.moriShapeSlider[i].get() < 100):
                    dataBytes = dataBytes + "0" + str(self.moriShapeSlider[i].get()) #To keed the same dimensions for all commands
                elif(self.moriShapeSlider[i].get() >= 0 and self.moriShapeSlider[i].get() < 10):
                    dataBytes = dataBytes + "00" + str(self.moriShapeSlider[i].get()) #To keed the same dimensions for all commands
                elif(self.moriShapeSlider[i].get() > -10 and self.moriShapeSlider[i].get() < 0):
                    dataBytes = dataBytes + "-0" + str(abs(self.moriShapeSlider[i].get())) #To keed the same dimensions for all commands
                else:
                    dataBytes = dataBytes + str(self.moriShapeSlider[i].get()) #What should be sent to the ESP??
            else:
                allocationByte = allocationByte + "0"

        command = startByte + " " + allocationByte + " " + dataBytes + " " + endByte
        print(command)
        self.mqtthost.publishLocal(number,command)

        
            
    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()

    def updateConnected(self):
        self.after(2000, self.updateConnected)
        tmp = self.numberConnected.get()
        if tmp != self.mqtthost.getNumberConnected():
            self.numberConnected.set(self.mqtthost.getNumberConnected())
            moriOrder = self.mqtthost.getMoriOrder()
            mori = self.mqtthost.getMoriIds()
            tmpList = []
            print("")
            print(colored("Online ESPs: ", "blue") + colored(moriOrder, "blue"))
            print("")
            for i in range(len(moriOrder)):
                tmpList.append(mori[moriOrder[i]])

            menu = self.listMori["menu"]
            menu.delete(0, "end")
            senderMenu = self.senderListMori["menu"]
            senderMenu.delete(0, "end")
            receiverMenu = self.receiverListMori["menu"]
            receiverMenu.delete(0, "end")
            shapeCommandMenu = self.shapeCommandListMori["menu"]
            shapeCommandMenu.delete(0, "end")

            self.moriNumber = tmpList
            for mori_individ in self.moriNumber:
                menu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.listMoriVar.set(value))
                senderMenu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.senderListMoriVar.set(value))
                receiverMenu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.receiverListMoriVar.set(value))
                shapeCommandMenu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.shapeCommandListMoriVar.set(value))

    def update_option_menu(self):
        menu = self.moriNumber["menu"]
        menu.delete(0, "end")
        for string in self.listMori:
            menu.add_command(label=string, 
                             command=lambda value=string: self.listMoriVar.set(value))

    def option_select(self, *args):
        print("ESP " + self.listMoriVar.get() + " selected.")

    def sender_option_select(self, *args):
        print("ESP " + self.senderListMoriVar.get() + " selected.")

    def receiver_option_select(self, *args):
        print("ESP " + self.receiverListMoriVar.get() + " selected.")

    def shape_command_option_select(self, *args):
        print("ESP " + self.shapeCommandListMoriVar.get() + " selected.")
        #self.mqtthost.publishLocal(self.shapeCommandListMoriVar.get(), "shape")
        self.currentMoriShape[self.shapeCommandListMoriVar.get()] = self.mqtthost.getMoriShape(self.shapeCommandListMoriVar.get())
        for i in range(len(self.currentMoriShape.get(self.shapeCommandListMoriVar.get()))):
            self.moriShapeSlider[i].set(self.currentMoriShape.get(self.shapeCommandListMoriVar.get())[i])


    def createWidgets(self):

        frame_party = Frame(self)
        frame_pubg = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        frame_listMori = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        frame_interMoriCom = Frame(self, relief=RAISED, borderwidth=1, pady = 5)
        frame_moriShapeCommand = Frame(self, relief=RAISED, borderwidth=1, pady = 5)
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


        self.interMoriCom1 = Label(frame_interMoriCom, text="From")
        self.interMoriCom1.grid(row=0, column = 0, sticky = S)
        self.senderListMori = OptionMenu(frame_interMoriCom, self.senderListMoriVar,*self.moriNumber)
        self.senderListMori.grid(row=1, column = 0,  ipadx = 50, sticky = S)

        self.interMoriCom2 = Label(frame_interMoriCom, text="To")
        self.interMoriCom2.grid(row=2, column = 0, sticky = S)
        self.receiverListMori = OptionMenu(frame_interMoriCom, self.receiverListMoriVar,*self.moriNumber)
        self.receiverListMori.grid(row=3, column = 0, ipadx = 50, sticky = S)

        self.interMoriComEntry = Entry(frame_interMoriCom, bd=1)
        self.interMoriComEntry.grid(row=1, column = 7, sticky = S)

        self.interMoriComButton = Button(frame_interMoriCom)
        self.interMoriComButton["text"] = "Publish",
        self.interMoriComButton.grid(row=2, column = 7, sticky = S)
        self.interMoriComButton["command"] = self.publishCommunication
    # ------------------------------Frame 5------------------------------------ #        

        nbrShapeCommands = 6
        extensionMin = 200
        extensionMax = 900
        angleMin = -120
        angleMax = 120
        self.moriShapeSlider = [None] * nbrShapeCommands
        self.currentMoriShape = {}

        self.moriShapeCmd0 = Label(frame_moriShapeCommand, text="SHAPE COMMANDS")
        self.moriShapeCmd0.grid(row=0, column = 1, sticky = S)

        self.moriShapeCmd1 = Label(frame_moriShapeCommand, text="MORI:")
        self.moriShapeCmd1.grid(row=1, column = 0, sticky = S)
        self.shapeCommandListMori = OptionMenu(frame_moriShapeCommand, self.shapeCommandListMoriVar,*self.moriNumber)
        self.shapeCommandListMori.grid(row=1, column = 1,  ipadx = 50, sticky = S)

        self.moriShapeCmd2 = Label(frame_moriShapeCommand, text="Extension A:")
        self.moriShapeCmd2.grid(row=2, column = 0, sticky = S)
        self.moriShapeCmd3 = Label(frame_moriShapeCommand, text="Extension B:")
        self.moriShapeCmd3.grid(row=3, column = 0, sticky = S)
        self.moriShapeCmd4 = Label(frame_moriShapeCommand, text="Extension C:")
        self.moriShapeCmd4.grid(row=4, column = 0, sticky = S)
        self.moriShapeCmd5 = Label(frame_moriShapeCommand, text="Angle Alpha:")
        self.moriShapeCmd5.grid(row=5, column = 0, sticky = S)
        self.moriShapeCmd6 = Label(frame_moriShapeCommand, text="Angle Beta:")
        self.moriShapeCmd6.grid(row=6, column = 0, sticky = S)
        self.moriShapeCmd7 = Label(frame_moriShapeCommand, text="Angle Gamma:")
        self.moriShapeCmd7.grid(row=7, column = 0, sticky = S)

        self.moriShapeSlider[0] = Scale(frame_moriShapeCommand, from_= extensionMin, orient = HORIZONTAL, length = 150, to = extensionMax)
        self.moriShapeSlider[0].grid(row = 2, column = 1, sticky = S)
        self.moriShapeSlider[1] = Scale(frame_moriShapeCommand, from_= extensionMin, orient = HORIZONTAL, length = 150, to = extensionMax)
        self.moriShapeSlider[1].grid(row = 3, column = 1, sticky = S)
        self.moriShapeSlider[2] = Scale(frame_moriShapeCommand, from_= extensionMin, orient = HORIZONTAL, length = 150, to = extensionMax)
        self.moriShapeSlider[2].grid(row = 4, column = 1, sticky = S)
        self.moriShapeSlider[3] = Scale(frame_moriShapeCommand, from_= angleMin, orient = HORIZONTAL, length = 150, to = angleMax)
        self.moriShapeSlider[3].grid(row = 5, column = 1, sticky = S)
        self.moriShapeSlider[4] = Scale(frame_moriShapeCommand, from_= angleMin, orient = HORIZONTAL, length = 150, to = angleMax)
        self.moriShapeSlider[4].grid(row = 6, column = 1, sticky = S)
        self.moriShapeSlider[5] = Scale(frame_moriShapeCommand, from_= angleMin, orient = HORIZONTAL, length = 150, to = angleMax)
        self.moriShapeSlider[5].grid(row = 7, column = 1, sticky = S)

        self.moriShapeCommandButton = Button(frame_moriShapeCommand)
        self.moriShapeCommandButton["text"] = "Publish",
        self.moriShapeCommandButton.grid(row=8, column = 1, sticky = S)
        self.moriShapeCommandButton["command"] = self.publishShapeCommand
    # ------------------------------Frame 6------------------------------------ # 


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
        frame_moriShapeCommand.pack({"side": "right"})
        frame_listMori.pack(ipadx = 10)
        frame_interMoriCom.pack()
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

        self.senderListMoriVar = StringVar()
        self.senderListMoriVar.set(self.moriNumber[0])
        self.senderListMoriVar.trace('w', self.sender_option_select)

        self.receiverListMoriVar = StringVar()
        self.receiverListMoriVar.set(self.moriNumber[0])
        self.receiverListMoriVar.trace('w', self.receiver_option_select)

        self.shapeCommandListMoriVar = StringVar()
        self.shapeCommandListMoriVar.set(self.moriNumber[0])
        self.shapeCommandListMoriVar.trace('w', self.shape_command_option_select)

        self.paaarty = self.partyOn()

        Thread(Frame.__init__(self, master))
        self.createWidgets()
        self.pack()
        

    def partyOn(self):
        return base64.b64decode(party).decode('UTF-8')