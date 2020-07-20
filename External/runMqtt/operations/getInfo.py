from termcolor import colored

def getInfo(self, pyld, espNum):
    msgld = ' '.join(pyld)
    print(colored(espNum + "-" + msgld, 'yellow'))
    return