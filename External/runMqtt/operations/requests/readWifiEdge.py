from termcolor import colored
import traceback

from Settings import names


def readWifiEdge(self, pyld, espNum):
    print(colored(names.idsToName[espNum] + ": WifiEdge " + pyld[2], 'green'))
    return