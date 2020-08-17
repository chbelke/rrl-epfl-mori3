from termcolor import colored
import traceback

def readAngles(self, pyld, espNum):
    print(colored(espNum + ": Angles", 'green'), end=" ")
    angles = [None]*3
    for i in range(0,3):
        angles[i] = int(pyld[i+2])
        print(colored(angles[i], 'green'), end=" ")
    print()
    return