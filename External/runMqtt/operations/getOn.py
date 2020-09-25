import time

from termcolor import colored

def getOn(self, pyld, espNum):

	if espNum not in self.idDict:
		return
	mac = self.idDict[espNum]
	self.coTimeDict[mac] = time.time()
	self.setCoTimeDict(self.coTimeDict)
	if not (self.macDict.get(mac) is None) and not (mac in self.macOrder): #Check if the ESP has been referenced and offline
		print(colored("New ESP connected", "green"))
		self.macOrder.append(mac)
		self.setMacOrder(self.macOrder)
	return