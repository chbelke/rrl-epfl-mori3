from termcolor import colored

from Settings import names

def getMac(self, pyld, espNum):

    print(colored("MAC recieved: " + pyld[1] + "for" + names.idsToName[espNum], 'green'))
    mac = pyld[1].replace(":", "")
    mac = mac.lower()
    mac = mac[:1] + 'e' + mac[2:] #Hack

    if self.macDict.get(mac) is None:
        self.macDict[mac] = [None] * 2 #First index is for the role of the ESP and the second is for its ID
        self.macDict.get(mac)[1] = espNum
        self.macDict.get(mac)[0] = "WiFi"
        self.idDict[espNum] = mac
        self.macOrder.append(mac)
        
        self.setMacDict(self.macDict)
        self.setIdDict(self.idDict)
        self.setMacOrder(self.macOrder)
        self.addConnection(espNum)

    elif self.macDict.get(mac)[0] == "Lost":
        self.macDict.get(mac)[0] = "WiFi"
        self.setMacDict(self.macDict)
        

    if self.moriShapeDict.get(espNum) is None: #Check if the ESP shape has already been asked once to initialize the shape array
        self.moriShapeDict[espNum] = [200, 200, 200, 0, 0, 0]
        self.setMoriShapeDict(self.moriShapeDict)

    return