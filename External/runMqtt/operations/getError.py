from termcolor import colored

from Settings import names

def getError(self, pyld, espNum):
    msgld = ' '.join(pyld)
    print(colored(names.idsToName[espNum] + "-" + msgld, 'red'))
    return