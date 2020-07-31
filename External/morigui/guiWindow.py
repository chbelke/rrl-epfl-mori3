import tkinter as tk
from threading import Thread
import threading
from termcolor import colored

# from runMqtt.wifi_host import wifi_host
from runMqtt.wirelessHost import WirelessHost

from morigui.frames.party import PartyFrame
from morigui.frames.pubg import PublishGlobal
from morigui.frames.publ import PublishLocal
from morigui.frames.startstop import StartStop
from morigui.frames.listudp import ListUDP
from morigui.frames.pubbulk import PublishBulk
from morigui.frames.pubbinary import PublishBinary


class MoriGui(tk.Frame):

    def __init__(self, master=None, *args, **kwargs):
        Thread(tk.Frame.__init__(self, master))

        self.master = master
        self.wifi_host = WirelessHost()
        self.wifi_host.start()

        self.numberConnected = tk.IntVar()
        self.numberConnected.set(self.wifi_host.getNumberConnected())
           
        self.createWidgets()
        self.pack()    

              
    def createWidgets(self):
        frame1 = tk.Frame(self)
        frame2 = tk.Frame(self, relief=tk.RAISED, borderwidth=1, 
            padx=5, pady=5)
        frame3 = tk.Frame(self)
        frame4 = tk.Frame(self)
        frame5 = tk.Frame(self)
        frame6 = tk.Frame(self)

        self.frame_party = PartyFrame(frame1)
        self.frame_start = StartStop(frame1, self.wifi_host)
        self.frame_pubg = PublishGlobal(frame2, self.wifi_host)
        self.frame_publ = PublishLocal(frame3, self, self.wifi_host)
        self.frame_udp = ListUDP(frame4, self, self.wifi_host)
        self.frame_pubbulk = PublishBulk(frame5, self.wifi_host)
        self.frame_binary = PublishBinary(frame6, self, self.wifi_host)

        frame5.pack(side="right", padx=10, pady=10, expand=True)
        frame1.pack(side="top", fill=tk.BOTH, expand=True, pady=5)
        frame4.pack(side="right", padx=10, pady=10, expand=True)
        frame6.pack(side="bottom", padx=10, pady=10, expand=True)
        frame2.pack(side="bottom", padx=10, pady=10, expand=True)
        frame3.pack(padx = 10)

            
    def updateConnected(self): #Updates the number of connected ESPs and the lists
        self.after(500, self.updateConnected)
        
        tmp = self.numberConnected.get()
        if tmp != self.wifi_host.getNumberConnected():
            self.numberConnected.set(self.wifi_host.getNumberConnected())
            espOrder = self.wifi_host.getEspOrder()
            esp = self.wifi_host.getEspIds()
            self.moriNumber = []
            print("")
            print(colored("Online ESPs: ", "blue") + colored(espOrder, "blue"))
            print("")
            for i in range(len(espOrder)):
                self.moriNumber.append(esp[espOrder[i]])

            self.frame_publ.update_menu(self.moriNumber)
            self.frame_binary.update_menu(self.moriNumber)
        self.frame_udp.update_udp()
