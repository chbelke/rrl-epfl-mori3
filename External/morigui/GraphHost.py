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
from PIL import Image, ImageTk

# from runMqtt.wifi_host import wifi_host
from runMqtt.wirelessHost import WirelessHost

from morigui.frames.party import PartyFrame



class GraphHost():

    def __init__(self, frame, master=None, wifi_host=None, *args, **kwargs):
        self.master = master
        self.wifi_host = wifi_host
        self.frame = frame
        self.image = ImageTk.PhotoImage(Image.open("images/Sign_Smaller.png"))
        self.createWidgets()

    def createWidgets(self):        
        graph_frame = GraphFrame(self.frame, self.wifi_host)
        redraw_frame = RedrawFrame(self.frame, graph_frame)
        legend_frame = LegendFrame(self.frame, graph_frame)
        photo_frame = PhotoFrame(self.frame, self.image)

        graph_frame.pack(side="left", fill=tk.BOTH, expand=True, pady=5)
        redraw_frame.pack(side="top", expand=True, pady=5)
        legend_frame.pack(side="top", expand=True, pady=5)
        photo_frame.pack(side="bottom", expand=True, pady=5)
        


class RedrawFrame(tk.Frame):
    def __init__(self, master, graphFrame):
        tk.Frame.__init__(self, master)
        self.graphFrame = graphFrame
        self.createWidgets()

    def createWidgets(self):
        self.redraw_button = tk.Button(self)
        self.redraw_button["text"] = "Redraw",
        self.redraw_button["command"] = lambda: self.graphFrame.redrawGraph()
        self.redraw_button.pack({"side": "bottom"})         
        self.pack(side="top", fill=tk.BOTH, expand=True, pady=5) 



class LegendFrame(tk.Frame):
    def __init__(self, master, graphFrame):
        tk.Frame.__init__(self, master)
        self.graphFrame = graphFrame
        self.createWidgets()

    def createWidgets(self):
        self.legend = plt.figure(figsize=(1.3,2), dpi=100)
        self.ax_leg = self.legend.add_subplot(111)
        self.legend.patch.set_facecolor('#d9d9d9')   


        self.drawLegend()
        plt.figure(self.legend.number)
        plt.gcf().canvas = FigureCanvasTkAgg(self.legend, self)
        plt.gcf().canvas.draw()
        plt.gcf().canvas.get_tk_widget().pack(side=tk.TOP, fill=tk.BOTH, expand=True)

    def drawLegend(self):
        plt.figure(self.legend.number)
        colors = self.graphFrame.colourDict
        handles = [matplotlib.patches.Patch(color=colors[x], label=x) for x in colors.keys()]        
        plt.legend(handles=handles)
        plt.gca().set_axis_off()


class GraphFrame(tk.Frame):

    def __init__(self, master=None, wifi_host=None, *args, **kwargs):
        Thread(tk.Frame.__init__(self, master))
        self.master = master
        self.wifi_host = wifi_host

        self.connMatrix = []
        self.nodes = []
        self.edges = []
        self.mori_status = {}


        self.G=nx.DiGraph()
        self.color_map = []
        self.node_size = 70
        self.font_size = 10
        self.colourDict = {'WiFi': 'xkcd:blue', 'UDP': 'xkcd:green', 'Lost': 'xkcd:red', 'Hub': 'xkcd:gold', 'NoWifi': 'xkcd:grey'}        
           
        self.createWidgets()
        self.updateConnected()           


              
    def createWidgets(self):
        self.fig = plt.figure(figsize=(3,3), dpi=100)
        self.ax = self.fig.add_subplot(111)
        

        plt.gcf().canvas = FigureCanvasTkAgg(self.fig, self)
        plt.gcf().canvas.draw()
        plt.gcf().canvas.get_tk_widget().pack(side=tk.RIGHT, fill=tk.BOTH, expand=True)

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
        plt.figure(self.fig.number)
        self.ax=plt.gca()
        self.ax.clear()
        self.ax.set_xlim(-1,1)
        self.ax.set_ylim(-1,1)        
        self.plotGraph()

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

        plt.gcf().canvas.draw()

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


    def redrawGraph(self):
        self.G=nx.DiGraph()
        self.G.add_nodes_from(self.nodes)
        self.G.add_edges_from(self.edges)
        self.pos = nx.spring_layout(self.G)
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


class PhotoFrame(tk.Frame):
    def __init__(self, master, image):
        tk.Frame.__init__(self, master)

        canvas = tk.Canvas(self, width=50, height=50)
        canvas.pack(side='bottom')
        canvas.create_image(25, 25, image=image)
        self.pack(side="bottom")#, fill=tk.BOTH, expand=1)