from termcolor import colored
import traceback

def readNeighbour(self, pyld, espNum):
    print(colored(espNum + ": Neighbour edge "+ pyld[2] + ':', 'green'), end=' ')
    for i in range(2,8):
        print(colored(pyld[i], 'green'), end='')
    print()
    return