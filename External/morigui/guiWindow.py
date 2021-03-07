import tkinter as tk
from threading import Thread
import threading
from termcolor import colored
import copy

# from runMqtt.wifi_host import wifi_host
from runMqtt.wirelessHost import WirelessHost

from morigui.GraphHost import GraphHost
from morigui.GraphHost import GraphFrame

from morigui.frames.party import PartyFrame
from morigui.frames.pubg import PublishGlobal
from morigui.frames.publ import PublishLocal
from morigui.frames.startstop import StartStop
from morigui.frames.listudp import ListUDP
from morigui.frames.pubbulk import PublishBulk
from morigui.frames.pubbinary import PublishBinary
from morigui.frames.datalog import DataLog
from morigui.frames.pinglog import PingLog
from morigui.frames.loadfile import LoadFile
from morigui.frames.inputflag import InputFlag


class MoriGui(tk.Frame):

    def __init__(self, master=None, *args, **kwargs):
        Thread(tk.Frame.__init__(self, master))

        self.master = master
        self.wifi_host = WirelessHost()
        self.wifi_host.start()

        self.numberConnected = tk.IntVar()
        self.numberConnected.set(0)
           
        self.tmpEspIds = {}          

        self.createWidgets()
        self.pack()    

              
    def createWidgets(self):
        frame1 = tk.Frame(self)
        frame2 = tk.Frame(self)
        frame4 = tk.Frame(self)
        frame5 = tk.Frame(self)
        frame7 = tk.Frame(self)
        frame8 = tk.Frame(self)

        self.frame_buttons = self.buttonFrames(frame1)
        self.frame_publish = self.publishFrames(frame2)
        self.frame_pubbulk = PublishBulk(frame5, self.wifi_host)    
        self.frame_udp = ListUDP(frame4, self, self.wifi_host)
        self.frame_graph = GraphHost(frame7, self.master, self.wifi_host)
        self.frame_file = LoadFile(frame8, self.wifi_host)
        

        frame8.pack(side="left", padx=15, fill=tk.BOTH, expand=True)
        frame7.pack(side="right", padx=10, pady=10, expand=True)
        frame5.pack(side="right", padx=10, pady=10, expand=True)
        frame1.pack(side="top", fill=tk.BOTH, expand=True, pady=5)
        frame4.pack(side="right", padx=10, pady=10, expand=True)
        frame2.pack(side="bottom", padx=10, pady=10, expand=True)

            
    def updateConnected(self): #Updates the number of connected ESPs and the lists
        self.after(100, self.updateConnected)
        
        self.wifi_host.updateConnected()
        # print("DJFSKLDEF ", self.wifi_host.getNumberConnected())
        # tmp = self.numberConnected.get()
        self.numberConnected.set(self.wifi_host.getNumberConnected())

        while self.tmpEspIds != self.wifi_host.getEspIds():
            espOrder = self.wifi_host.getEspOrder()
            self.tmpEspIds = copy.deepcopy(self.wifi_host.getEspIds())
            self.moriNumber = []
            print("")
            print(colored("Online ESPs: ", "blue") + colored(espOrder, "blue"))
            print("")
            for i in range(len(espOrder)):
                try:
                    self.moriNumber.append(self.tmpEspIds[espOrder[i]])
                except KeyError:
                    print(colored("Err: ESP not found", 'red'))
                    print("i", i)
                    print("espOrder", espOrder)
                    print("tmpEspIds", tmpEspIds)
                    continue

            self.frame_publ.update_menu(self.moriNumber)
            # self.frame_binary.update_menu(self.moriNumber)
        self.frame_udp.update_udp()

        self.frame_file.checkAutoUpdate()




    def buttonFrames(self, frame):
        frame_1 = tk.Frame(frame)
        frame_2 = tk.Frame(frame)

        frame_st = tk.Frame(frame_1)
        frame_pt = tk.Frame(frame_1)
        frame_dl = tk.Frame(frame_1)
        frame_pl = tk.Frame(frame_2)
        frame_in = tk.Frame(frame_2)

        self.frame_datalog = DataLog(frame_dl, self.wifi_host)
        self.frame_party = PartyFrame(frame_pt, self.wifi_host)
        self.frame_input = InputFlag(frame_in, self.wifi_host)
        self.frame_start = StartStop(frame_st, self.wifi_host, self.frame_input)
        self.frame_pinglog = PingLog(frame_pl, self.wifi_host)

        frame_st.pack(side="left", fill=tk.BOTH, expand=True)
        frame_pt.pack(side="left", fill=tk.BOTH, expand=True)
        frame_dl.pack(side="right", fill=tk.BOTH, expand=True)

        frame_pl.pack(side="left", fill=tk.BOTH, expand=True)
        frame_in.pack(side="right", fill=tk.BOTH, expand=True)

        frame_1.pack(side="top", fill=tk.BOTH, expand=True)
        frame_2.pack(side="top", fill=tk.BOTH, expand=True, pady=5)


    def publishFrames(self, frame):
        frame_pg = tk.Frame(frame)       
        frame_pl = tk.Frame(frame)           

        self.frame_pubg = PublishGlobal(frame_pg, self.wifi_host)
        self.frame_publ = PublishLocal(frame_pl, self, self.wifi_host)    

        frame_pl.pack(side="top", fill=tk.BOTH, expand=True)
        frame_pg.pack(side="top", fill=tk.BOTH, expand=True, pady=10)
