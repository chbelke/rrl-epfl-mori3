import time

from termcolor import colored

def getOn(self, pyld, espNum):

	mac = pyld[1].replace(":", "")
	mac = mac.lower()
	mac = mac[:1] + 'e' + mac[2:] #Hack
	self.coTimeDict[mac] = time.time()
	self.setCoTimeDict(self.coTimeDict)
	if not (self.macDict.get(mac) is None) and not (mac in self.macOrder): #Check if the ESP has been referenced and offline
		print(colored("New ESP connected", "green"))
		self.macOrder.append(mac)
		self.setMacOrder(self.macOrder)
	return