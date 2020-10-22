from termcolor import colored
import traceback
import codecs

def readNeighbour(self, pyld, espNum):
    tmp = bytearray.fromhex(pyld[2]).decode()
    neighbour = '00'+ tmp[1:]
    edge = int(pyld[2][1], 16)
    self.updateConnection(espNum, edge, neighbour)
    # print(colored(str(neighbour) + " connected", 'green'))
    return