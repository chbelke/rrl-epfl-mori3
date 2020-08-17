from termcolor import colored
import traceback

def readWifiEdge(self, pyld, espNum):
    print(colored(espNum + ": WifiEdge " + pyld[2], 'green'))
    return