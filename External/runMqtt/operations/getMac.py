from termcolor import colored

def getMac(self, pyld, espNum):

    print(colored("MAC address recieved: " + pyld[1], 'green'))
    mac = pyld[1].replace(":", "")
    mac = mac.lower()
    mac = mac[:1] + 'e' + mac[2:] #Hack

    if self.macDict.get(mac) is None:
        self.macDict[mac] = [None] * 2 #First index is for the role of the ESP and the second is for its ID
        self.macDict.get(mac)[1] = espNum
        self.idDict[espNum] = mac
        self.macOrder.append(mac)
        
        self.setMacDict(self.macDict)
        self.setIdDict(self.idDict)
        self.setMacOrder(self.macOrder)

    if self.moriShapeDict.get(espNum) is None: #Check if the ESP shape has already been asked once to initialize the shape array
        self.moriShapeDict[espNum] = [200, 200, 200, 0, 0, 0]
        self.setMoriShapeDict(self.moriShapeDict)

    return