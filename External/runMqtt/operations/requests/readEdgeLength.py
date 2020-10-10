from termcolor import colored
import traceback

from Settings import names

def readEdgeLength(self, pyld, espNum):
    print(colored(names.idsToName[espNum] + ": Edge Lengths", 'green'), end=" ")
    edges = [None]*3
    for i in range(0,3):
        edges[i] = int(pyld[i+2])
        print(colored(edges[i], 'green'), end=" ")
    print()
    return