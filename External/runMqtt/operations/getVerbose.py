import traceback
from termcolor import colored
import codecs

from Settings import names

def getVerbose(self, pyld, espNum):
    print(colored(names.idsToName[espNum] + "-", 'yellow'), end="")
    print(colored(pyld[0], 'yellow'), end=" ")
    print(colored(int(pyld[2],16), 'yellow'))
    return
	
	
    for i in pyld:
        print(colored(i, 'yellow'))    
		#print(colored(i, 'yellow'), end=" ")    
    print()
    # print(colored(espNum + "-" + (pyld[2]) + ", " + int(hex(pyld[2])), 'yellow'))    
    # print(pyld[1])

    return
    try:
        if isinstance(pyld[1],str):
            print(colored(espNum + "-" + (pyld[1]), 'yellow'))
        else:
            try:
                msgld = codecs.decode(pyld[1], 'utf-8')
                print(colored(espNum + "-" + msgld, 'yellow'))
            except:
                print(colored(espNum + "-", 'yellow'), end="")
                for i in range(len(pyld[1])-1):
                    if i % 2 == 1:
                        continue
                    print(pyld[1][i]*256 + pyld[1][i+1], end=" ")
                print();
    except:
        print(colored("IN TRACEBACK", 'red'))
        traceback.print_exc()
    
    return