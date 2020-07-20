
def getIP(self, pyld, espNum):
    if self.IPDict.get(espNum) is None:
        self.IPDict[espNum] = pyld[1]
        self.EPDict[pyld[1]] = espNum
        self.setIPDict(self.IPDict, self.EPDict)
    return  