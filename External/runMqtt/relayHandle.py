from termcolor import colored
import traceback

def relayHandle(pyld, espNum):
    msgld = ' '.join(pyld)
    print(colored(espNum + "-" + msgld, 'red'))

    try:
        msg = ' '.join(pyld[1:])
        espNum = "00"+str(msg[0:5])
        msg = msg[5:].rsplit(' ')
        return msg, espNum
    except:
        print(colored("IN TRACEBACK", 'red'))
        traceback.print_exc()


def splitMessageRelay(pyld):
        print(pyld)
        msg = ' '.join(pyld[1:])
        print(msg)
        espNum = "00"+str(msg[0:5])
        print(espNum)
        print(msg[5:])
        print("\n")
        return espNum, msg[5:]
