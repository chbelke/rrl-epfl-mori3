import serial, time
import sys

from Settings import names

def getVersion(self, pyld, espNum):

	mac =  pyld[1].replace(":", "")
	mac = mac.lower()
	mac = mac[:1] + 'e' + mac[2:] #Hack
	if self.verCallTimeDict.get(mac) is None:
		self.verCallTimeDict[mac] = time.time()
		self.verCalledDict[mac] = False

	if self.verCalledDict.get(mac) == False: #Check if the version has already been asked a short while ago (if it has: wait a bit)
		self.verCallTimeDict[mac] = time.time()
		
		espVer = float(pyld[1])
		if(espVer == self.version):
			print("ESP/{} Software up to date".format(espNum))
			self.publishLocal("gver",espNum)   #version good
			print("after version")
		
		elif(espVer < self.version):
			print("ESP/{} Software out date".format(espNum))
			print("ESP/{}: {}".format(espNum,espVer))
			print("Curr: {}".format(self.version))
			self.publishLocal("bver",espNum)   #version bad

		elif(espVer > self.version):
			print("Kevin is an idiot and forgot to update this to match newest version")
			print("ESP/{}: {}".format(espNum,espVer))
			print("Curr: {}".format(self.version))        
			self.mqttClient.exit()
			sys.exit()
	
		self.verCalledDict[mac] = True

	elif time.time() - self.verCallTimeDict.get(mac) > 1:
		self.verCalledDict[mac] = False	   
	return