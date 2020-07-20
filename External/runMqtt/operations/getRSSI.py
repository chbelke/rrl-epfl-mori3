

def getRSSI(self, pyld, espNum):

    macaddr = pyld[1][:-1]
    rssi = int(pyld[2])
    if(macaddr in self.macDict):  #check if we know other mac
        localMacPos = self.macOrder.index(macaddr)
        foreignMacPos = self.macOrder.index(self.idDict[espNum])
        self.data[localMacPos][foreignMacPos].append(rssi)
        print('RSSI of ['+ macaddr + '] according to [' + self.idDict[espNum] + '] is: ' + rssi)
    else:
        if time.time() - self.macCallTime > 3.0:
           print(macaddr + " is not found. Calling all for MAC addresses.")
           self.publishGlobal("mac")
           self.macCallTime = time.time()

    return