from termcolor import colored

def getWifi(self, pyld, espNum):

    noWifiList = self.getNoWifiDict()
    macDict = self.getEspIds()
    if pyld[1] == "Off":
        if espNum not in noWifiList:
            print(colored("WiFi disabled for " + espNum, 'green'))
            noWifiList.append(espNum)
            self.setNoWifiDict(noWifiList)
            macDict.get(self.idDict[espNum])[0] = "NoWifi"
            self.setMacDict(macDict)

    elif pyld[1] == "On":
        if espNum in noWifiList:
            print(colored("WiFi enabled for " + espNum, 'green'))
            macDict.get(self.idDict[espNum])[0] = "WiFi"
            noWifiList.remove(noWifiList)
            self.setNoWifiDict(noWifiList)
            self.setMacDict(macDict)


    return