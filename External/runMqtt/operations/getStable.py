from termcolor import colored

from Settings import names

def getStable(self, pyld, espNum):
    self.stableState[espNum] = (pyld[1]=='1')
    self.setStableState(self.stableState)
    return