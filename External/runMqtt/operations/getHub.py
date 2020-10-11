from termcolor import colored

from Settings import names

def getHub(self, pyld, espNum):
    hublist = self.getHubDict()
    if pyld[1] == "On":
        if espNum not in self.HubDict:
            print(colored(names.idsToName[espNum] + " set as hub", 'green'))
            hublist.append(espNum)
            self.setHubDict(hublist)
            # macDict.get(self.idDict[espNum])[0] = "Hub"
            # self.setMacDict(macDict)
        self.publishLocal("hello", espNum)

    elif pyld[1] == "Off":
        if espNum in self.HubDict:
            print(colored(names.idsToName[espNum] + " no longer a hub", 'green'))
            # macDict.get(self.idDict[espNum])[0] = "WiFi"
            self.HubDict.remove(espNum)
            self.setHubDict(self.HubDict)
            # self.setMacDict(macDict)


    return