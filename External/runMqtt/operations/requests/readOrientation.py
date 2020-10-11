from termcolor import colored
import traceback

from Settings import names

def readOrientation(self, pyld, espNum):
    print(colored(names.idsToName[espNum] + ": Orientation", 'green'), end=" ")
    orient = [None]*3
    for i in range(0,3):
        orient[i] = int(pyld[i+2])
        print(colored(orient[i], 'green'), end=" ")
    print()
    return