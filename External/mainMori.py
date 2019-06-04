'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Most recent handler of

Scans for modules with SSID "helloThere", and then relays the measured signal 
strength and corresponding MAC address over the network.

Used in conjunction with wifiHandler.py, distanceRanger.py, getData.py, and
mqttAnalyzer.py

Before flashing to ESP8266, please change clientName, publishName, and recieveName
to their appropriate values.

**********************************************************************************
'''


import serial, time
import datetime
import sys, select, os
# import matplotlib.pyplot as plt
from termcolor import colored
import tkinter as tk
from threading import Thread

from morigui.guiWindow import MoriGui


def main():
    startTime = time.time()
    gui = mainGui()

    # try:
    #   while(True):

    # except KeyboardInterrupt:
    #   print("Exiting...")
    #   gui.mqtthost.exit()
    #   root.destroy()
    #   print("Exit Success!")


class mainGui(Thread):

    def __init__(self):
        Thread.__init__(self)
        self.start()

    def callback(self):
        self.gui.mqtthost.exit()
        self.root.quit()

    def run(self):
        self.root = tk.Tk()
        self.root.protocol("WM_DELETE_WINDOW", self.callback)
        self.root.title("ESP Communication")
        self.gui = MoriGui(master=self.root)

        # label = tk.Label(self.root, text="Hello World")
        # label.pack()
        self.gui.updateConnected()

        self.gui.mainloop()
        

if __name__ == '__main__':
    sys.exit(main())

