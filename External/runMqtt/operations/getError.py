from termcolor import colored

def getError(self, pyld, espNum):
    msgld = ' '.join(pyld)
    print(colored(espNum + "-" + msgld, 'red'))
    return