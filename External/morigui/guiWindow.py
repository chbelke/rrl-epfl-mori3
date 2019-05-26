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

        leaderDict = self.mqtthost.getLeaderIds()
        #Enter if the ESP has follower and it is asked to lead
        if (not(leaderDict.get(number) is None)) and self.leadDict.get(self.leaderListVar.get()):
            for i in range(len(leaderDict.get(number))):
                message = "com" + leaderDict.get(number)[i] + text
                print("Published \"" + message + "\" to esp/" + leaderDict.get(number)[i] + "/rec")
                self.mqtthost.publishLocal(number,message)

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

        text = "com" + receiver + text # c is for communication

        self.mqtthost.publishLocal(sender,text)
        print(colored("ESP" + sender + " will send ", "yellow") + text + colored(" to ESP" + receiver, "yellow"))
        self.interMoriComEntry.delete(0, 'end')

    def publishShapeCommand(self):
        startByte = "01"
        mode = "00"
        allocationByte = mode
        dataBytes = ""
        endByte = "150"

        number = self.shapeCommandListMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return
        shapeChange = False
        for i in range(len(self.moriShapeSlider)):
            #print(self.moriShapeSlider[i].get())
            if self.moriShapeSlider[i].get() != self.currentMoriShape.get(self.shapeCommandListMoriVar.get())[i]:
                shapeChange = True
                allocationByte = allocationByte + "1" #Shape change

                #The following conditions are necessary to keep the same message size for all shape commands (size = 4)
                if(self.moriShapeSlider[i].get() >= 100):
                    dataBytes = dataBytes + "0" + str(self.moriShapeSlider[i].get())
                elif(self.moriShapeSlider[i].get() >= 10 and self.moriShapeSlider[i].get() < 100):
                    dataBytes = dataBytes + "00" + str(self.moriShapeSlider[i].get())
                elif(self.moriShapeSlider[i].get() >= 0 and self.moriShapeSlider[i].get() < 10):
                    dataBytes = dataBytes + "000" + str(self.moriShapeSlider[i].get())
                elif(self.moriShapeSlider[i].get() > -10 and self.moriShapeSlider[i].get() < 0):
                    dataBytes = dataBytes + "-00" + str(abs(self.moriShapeSlider[i].get()))
                elif(self.moriShapeSlider[i].get() > -100 and self.moriShapeSlider[i].get() < -10):
                    dataBytes = dataBytes + "-0" + str(abs(self.moriShapeSlider[i].get()))
                else:
                    dataBytes = dataBytes + str(self.moriShapeSlider[i].get())
            else:
                allocationByte = allocationByte + "0" #No shape change

        if shapeChange:
            command = startByte + " " + allocationByte + " " + dataBytes + " " + endByte
        else: #To avoid having two spaces
            command = startByte + " " + allocationByte + " " + endByte
        print("Shape command: " + command)
        self.mqtthost.publishLocal(number,command)

        leaderDict = self.mqtthost.getLeaderIds()
        #Enter if the ESP has follower and it is asked to lead
        if (not(leaderDict.get(number) is None)) and self.leadDict.get(self.leaderListVar.get()):
            for i in range(len(leaderDict.get(number))):
                dataBytes = ""
                allocationByte = mode
                shapeChange = False
                for ii in range(len(self.moriShapeSlider)):
                    #print(self.moriShapeSlider[i].get())
                    if self.moriShapeSlider[ii].get() != self.currentMoriShape.get(leaderDict.get(number)[i])[ii]:
                        shapeChange = True
                        allocationByte = allocationByte + "1" #Shape change

                        #The following conditions are necessary to keep the same message size for all shape commands (size = 4)
                        if(self.moriShapeSlider[ii].get() >= 100):
                            dataBytes = dataBytes + "0" + str(self.moriShapeSlider[ii].get())
                        elif(self.moriShapeSlider[ii].get() >= 10 and self.moriShapeSlider[ii].get() < 100):
                            dataBytes = dataBytes + "00" + str(self.moriShapeSlider[ii].get())
                        elif(self.moriShapeSlider[ii].get() >= 0 and self.moriShapeSlider[ii].get() < 10):
                            dataBytes = dataBytes + "000" + str(self.moriShapeSlider[ii].get())
                        elif(self.moriShapeSlider[ii].get() > -10 and self.moriShapeSlider[ii].get() < 0):
                            dataBytes = dataBytes + "-00" + str(abs(self.moriShapeSlider[ii].get()))
                        elif(self.moriShapeSlider[ii].get() > -100 and self.moriShapeSlider[ii].get() < -10):
                            dataBytes = dataBytes + "-0" + str(abs(self.moriShapeSlider[ii].get()))
                        else:
                            dataBytes = dataBytes + str(self.moriShapeSlider[ii].get())
                    else:
                        allocationByte = allocationByte + "0" #No shape change

                if shapeChange:
                    command = startByte + " " + allocationByte + " " + dataBytes + " " + endByte
                else: #To avoid having two spaces
                    command = startByte + " " + allocationByte + " " + endByte
                message = "com" + leaderDict.get(number)[i] + command
                print("Published \"" + message + "\" to esp/" + leaderDict.get(number)[i] + "/rec")
                self.mqtthost.publishLocal(number,message)
        
        #IntVar arrays cannot be simply manipulated
        self.shape0.set(self.moriShapeSlider[0].get())
        self.shape1.set(self.moriShapeSlider[1].get())
        self.shape2.set(self.moriShapeSlider[2].get())
        self.shape3.set(self.moriShapeSlider[3].get())
        self.shape4.set(self.moriShapeSlider[4].get())
        self.shape5.set(self.moriShapeSlider[5].get())
        
    def publishHandshake(self):
        leader = self.handshakeLeaderListVar.get()
        follower = self.handshakeFollowerListVar.get()
        if leader == 'None':
            print("No Mori selected to lead")
            return
        if follower == 'None':
            print("No Mori selected to follow")
            return
        if leader == follower:
            print("Same Mori selected")
            return

        leaderDict = self.mqtthost.getLeaderIds()

        #Check if the leader mori is already leading the follower mori
        #this is also done later in the process but doing this check here avoids the operations of another handshake
        if not(leaderDict.get(self.handshakeLeaderListVar.get()) is None): 
            print(leaderDict)
            print(len(leaderDict.get(self.handshakeLeaderListVar.get())))
            for i in range(len(leaderDict.get(self.handshakeLeaderListVar.get()))):
                if leaderDict.get(self.handshakeLeaderListVar.get())[i] == self.handshakeFollowerListVar.get():
                    print("Leader Mori is already leading the follower Mori!")
                    return
        if not(leaderDict.get(self.handshakeFollowerListVar.get()) is None):
            for i in range(len(leaderDict.get(self.handshakeFollowerListVar.get()))):
                if leaderDict.get(self.handshakeFollowerListVar.get())[i] == self.handshakeLeaderListVar.get():
                    print("Leader Mori is following the follower Mori!")
                    return

        message = "ha" + "l" + follower # 'ha' is for handshake, 'l' is for leader

        print(colored("ESP" + leader + " will lead", "yellow") + colored(" ESP" + follower, "yellow"))
        self.mqtthost.publishLocal(leader,message)

    
    #Reset all leaders and followers (used to recreate another hierarchy)
    def resetHandshakes(self):
        print("No more leader and followers!")
        self.mqtthost.resetHandshakes()
        self.updateConnected #Important to reset the list

    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()

    def updateConnected(self): #Updates the number of connected ESPs and the lists
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
            handshakeLeaderMenu = self.handshakeLeaderList["menu"]
            handshakeLeaderMenu.delete(0, "end")
            handshakeFollowerMenu = self.handshakeFollowerList["menu"]
            handshakeFollowerMenu.delete(0, "end")
            #Create all roll down menus
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
                handshakeLeaderMenu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.handshakeLeaderListVar.set(value))
                handshakeFollowerMenu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ: self.handshakeFollowerListVar.set(value))
        
        #Similar loop for the leader - follower widget (roll down menu)
        tmp = self.numberLeaders.get()
        if tmp != len(self.mqtthost.getLeaderOrder()):
            self.numberLeaders.set(len(self.mqtthost.getLeaderOrder()))
            tmpList = self.mqtthost.getLeaderOrder()

            leaderMenu = self.leaderList["menu"]
            leaderMenu.delete(0, "end")
            for leader_individ in tmpList:
                leaderMenu.add_command(label=leader_individ, 
                             command=lambda value=leader_individ: self.leaderListVar.set(value))
                if self.leadDict.get(leader_individ) is None:
                    self.leadDict[leader_individ] = True #The leader will lead by default


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
        self.currentMoriShape = self.mqtthost.getMoriShape()
        for i in range(len(self.currentMoriShape.get(self.shapeCommandListMoriVar.get()))):
            self.moriShapeSlider[i].set(self.currentMoriShape.get(self.shapeCommandListMoriVar.get())[i])
        #IntVar arrays cannot be simply manipulated
        self.shape0.set(self.moriShapeSlider[0].get())
        self.shape1.set(self.moriShapeSlider[1].get())
        self.shape2.set(self.moriShapeSlider[2].get())
        self.shape3.set(self.moriShapeSlider[3].get())
        self.shape4.set(self.moriShapeSlider[4].get())
        self.shape5.set(self.moriShapeSlider[5].get())

    def handshake_leader_option_select(self, *args):
        print("ESP " + self.handshakeLeaderListVar.get() + " selected.")

    def handshake_follower_option_select(self, *args):
        print("ESP " + self.handshakeFollowerListVar.get() + " selected.")

    def leader_option_select(self, *args):
        print("ESP " + self.leaderListVar.get() + " selected.")
        #Set checkbox to the previously saved value (default is checked (leader will lead))
        if self.leadDict.get(self.leaderListVar.get()): 
            self.moriLeadCheckbox.select()
        else:
            self.moriLeadCheckbox.deselect()

    def toggleLead(self):
        #Change the leader status (to lead or not to lead :P) when the checkbox is toggled
        if self.leadDict.get(self.leaderListVar.get()) == True:
            self.leadDict[self.leaderListVar.get()] = False
        else:
            self.leadDict[self.leaderListVar.get()] = True


    def createWidgets(self):
        frame_party = Frame(self)
        frame_pubg = Frame(self, relief=RAISED, borderwidth = 1, pady = 5)
        frame_listMori = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_interMoriCom = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriShapeCommand = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriHandshake = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriLead = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
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
        
        #IntVar arrays cannot be simply manipulated
        self.shape0 = IntVar()
        self.shape0.set(extensionMin)
        self.shape1 = IntVar()
        self.shape1.set(extensionMin)
        self.shape2 = IntVar()
        self.shape2.set(extensionMin)
        self.shape3 = IntVar()
        self.shape3.set((angleMin+angleMax)/2)
        self.shape4 = IntVar()
        self.shape4.set((angleMin+angleMax)/2)
        self.shape5 = IntVar()
        self.shape5.set((angleMin+angleMax)/2)

        self.moriShapeCmd0 = Label(frame_moriShapeCommand, text="SHAPE COMMANDS")
        self.moriShapeCmd0.grid(row=0, column = 1, sticky = S)
        self.moriShapeCmd0 = Label(frame_moriShapeCommand, text="Current")
        self.moriShapeCmd0.grid(row=1, column = 3, sticky = N, pady = 15)
        self.moriShapeCmd0 = Label(frame_moriShapeCommand, text="Shape")
        self.moriShapeCmd0.grid(row=1, column = 3, sticky = S)

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

        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape0))
        self.moriCurrentShape.grid(row = 2, column = 3, sticky = S)
        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape1))
        self.moriCurrentShape.grid(row = 3, column = 3, sticky = S)
        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape2))
        self.moriCurrentShape.grid(row = 4, column = 3, sticky = S)
        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape3))
        self.moriCurrentShape.grid(row = 5, column = 3, sticky = S)
        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape4))
        self.moriCurrentShape.grid(row = 6, column = 3, sticky = S)
        self.moriCurrentShape = Label(frame_moriShapeCommand, textvariable=str(self.shape5))
        self.moriCurrentShape.grid(row = 7, column = 3, sticky = S)

        self.moriShapeCommandButton = Button(frame_moriShapeCommand)
        self.moriShapeCommandButton["text"] = "Publish",
        self.moriShapeCommandButton.grid(row=8, column = 1, sticky = N)
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
    # ------------------------------Frame 7------------------------------------ #


        self.moriHanshakeCmd0 = Label(frame_moriHandshake, text="HANDSHAKE")
        self.moriHanshakeCmd0.grid(row=0, column = 0, sticky = E)

        self.moriHandshakeCmd1 = Label(frame_moriHandshake, text="Leader: ")
        self.moriHandshakeCmd1.grid(row=1, column = 0)
        self.handshakeLeaderList = OptionMenu(frame_moriHandshake, self.handshakeLeaderListVar,*self.moriNumber)
        self.handshakeLeaderList.grid(row=1, column = 1, ipadx = 50, sticky = SW)

        self.moriHandshakeCmd2 = Label(frame_moriHandshake, text="Follower: ")
        self.moriHandshakeCmd2.grid(row=3, column = 0)
        self.handshakeFollowerList = OptionMenu(frame_moriHandshake, self.handshakeFollowerListVar,*self.moriNumber)
        self.handshakeFollowerList.grid(row=3, column = 1, ipadx = 50, sticky = SW)

        self.handshakeButton = Button(frame_moriHandshake)
        self.handshakeButton["text"] = "Link",
        self.handshakeButton.grid(row=4, column = 1, ipadx = 40, sticky = E)
        self.handshakeButton["command"] = self.publishHandshake

        self.moriHanshakeReset = Button(frame_moriHandshake)
        self.moriHanshakeReset["text"] = "Reset"
        self.moriHanshakeReset.grid(row=0, column = 1, ipadx = 35, sticky = E)
        self.moriHanshakeReset["command"] = self.resetHandshakes
    # ------------------------------Frame 8------------------------------------ # 


        self.leaderList = OptionMenu(frame_moriLead, self.leaderListVar,*self.moriNumber)
        self.leaderList.grid(row=0, column = 0, ipadx = 50)

        self.moriLeadCmd0 = Label(frame_moriLead, text="LEAD?")
        self.moriLeadCmd0.grid(row=0, column = 1, ipadx = 20)

        self.moriLeadCheckbox = Checkbutton(frame_moriLead, command = self.toggleLead)
        self.moriLeadCheckbox.grid(row=0, column = 2,  sticky = W)
    # ------------------------------Frame 9------------------------------------ # 
 

        frame_party.pack(fill=X)
        frame_moriShapeCommand.pack({"side": "right"})
        frame_listMori.pack(ipadx = 10)
        frame_interMoriCom.pack()
        frame_moriHandshake.pack()
        frame_moriLead.pack()
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

        self.handshakeLeaderListVar = StringVar()
        self.handshakeLeaderListVar.set(self.moriNumber[0])
        self.handshakeLeaderListVar.trace('w', self.handshake_leader_option_select)

        self.handshakeFollowerListVar = StringVar()
        self.handshakeFollowerListVar.set(self.moriNumber[0])
        self.handshakeFollowerListVar.trace('w', self.handshake_follower_option_select)

        self.numberLeaders = IntVar()
        self.numberLeaders.set(0)

        self.leaderListVar = StringVar()
        self.leaderListVar.set(self.moriNumber[0])
        self.leaderListVar.trace('w', self.leader_option_select)

        self.leadDict = {}

        self.paaarty = self.partyOn()

        Thread(Frame.__init__(self, master))
        self.createWidgets()
        self.pack()
        

    def partyOn(self):
        return base64.b64decode(party).decode('UTF-8')