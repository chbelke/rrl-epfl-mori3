from termcolor import colored
import traceback

from Settings import names

def relayHandle(pyld, espNum):
    # print(pyld)
    # msgld = ' '.join(pyld)
    # print(colored(espNum + "-" + msgld, 'red'))

    try:
        msg = ' '.join(pyld[1:])
        # espNum = "0"+str(msg[0:7])
        espNum = names.convertFromLetter(msg[0])
        msg = msg[1:].rsplit(' ')
        return msg, espNum
    except:
        try:    #special accomodation for pings
            espNum = names.convertFromLetter(chr(pyld[1][0]))
            # espNum = '0' + pyld[1][0:7].decode('UTF-8')   #espNum
            msg = []
            msg.append(pyld[1][1:5].decode('UTF-8'))
            msg.append(pyld[1][6:])
            return msg, espNum
        except:
            print(colored("IN TRACEBACK", 'red'))
            traceback.print_exc()


def splitMessageRelay(pyld):
        # print(pyld)
        msg = ' '.join(pyld[1:])
        # print(msg)
        espNum = "00"+str(msg[0:5])
        # print(espNum)
        # print(msg[5:])
        # print("\n")
        return espNum, msg[5:]
