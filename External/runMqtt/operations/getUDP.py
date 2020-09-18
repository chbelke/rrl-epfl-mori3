from termcolor import colored

def startUDP(self, pyld, espNum):

    if pyld[1] == "Start":
        if espNum not in self.UDPDict:
            print(colored("UDP enabled for: " + espNum, 'green'))
            self.UDPDict.append(espNum)
            self.setUDPDict(self.UDPDict)
            self.macDict.get(self.idDict[espNum])[0] = "UDP"
        self.publishLocal("hello", espNum)

    elif pyld[1] == "Stop":
        if espNum in self.UDPDict:
            print(colored("UDP disabled for: " + espNum, 'red'))
            self.macDict.get(self.idDict[espNum])[0] = "WiFi"
            self.UDPDict.remove(espNum)


    return