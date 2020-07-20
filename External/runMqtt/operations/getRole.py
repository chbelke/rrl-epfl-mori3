from termcolor import colored

def getRole(self, pyld, espNum):

    mac = pyld[1].replace(":", "")
    mac = mac.lower()
    mac = mac[:1] + 'e' + mac[2:] #Hack

    if self.macDict.get(mac) is None:
        print(colored("ESP: " + pyld[1] + " not yet initialized!", "red"))
    else:
        self.macDict.get(mac)[0] = pyld[2]
        print(self.macDict) 
    return