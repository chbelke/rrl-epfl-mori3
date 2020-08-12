from termcolor import colored
import runMqtt.operations.requests.wifiEdge as wifiEdge
import traceback


requests = {
   'WE': wifiEdge.wifiEdge,
}   


def getRequest(self, pyld, espNum):
   print(pyld)
   print(espNum)
   try:
      cmd = pyld[1]
      print(requests[cmd])
      requests[cmd](self, pyld, espNum)
   except:
      traceback.print_exc()    