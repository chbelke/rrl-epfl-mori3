import traceback
from termcolor import colored
import codecs

def getDataLog(self, pyld, espNum):
    print(colored(espNum + "-", 'yellow'), end="")
    for i in pyld:
        print(colored(i, 'yellow'), end=" ")    
    print()

    i = 2;

    values = [0]*len(pyld)
    for tmp in pyld:
        values = int(tmp)
    time = 0
    angles = [None]*3
    edges = [None]*3
    orient = [None]*3
    try:
        time = (values[4] << 24) | (values[3] << 16) | (values[2] << 8) | values[1];
        if pyld[0] & 0b00000001:    #angles
            for j in range(0,2):
                angles[j] = values[i+1]*256 + values[i]
                i += 2
        if pyld[0] & 0b00000010:    #edges
            for j in range(0,2):
                edges[j] = values[i]
                i += 1
        if pyld[0] & 0b00000100:    #orient
            for j in range(0,2):
                orient[j] = values[i+1]*256 + values[i]
                i += 2                
        cscString = [espNum, time, angles.join(','), edges.join(','), orient.join(',')]
        self.writeDataLogFile(cscString.join(','))
    except:
        print(colored("IN TRACEBACK", 'red'))
        traceback.print_exc()
    
    return