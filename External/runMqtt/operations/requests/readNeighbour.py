from termcolor import colored
import traceback

def readNeighbour(self, pyld, espNum):
    print(colored(espNum + ": Neighbour edge "+ pyld[2] + ':', 'green'), end=' ')
    for i in range(3,9):
        print(colored(bytearray.fromhex(pyld[i]).decode(), 'green'), end='')
    print()
    return