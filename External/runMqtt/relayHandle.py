from termcolor import colored
import traceback

def relayHandle(pyld, espNum):
    print(pyld)
    # msgld = ' '.join(pyld)
    # print(colored(espNum + "-" + msgld, 'red'))

    try:
        msg = ' '.join(pyld[1:])
        espNum = "0"+str(msg[0:7])
        msg = msg[7:].rsplit(' ')
        return msg, espNum
    except:
        try:    #special accomodation for pings
            espNum = '0' + pyld[1][0:7].decode('UTF-8')   #espNum
            msg = []
            msg.append(pyld[1][7:11].decode('UTF-8'))
            msg.append(pyld[1][12:])
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
