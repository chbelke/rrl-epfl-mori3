from termcolor import colored

from Settings import names

def getInfo(self, pyld, espNum):
    # if any(isinstance(ld,bytes) for ld in pyld):
    #     print(colored("Error: Info published as bytes",'red'))
    #     return
    msgld = ' '.join(pyld)
    print(colored(names.idsToName[espNum] + "-" + msgld, 'yellow'))
    return