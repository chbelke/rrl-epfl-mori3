from termcolor import colored

def startUDP(self, pyld, espNum):

    if pyld[1] == "Start":
        if espNum not in self.UDPDict:
            print(colored("UDP enabled for: " + espNum, 'green'))
            self.UDPDict.append(espNum)
            self.setUDPDict(self.UDPDict)

    elif pyld[1] == "Stop":
        if espNum in self.UDPDict:
            print(colored("UDP disabled for: " + espNum, 'red'))

            self.UDPDict.remove(espNum)


    return