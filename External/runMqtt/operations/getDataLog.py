import traceback
from termcolor import colored
import codecs

def getDataLog(self, pyld, espNum):
    # print(colored(espNum + "-", 'yellow'), end="")
    # for py in pyld:
    #     print(colored(py, 'yellow'), end=" ")    
    # print()
    pld_str = pyld[1]
    num_hex = int(len(pyld[1]))
    values = [None]*int((num_hex)/2)
    for i in range(int(num_hex/2)):
        values[i] = int(pld_str[i*2:i*2+2], 16)

    time = 0
    angles = [0]*3
    edges = [0]*3
    orient = [0]*3
    i = 5;
    try:
        time = (values[1] << 24) | (values[2] << 16) | (values[3] << 8) | values[4];
        if values[0] & 0b00000001:    #angles
            for j in range(0,3):
                angles[j] = values[i]*256 + values[i+1]
                i += 2
        if values[0] & 0b00000010:    #edges
            for j in range(0,3):
                edges[j] = values[i]
                i += 1
        if values[0] & 0b00000100:    #orient
            for j in range(0,3):
                orient[j] = values[i]*256 + values[i+1]
                if orient[j] >= 32768:
                    orient[j] -= 65536 

                # orient[j] = values[i]*256 + values[i+1]
                i += 2                
        cscString = [espNum, str(time), str(angles).strip('[]'), str(edges).strip('[]'), str(orient).strip('[]') + '\n']
        self.writeDataLogFile(','.join(cscString))
    except:
        print(colored("IN TRACEBACK", 'red'))
        traceback.print_exc()
    
    return