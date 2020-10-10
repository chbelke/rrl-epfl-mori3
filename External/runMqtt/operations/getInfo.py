from termcolor import colored

from Settings import names

def getInfo(self, pyld, espNum):
    msgld = ' '.join(pyld)
    print(colored(names.idsToName[espNum] + "-" + msgld, 'yellow'))
    return