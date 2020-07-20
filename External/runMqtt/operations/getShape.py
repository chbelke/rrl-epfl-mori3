
def getShape(self, pyld, espNum):

	mac = pyld[1].replace(":", "")
	mac = mac.lower()
	mac = mac[:1] + 'e' + mac[2:] #Hack
	if not (self.macDict.get(mac) is None):  #Check if the ESP has been referenced
		if self.moriShapeDict.get(espNum) is None: #Check if the ESP shape has already been asked once to initialize the shape array
			self.moriShapeDict[espNum] = [200, 200, 200, 0, 0, 0]

		for i in range(2,len(pyld)): #Store the desired shape values
			self.moriShapeDict.get(espNum)[i-2] = int(pyld[i])
	print(self.moriShapeDict)
	return


