from termcolor import colored
import traceback

from Settings import names

def wifiEdge(self, pyld, espNum):
    print(colored(names.idsToName[espNum] + ": Request for WiFi Edge", 'red'))
    return