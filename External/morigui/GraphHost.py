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
        # self.wifi_host = guiHost.wifi_host
        self.wifi_host = guiHost

        self.connMatrix = []
        self.nodes = []
        self.edges = []
        self.mori_status = {}


        self.G=nx.DiGraph()
        self.color_map = []
        self.node_size = 70
        # self.font_size = 18
        self.font_size = 10
        self.colourDict = {'WiFi': 'xkcd:blue', 'UDP': 'xkcd:green', 'Lost': 'xkcd:red', 'Hub': 'xkcd:gold', 'NoWifi': 'xkcd:grey'}        
           
        self.createWidgets()
        # animate = matplotlib.animation.FuncAnimation(self.fig, self.plotFigure, interval=1000)


              
    def createWidgets(self):
        # self.fig = plt.figure(figsize=(5,5), dpi=100)
        self.fig = plt.figure(figsize=(3,3), dpi=100)
        # self.fig.tight_layout(pad=2)
        self.ax = self.fig.add_subplot(111)
        

        self.canvas = FigureCanvasTkAgg(self.fig, self)
        self.canvas.draw()
        self.canvas.get_tk_widget().pack(side=tk.BOTTOM, fill=tk.BOTH, expand=True)

        # toolbar = NavigationToolbar2Tk(self.canvas, self)
        # toolbar.update()
        # self.canvas._tkcanvas.pack(side=tk.TOP, fill=tk.BOTH, expand=True)

        self.pack(side="top", fill=tk.BOTH, expand=True, pady=5)        

            
    def updateConnected(self): #Updates the number of connected ESPs and the lists
        self.after(500, self.updateConnected)
       
        if self.wifi_host.getNumberConnected() <= 0:
            return

        tmp = copy.deepcopy(self.connMatrix)
        if tmp != self.wifi_host.getConnMatrix():
            self.newConnectionMatrix()

        if self.mori_status != self.wifi_host.getEspIds():
            self.mori_status = copy.deepcopy(self.wifi_host.getEspIds())
        
        self.updateColors()
        
        # if update_flag:
        self.plotFigure()



    def newConnectionMatrix(self):
        self.connMatrix = copy.deepcopy(self.wifi_host.getConnMatrix())
        self.getNodes()
        self.getEdges()

    def getNodes(self):
        self.nodes = []
        for connId in self.connMatrix:
            self.nodes.append(connId)

    def getEdges(self):
        self.edges = []
        for connId in self.connMatrix:
            for i in range(0,3):
                if self.connMatrix[connId][i] != 0:
                    self.edges.append((connId,self.connMatrix[connId][i]))


    def plotFigure(self):
        self.ax=plt.gca()
        self.ax.clear()
        self.ax.set_xlim(-1,1)
        self.ax.set_ylim(-1,1)        
        # self.ax.plot(random.sample(range(1, 10), 8), random.sample(range(1, 10), 8))
        self.plotGraph()
        

        # nx.draw(self.G, self.pos, node_color=self.color_map, node_size=self.node_size,
        #     font_size=self.font_size)

        # pos_comp = {}
        # comp = 0.8  # offset on the y axis
        # for k, v in self.pos.items():
        #     pos_comp[k] = (v[0]*comp, v[1]*comp)

        x_values, y_values = zip(*self.pos.values())
        x_max = max(x_values)
        x_min = min(x_values)
        if x_max > x_min:
            x_margin = (x_max - x_min) * 0.25
            plt.xlim(min(-1,x_min - x_margin), max(1,x_max + x_margin))

        y_max = max(y_values)
        y_min = min(y_values)
        if y_max > y_min:
            y_margin = (y_max - y_min) * 0.10
            plt.ylim(min(-1,y_min - y_margin), max(1,y_max + y_margin))        

        pos_higher = {}
        y_off = 0.01  # offset on the y axis
        for k, v in self.pos.items():
            pos_higher[k] = (v[0], v[1]+y_off)

        if len(self.color_map) < len(self.pos):
            return

        nx.draw_networkx_nodes(self.G, self.pos, node_color=self.color_map, node_size=self.node_size, linewidths=None)
        nx.draw_networkx_edges(self.G, self.pos, width=1, arrowsize=5)
        nx.draw_networkx_labels(self.G, pos_higher, font_size=self.font_size, verticalalignment='bottom')
        plt.box(on=False)

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
        espIds = self.wifi_host.getIdDict()
        for node in self.G:
            state = self.mori_status[espIds[node]][0]
            if state == "Lost":
                self.color_map.append(self.colourDict[state])
                continue
            elif node in self.wifi_host.getHubDict():
                self.color_map.append(self.colourDict["Hub"])
                continue
            elif node in self.wifi_host.getNoWifiDict():
                self.color_map.append(self.colourDict["NoWifi"])
                continue
            self.color_map.append(self.colourDict[state])

        return