import tkinter as tk
from threading import Thread
import threading
import copy
from termcolor import colored
import random

import matplotlib
import matplotlib.pyplot as plt
matplotlib.use("TkAgg")
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg, NavigationToolbar2Tk
from matplotlib.figure import Figure
import networkx as nx

# from runMqtt.wifi_host import wifi_host
from runMqtt.wirelessHost import WirelessHost

from morigui.frames.party import PartyFrame



class GraphHost(tk.Toplevel):

    def __init__(self, master=None, *args, **kwargs):
        Thread(tk.Toplevel.__init__(self, master))

        self.title("yoooooooooooooooooo")
        # self.geometry("200x200")
        # label = tk.Label(self, text="FUKKIN LABEL YOOOO")
        # label.pack()


class GraphFrame(tk.Frame):

    def __init__(self, master=None, guiHost=None, *args, **kwargs):
        Thread(tk.Frame.__init__(self, master))
        self.master = master
        self.wifi_host = guiHost.wifi_host

        self.connMatrix = []
        self.nodes = []
        self.edges = []
        self.mori_status = {}
        self.colourDict = {'WiFi': 'blue', 'UDP': 'green', 'Lost': 'red'}        
        self.G=nx.DiGraph()
           
        self.createWidgets()
        # animate = matplotlib.animation.FuncAnimation(self.fig, self.plotFigure, interval=1000)


              
    def createWidgets(self):
        self.fig = plt.figure(figsize=(5,5), dpi=100)
        self.ax = self.fig.add_subplot(111)
        

        self.canvas = FigureCanvasTkAgg(self.fig, self)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        toolbar = NavigationToolbar2Tk(self.canvas, self)
        toolbar.update()
        self.canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.pack(side="top", fill=tk.BOTH, expand=True, pady=5)        

            
    def updateConnected(self): #Updates the number of connected ESPs and the lists
        self.after(500, self.updateConnected)
        
        update_flag=False
        tmp = copy.deepcopy(self.connMatrix)
        
        if tmp != self.wifi_host.getConnMatrix():
            self.newConnectionMatrix()
            update_flag=True

        if self.mori_status != self.wifi_host.getEspIds():
            self.tmpEspIds = copy.deepcopy(self.wifi_host.getEspIds())
            self.updateColors()
            update_flag=True
        
        if update_flag:
            self.plotFigure()



    def newConnectionMatrix(self):
        self.connMatrix = copy.deepcopy(self.wifi_host.getConnMatrix())
        self.getNodes()
        self.getEdges()

    def getNodes(self):
        self.nodes = []
        for connId in self.connMatrix:
            self.nodes.append(connId[0])

    def getEdges(self):
        self.edges = []
        for connId in self.connMatrix:
            for i in range(1,4):
                if connId[i] != 0:
                    self.edges.append((connId[0],connId[i]))


    def plotFigure(self):
        self.ax=plt.gca()
        self.ax.clear()
        # self.ax.plot(random.sample(range(1, 10), 8), random.sample(range(1, 10), 8))
        self.plotGraph()

        nx.draw(self.G, self.pos, node_color=self.color_map, with_labels=True)
        self.canvas.draw()

    def plotGraph(self):
        self.G=nx.DiGraph()
        self.G.add_nodes_from(self.nodes)
        self.G.add_edges_from(self.edges)
        tmp_pos = nx.spring_layout(self.G)
        try:
            for label in tmp_pos:
                if label in self.pos:
                    tmp_pos[label] = self.pos[label]
            self.pos = tmp_pos
        except AttributeError:
            self.pos = tmp_pos
        return


    def updateColors(self):
        self.color_map = []
        for node in self.G:
            espIds = self.wifi_host.getIdDict()
            self.color_map.append(self.colourDict[self.tmpEspIds[espIds[node]][0]])
        return