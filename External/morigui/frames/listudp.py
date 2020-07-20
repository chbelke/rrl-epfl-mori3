import tkinter as tk
import base64
from termcolor import colored




class ListUDP():

    def __init__(self, frame, parent, mqtthost):
        self.frame = frame
        # self.getUDPDict = parent.getUDPDict
        self.mqtthost = mqtthost
        self.load()

    def load(self):
        self.label = tk.Label(self.frame, text = "UDP")
        self.label.pack()

        self.listbox = tk.Listbox(self.frame, width = 15)
        self.listbox.pack()
     
        self.listbox.insert(0, "Nachos") 
        self.listbox.insert(1, "Nachos") 
        self.listbox.insert(2, "Sandwich") 
        self.listbox.insert(3, "Burger") 
        self.listbox.insert(4, "Pizza") 
        self.listbox.insert(5, "Burrito")

        self.listbox.pack()

        self.update_udp()

    def update_udp(self):
        udpList = self.mqtthost.getUDPDict()

        self.listbox.delete(0, tk.END)

        self.listbox.insert(tk.END, *udpList)

        self.listbox.pack()