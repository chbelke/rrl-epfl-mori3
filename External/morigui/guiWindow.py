from tkinter import *
from morigui.mqttRun import MqttHost
from threading import Thread


class MoriGui(Frame):

    def say_hi(self):
        print("hi there, everyone!")

    def runMqtt(self):
        # self.mqtthost.start()
        self.mqtthost.run()

    def publishGlobal(self):
        text = self.pub_cmd.get()
        self.mqtthost.publishGlobal(text)
        print("Published \"" + text + "\" to esp/rec")
        self.pub_cmd.delete(0, 'end')

    def quitButton(self):
        print("Stopping MQTT")
        self.mqtthost.exit()

    def createWidgets(self):

        frame1 = Frame(self)
        frame1.pack(fill=X)

        self.hi_there = Button(frame1)
        self.hi_there["text"] = "Hello",
        self.hi_there["command"] = self.say_hi
        self.hi_there.pack(fill=X, padx=5, pady=5, expand=True)

        self.disp = Label(frame1, textvariable = self.mqtthost.printConnected())
        print(self.mqtthost.printConnected())
        self.disp.pack(fill=X, padx=5, pady=5, expand=True)

        frame2 = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        frame2.pack()

        self.pub_label = Label(frame2, text="Global message")
        self.pub_label.pack()
     
        self.Mqtt_pub = Button(frame2)
        self.Mqtt_pub["text"] = "Publish",
        self.Mqtt_pub["command"] = self.publishGlobal
        self.Mqtt_pub.pack({"side": "right"})

        self.pub_cmd = Entry(frame2, bd=1)
        self.pub_cmd.pack(fill=X, padx=5, expand=True) 

        frame3 = Frame(self, relief=RAISED, borderwidth=1, pady=5)
        frame3.pack()

        moriNumber = [1,2,3,4,5]
        self.listMoriLabel = Label(frame3, text="Mori Available")
        self.listMoriLabel.pack({"side": "left"})
        self.listMori = OptionMenu(frame3, moriNumber,moriNumber)
        self.listMori.pack(fill=X, side ="left", expand=True)



        bottomFrame = Frame(self)
        bottomFrame.pack(fill=BOTH, expand=True, pady=5)        

        self.QUIT = Button(bottomFrame)
        self.QUIT["text"] = "Stop"
        self.QUIT["fg"]   = "red"
        self.QUIT["command"] =  self.quitButton
        self.QUIT.pack({"side": "right"})

        self.Mqtt = Button(bottomFrame)
        self.Mqtt["text"] = "Start",
        self.Mqtt["command"] = self.runMqtt
        self.Mqtt.pack({"side": "right"})

    def __init__(self, master=None):
        self.mqtthost = MqttHost()
        self.mqtthost.start()
        Thread(Frame.__init__(self, master))
        self.createWidgets()
        self.pack()
        