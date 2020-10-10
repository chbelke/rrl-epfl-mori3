from termcolor import colored

from Settings import names

def getWifi(self, pyld, espNum):

    noWifiList = self.getNoWifiDict()
    macDict = self.getEspIds()
    if pyld[1] == "Off":
        if espNum not in noWifiList:
            print(colored("WiFi disabled for " + names.idsToName[espNum], 'green'))
            noWifiList[espNum] = []
            self.setNoWifiDict(noWifiList)
            self.updateNoWifiDict(espNum)
            macDict.get(self.idDict[espNum])[0] = "NoWifi"
            self.setMacDict(macDict)

    elif pyld[1] == "On":
        if espNum in noWifiList:
            print(colored("WiFi enabled for " + names.idsToName[espNum], 'green'))
            macDict.get(self.idDict[espNum])[0] = "WiFi"
            del noWifiList[espNum]
            self.setNoWifiDict(noWifiList)
            self.setMacDict(macDict)


    return