from termcolor import colored
import traceback

def readNeighbour(self, pyld, espNum):
    print(colored(espNum + ": Neighbour edge "+ pyld[2] + ':', 'green'), end=' ')
    tmp = [None] * 6
    for i in range(0,6):
        tmp[i] = bytearray.fromhex(pyld[i+3]).decode()
    neighbour = ''.join(tmp)
    print(colored(str(neighbour) + " connected", 'green'))
    return