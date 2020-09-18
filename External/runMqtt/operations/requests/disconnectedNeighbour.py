from termcolor import colored
import traceback

def disconnectedNeighbour(self, pyld, espNum):
    # print(espNum, pyld)
    # print(colored(espNum + ": Neighbour edge "+ pyld[2] + ' disconnected', 'yellow'))
    self.updateConnection(espNum, pyld[2], 0)
    return