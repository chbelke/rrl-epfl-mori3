import os

from termcolor import colored
import tkinter as tk
from tkinter import filedialog
import json

from Settings import names


class LoadFile():

    def __init__(self, frame, mqtthost):
        self.frame = frame
        self.mqtthost = mqtthost
        self.fileName = []
        self.fileContents = []
        self.iteration = 0
        self.load()

    def load(self):

        frame_load_display = tk.Frame(self.frame)
        frame_disp_text = tk.Frame(self.frame)
        frame_prev_next = tk.Frame(self.frame)

        load_button = tk.Button(frame_load_display)
        load_button["text"] = "LoadFile",
        load_button["command"] = lambda: self.openFile()
        load_button.pack({"side": "top"}, fill=tk.X, expand=True)

        self.next_stage = tk.Label(frame_load_display, font='Helvetica 10 bold')
        self.next_stage["text"] = "No File Loaded"
        self.next_stage.pack({"side": "top"}, fill=tk.X, expand=True)

        self.show_json = tk.Label(frame_disp_text, anchor='w')
        self.show_json["text"] = ""
        self.show_json.pack({"side": "top"}, fill=tk.X)

        self.next_button = tk.Button(frame_prev_next)
        self.next_button["text"] = "Next",
        self.next_button["command"] = lambda: self.iterateJson()
        self.next_button.pack({"side": "left"}, fill=tk.X, expand=True)

        self.run_button = tk.Button(frame_prev_next)
        self.run_button["text"] = "Run",
        self.run_button["fg"]   = "green"
        self.run_button["command"] = lambda: self.runJson()
        self.run_button.pack({"side": "right"}, fill=tk.X, expand=True)

        self.prev_button = tk.Button(frame_prev_next)
        self.prev_button["text"] = "Prev",
        self.prev_button["command"] = lambda: self.prevJson()
        self.prev_button.pack({"side": "left"}, fill=tk.X, expand=True)

        frame_load_display.pack(side="top", expand=True, pady=5)
        frame_disp_text.pack(side="top", fill=tk.BOTH, expand=True, pady=5)
        frame_prev_next.pack(side="bottom", expand=True, pady=5)


    def openFile(self):
        if not os.path.exists("CommandFiles"):
            os.makedirs("CommandFiles")
        ftypes = [('JSON', '*.json'), ('All files', '*')]
        self.fileName = filedialog.askopenfilename(initialdir ="CommandFiles", filetypes=ftypes)

        try:
            with open(self.fileName, 'r') as f:
                self.fileContents = json.load(f, object_pairs_hook=self.value_resolver)
        except FileNotFoundError:
            print("Cannot open file")
            return
        self.iteration = 0
        self.displayJson()

    def displayJson(self):
        self.next_stage["text"] = self.fileContents[self.iteration]["Label"]
        textstr = ""
        for module in self.fileContents[self.iteration]:
            if module == 'Label':
                continue
            for cmd in self.fileContents[self.iteration][module]:
                textstr = textstr + module[:4] + ": " + cmd + "\n"
        self.show_json["text"] = textstr[:-1] #remove last \n


    def iterateJson(self):
        if self.iteration < len(self.fileContents)-1:
            self.iteration += 1
            self.displayJson()
            self.prev_button["fg"] = "black"
        else:
            self.next_button["fg"] = "gray"


    def prevJson(self):
        if self.iteration > 0:
            self.iteration -= 1
            self.displayJson()
            self.next_button["fg"] = "black"
            if self.iteration <= 0:
                self.prev_button["fg"] = "gray"
        else:
            self.prev_button["fg"] = "gray"    


    def runJson(self):
        for module in self.fileContents[self.iteration]:
            if module == 'Label':
                continue
            for cmd in self.fileContents[self.iteration][module]:
                self.mqtthost.publishLocal(cmd, self.checkName(module))  
        self.iterateJson()


    def value_resolver(self, pairs):
        print(pairs)
        unique_modules = set([i[0] for i in pairs])
        new_dict = {}
        print(unique_modules)
        for module in unique_modules:
            values = [v for k, v in pairs if k == module]
            new_dict[module] = values
        return dict(new_dict)        


    def checkName(self, name):
        try:
            return names.nameToIds[name]
        except KeyError:
            return name    

