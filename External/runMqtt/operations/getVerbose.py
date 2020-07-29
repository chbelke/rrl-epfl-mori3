from termcolor import colored
import codecs

def getVerbose(self, pyld, espNum):
    try:
        msgld = codecs.decode(pyld[1], 'utf-8')
        print(colored(espNum + "-" + msgld, 'yellow'))
    except:
        msgld = codecs.decode(pyld[1], 'latin-1')
        print(colored(espNum + "-", 'yellow'), end="")
        for i in range(len(pyld[1])-1):
            if i % 2 is 1:
                continue
            print(pyld[1][i]*256 + pyld[1][i+1], end=" ")
        print();
    # print(colored(espNum + "-" + msgld, 'yellow'))
    return