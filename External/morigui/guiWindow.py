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
        #print("Published \"" + text + "\" to esp/" + number + "/rec")

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

        text = "com" + receiver + text # com is for communication

        self.mqtthost.publishLocal(sender,text)
        #print(colored("ESP" + sender + " will send ", "yellow") + text + colored(" to ESP" + receiver, "yellow"))
        self.interMoriComEntry.delete(0, 'end')

    def publishShapeCommand(self):
        startByte = "13"
        mode = "00"
        allocationByte = mode
        dataBytes = ""
        endByte = "150"

        self.currentMoriShape = self.mqtthost.getMoriShape()
        number = self.shapeCommandListMoriVar.get()
        if number == 'None':
            print("No Mori selected")
            return
        shapeChange = False
        for i in range(len(self.moriShapeSlider)):
            #print(self.moriShapeSlider[i].get())
            if self.moriShapeSlider[i].get() != self.currentMoriShape.get(number)[i]:
                shapeChange = True
                allocationByte = allocationByte + "1" #Shape change

                if i < 3: #Extention value
                    newShape = 1100 - self.moriShapeSlider[i].get() #Redefine the range for the Mori
                else: #Angle value
                    newShape = self.moriShapeSlider[i].get()
                
                #The following conditions are necessary to keep the same message size for all shape commands (size = 4)
                if(newShape >= 100):
                    dataBytes = dataBytes + "0" + str(newShape)
                elif(newShape >= 10 and self.moriShapeSlider[i].get() < 100):
                    dataBytes = dataBytes + "00" + str(newShape)
                elif(newShape >= 0 and self.moriShapeSlider[i].get() < 10):
                    dataBytes = dataBytes + "000" + str(newShape)
                elif(newShape > -10 and self.moriShapeSlider[i].get() < 0):
                    dataBytes = dataBytes + "-00" + str(abs(newShape))
                elif(newShape > -100 and self.moriShapeSlider[i].get() < -10):
                    dataBytes = dataBytes + "-0" + str(abs(newShape))
                else:
                    dataBytes = dataBytes + str(newShape)
            else:
                allocationByte = allocationByte + "0" #No shape change

        if shapeChange:
            command = startByte + " " + allocationByte + " " + dataBytes + " " + endByte
        else: #To avoid having two spaces
            command = startByte + " " + allocationByte + " " + endByte

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

                        if i < 3: #Extention value
                            newShape = 1100 - self.moriShapeSlider[ii].get() #Redefine the range for the Mori
                        else: #Angle value
                            newShape = self.moriShapeSlider[ii].get()

                        #The following conditions are necessary to keep the same message size for all shape commands (size = 4)
                        if(newShape >= 100):
                            dataBytes = dataBytes + "0" + str(newShape)
                        elif(newShape >= 10 and self.moriShapeSlider[i].get() < 100):
                            dataBytes = dataBytes + "00" + str(newShape)
                        elif(newShape >= 0 and self.moriShapeSlider[i].get() < 10):
                            dataBytes = dataBytes + "000" + str(newShape)
                        elif(newShape > -10 and self.moriShapeSlider[i].get() < 0):
                            dataBytes = dataBytes + "-00" + str(abs(newShape))
                        elif(newShape > -100 and self.moriShapeSlider[i].get() < -10):
                            dataBytes = dataBytes + "-0" + str(abs(newShape))
                        else:
                            dataBytes = dataBytes + str(newShape)
                    else:
                        allocationByte = allocationByte + "0" #No shape change
                if shapeChange:
                    command = startByte + " " + allocationByte + " " + dataBytes + " " + endByte
                else: #To avoid having two spaces
                    command = startByte + " " + allocationByte + " " + endByte
                message = "com" + leaderDict.get(number)[i] + command 
                #print("Published \"" + message + "\" to esp/" + leaderDict.get(number)[i] + "/rec")
                self.mqtthost.publishLocal(number,message)    
        
        #IntVar arrays cannot be simply manipulated
        self.shape0.set(self.moriShapeSlider[0].get())
        self.shape1.set(self.moriShapeSlider[1].get())
        self.shape2.set(self.moriShapeSlider[2].get())
        self.shape3.set(self.moriShapeSlider[3].get())
        self.shape4.set(self.moriShapeSlider[4].get())
        self.shape5.set(self.moriShapeSlider[5].get())
        
    def publishHandshake(self, leader, follower, leaderDict, type):
        if leader == 'None':
            print("No ESP selected to lead")
            return
        if follower == 'None':
            print("No Mori selected to follow")
            return
        if leader == follower:
            print("Same Mori selected")
            return

        #Check if the leader esp is already leading the follower esp
        #this is also done later in the process but doing this check here avoids the operations of another handshake
        if not(leaderDict.get(leader) is None): 
            for i in range(len(leaderDict.get(leader))):
                if leaderDict.get(leader)[i] == follower:
                    print("Leader ESP is already leading the follower Mori!")
                    return
        if not(leaderDict.get(follower) is None):
            for i in range(len(leaderDict.get(follower))):
                if leaderDict.get(follower)[i] == leader:
                    print("Leader Mori is following the follower Mori!")
                    return

        #"type" defines if this is a leader-follower or a controller-Mori handshake
        message = "hand" + "l" + type + follower # 'hand' is for handshake, 'l' is for leader

        print(colored("ESP" + leader + " will lead", "yellow") + colored(" ESP" + follower, "yellow"))
        self.mqtthost.publishLocal(leader,message)

    
    #Reset all leaders and followers (used to recreate another hierarchy)
    def resetHandshakes(self):
        print("No more leader and followers!")
        self.mqtthost.resetHandshakes()
        self.updateConnected #Important to reset the list

    def resetControl(self):
        print("No more controlers!")
        if self.UDPcommunication: #Reset the UDP and joystick control to "off"
            self.toggleUDP()
            self.controlUDPCheckbox.deselect()
        self.mqtthost.resetControllers()
        self.updateConnected #Important to reset the list

    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()

    def updateConnected(self): #Updates the number of connected ESPs and the lists
        #self.update_shape_sliders(False)
        self.after(1500, self.updateConnected)

        tmp = self.numberConnected.get()
        if tmp != self.mqtthost.getNumberConnected():
            self.numberConnected.set(self.mqtthost.getNumberConnected())
            espOrder = self.mqtthost.getEspOrder()
            esp = self.mqtthost.getEspIds()
            tmpList = []
            print("")
            print(colored("Online ESPs: ", "blue") + colored(espOrder, "blue"))
            print("")
            for i in range(len(espOrder)):
                tmpList.append(esp[espOrder[i]])


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
            controllerMenu = self.controllerList["menu"]
            controllerMenu.delete(0, "end")
            moriMenu = self.moriList["menu"]
            moriMenu.delete(0, "end")


            #Create all roll down menus
            self.moriNumber = tmpList
            for mori_individ in self.moriNumber:
                menu.add_command(label=mori_individ, 
                             command=lambda value=mori_individ[1]: self.listMoriVar.set(value))
                senderMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.senderListMoriVar.set(value))
                receiverMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.receiverListMoriVar.set(value))
                
                if mori_individ[0] == "contr":
                    controllerMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.controllerListVar.set(value))
                elif mori_individ[0] == "mori":
                    shapeCommandMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.shapeCommandListMoriVar.set(value))
                    handshakeLeaderMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.handshakeLeaderListVar.set(value))
                    handshakeFollowerMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.handshakeFollowerListVar.set(value))
                    moriMenu.add_command(label=mori_individ[1], 
                             command=lambda value=mori_individ[1]: self.moriListVar.set(value))
                else:
                    print(colored("Wrong role for esp: " + mori_individ[1] + " current role: " + mori_individ[0], "red"))
        
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

    def update_shape_sliders(self, compel):
        if self.joystickCommand or compel:
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

    def option_select(self, *args):
        print("ESP " + self.listMoriVar.get() + " selected.")

    def sender_option_select(self, *args):
        print("ESP " + self.senderListMoriVar.get() + " selected.")

    def receiver_option_select(self, *args):
        print("ESP " + self.receiverListMoriVar.get() + " selected.")

    def shape_command_option_select(self, *args):
        print("ESP " + self.shapeCommandListMoriVar.get() + " selected.")
        self.mqtthost.publishLocal(self.shapeCommandListMoriVar.get(), "shape")
        self.update_shape_sliders(True)

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

    def controller_option_select(self, *args):
        print("ESP " + self.controllerListVar.get() + " selected.")

    def mori_option_select(self, *args):
        print("ESP " + self.moriListVar.get() + " selected.")

    def toggleLead(self):
        #Change the leader status (to lead or not to lead :P) when the checkbox is toggled
        if self.leadDict.get(self.leaderListVar.get()) == True:
            self.leadDict[self.leaderListVar.get()] = False
        else:
            self.leadDict[self.leaderListVar.get()] = True

    def toggleUDP(self):
        #Enter if there is a mori-controler pair
        if len(self.mqtthost.getControllerOrder()) > 0: 
            #Activate UDP communication
            if not self.UDPcommunication:
                self.UDPcommunication = True
                self.mqtthost.toggleUDP()
            else:
                self.UDPcommunication = False
                self.mqtthost.toggleUDP()
                #Deactivate joystick control when UDP communication is disabled
                if self.joystickCommand: 
                    self.toggleJoystick()
                    self.joystickControlCheckbox.deselect()
        else:
            print("No controller-Mori pair defined")
            self.controlUDPCheckbox.deselect()

    def toggleJoystick(self):
        if not self.joystickCommand:
            if self.mqtthost.toggleJoystick(self.moriListVar.get(), True):
                self.joystickCommand = True
                #Activate UDP communication when joystick control is enabled
                if not self.UDPcommunication: 
                    self.toggleUDP()
                    self.controlUDPCheckbox.select()
            else:
                print("No controller-Mori pair defined")
                self.joystickControlCheckbox.deselect()
        else: 
            #Deactivate joystick control
            if self.mqtthost.toggleJoystick(self.moriListVar.get(), False):
                self.joystickCommand = False
            else:
                print("No controller-Mori pair defined")
                
    def createWidgets(self):
        frame_party = Frame(self)
        frame_pubg = Frame(self, relief=RAISED, borderwidth = 1, pady = 5)
        frame_listMori = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_interMoriCom = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriShapeCommand = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriHandshake = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_moriLead = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
        frame_controllerMori = Frame(self, relief = RAISED, borderwidth = 1, pady = 5)
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
        self.Stop.pack()

        self.Mqtt = Button(bottomFrame)
        self.Mqtt["text"] = "Start",
        self.Mqtt["command"] = self.runMqtt
        self.Mqtt.pack()


    # ------------------------------Frame 7------------------------------------ #
        self.moriHanshakeCmd0 = Label(frame_moriHandshake, text="HANDSHAKE")
        self.moriHanshakeCmd0.grid(row=0, column = 0, sticky = E)

        self.moriHandshakeCmd1 = Label(frame_moriHandshake, text="Leader: ")
        self.moriHandshakeCmd1.grid(row=1, column = 0)
        self.handshakeLeaderList = OptionMenu(frame_moriHandshake, self.handshakeLeaderListVar,*self.moriNumber)
        self.handshakeLeaderList.grid(row=1, column = 1, ipadx = 50, sticky = SW)

        self.moriHandshakeCmd2 = Label(frame_moriHandshake, text="Follower: ")
        self.moriHandshakeCmd2.grid(row=2, column = 0)
        self.handshakeFollowerList = OptionMenu(frame_moriHandshake, self.handshakeFollowerListVar,*self.moriNumber)
        self.handshakeFollowerList.grid(row=2, column = 1, ipadx = 50, sticky = SW)

        self.handshakeButton = Button(frame_moriHandshake)
        self.handshakeButton["text"] = "Link",
        self.handshakeButton.grid(row=3, column = 1, ipadx = 40, sticky = E)
        self.handshakeButton["command"] = lambda: self.publishHandshake(self.handshakeLeaderListVar.get(), self.handshakeFollowerListVar.get(), self.mqtthost.getLeaderIds(),'l')

        self.moriHanshakeReset = Button(frame_moriHandshake)
        self.moriHanshakeReset["text"] = "Reset"
        self.moriHanshakeReset.grid(row=0, column = 1, ipadx = 35, sticky = E)
        self.moriHanshakeReset["command"] = self.resetHandshakes


    # ------------------------------Frame 8------------------------------------ #
        self.leaderList = OptionMenu(frame_moriLead, self.leaderListVar,*self.moriNumber)
        self.leaderList.grid(row=0, column = 0, ipadx = 50)

        self.moriLeadCmd0 = Label(frame_moriLead, text="LEAD")
        self.moriLeadCmd0.grid(row=0, column = 2, ipadx = 20, sticky = W)

        self.moriLeadCheckbox = Checkbutton(frame_moriLead, command = self.toggleLead)
        self.moriLeadCheckbox.grid(row=0, column = 1)

        self.controlUDPCmd0 = Label(frame_moriLead, text="UDP Communication")
        self.controlUDPCmd0.grid(row=1, column = 0, ipadx = 20)
        self.controlUDPCheckbox = Checkbutton(frame_moriLead, command = self.toggleUDP)
        self.controlUDPCheckbox.grid(row=1, column = 1)

        self.joystickControlCmd0 = Label(frame_moriLead, text="Joystick commands")
        self.joystickControlCmd0.grid(row=2, column = 0, ipadx = 20)
        self.joystickControlCheckbox = Checkbutton(frame_moriLead, command = self.toggleJoystick)
        self.joystickControlCheckbox.grid(row=2, column = 1)


    # ------------------------------Frame 9------------------------------------ # 
        self.controllerList = OptionMenu(frame_controllerMori, self.controllerListVar,*self.moriNumber)
        self.controllerList.grid(row=1, column = 1, ipadx = 50)
        self.controllerMoriCmd0 = Label(frame_controllerMori, text="Controller")
        self.controllerMoriCmd0.grid(row=1, column = 0, ipadx = 20)

        self.moriList = OptionMenu(frame_controllerMori, self.moriListVar,*self.moriNumber)
        self.moriList.grid(row=2, column = 1, ipadx = 50)
        self.controllerMoriCmd1 = Label(frame_controllerMori, text="Mori")
        self.controllerMoriCmd1.grid(row=2, column = 0, ipadx = 20)

        self.controllerLink = Button(frame_controllerMori)
        self.controllerLink["text"] = "Connect"
        self.controllerLink.grid(row=4, column = 1, ipadx = 35, sticky = E)
        self.controllerLink["command"] =  lambda: self.publishHandshake(self.controllerListVar.get(), self.moriListVar.get(), self.mqtthost.getControllerIds(),'c')

        self.controlReset = Button(frame_controllerMori, text = "Disconnect", command = self.resetControl)
        self.controlReset.grid(row=0, column = 1, ipadx = 35, sticky = E)

 

        #frame_party.pack(fill=X)
        frame_moriShapeCommand.pack({"side": "right"})
        frame_controllerMori.pack({"side": "left"})
        frame_listMori.pack(ipadx = 10)
        frame_interMoriCom.pack()
        frame_moriHandshake.pack()
        frame_moriLead.pack()
        frame_pubg.pack()
        #bottomFrame.pack(fill=BOTH, expand=True, pady=5)
        

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

        self.controllerListVar = StringVar()
        self.controllerListVar.set(self.moriNumber[0])
        self.controllerListVar.trace('w', self.controller_option_select)

        self.moriListVar = StringVar()
        self.moriListVar.set(self.moriNumber[0])
        self.moriListVar.trace('w', self.mori_option_select)

        self.leadDict = {}
        self.UDPcommunication = False
        self.joystickCommand = False

        self.paaarty = self.partyOn()

        Thread(Frame.__init__(self, master))
        self.createWidgets()
        self.pack()
        

    def partyOn(self):
        return base64.b64decode(party).decode('UTF-8')