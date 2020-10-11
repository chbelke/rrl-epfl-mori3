from termcolor import colored

from Settings import names

def startUDP(self, pyld, espNum):

    if pyld[1] == "Start":
        if espNum not in self.UDPDict:
            self.UDPDict.append(espNum)
            self.setUDPDict(self.UDPDict)
            self.espUDP(espNum)
        self.publishLocal("hello", espNum)

    elif pyld[1] == "Stop":
        if espNum in self.UDPDict:
            print(colored("UDP disabled for: " + names.idsToName[espNum], 'red'))
            self.macDict.get(self.idDict[espNum])[0] = "WiFi"
            self.UDPDict.remove(espNum)


    return