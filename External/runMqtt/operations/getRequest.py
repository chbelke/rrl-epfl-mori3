from termcolor import colored
import traceback

import runMqtt.operations.requests.wifiEdge as wifiEdge
import runMqtt.operations.requests.readEdgeLength as readEdgeLength
import runMqtt.operations.requests.readAngles as readAngles
import runMqtt.operations.requests.readOrientation as readOrientation
import runMqtt.operations.requests.readNeighbour as readNeighbour
import runMqtt.operations.requests.disconnectedNeighbour as disconnectedNeighbour
import runMqtt.operations.requests.readWifiEdge as readWifiEdge


requests = {
   'WE': wifiEdge.wifiEdge,
   'EL': readEdgeLength.readEdgeLength,
   'AN': readAngles.readAngles,
   'OR': readOrientation.readOrientation,
   'NB': readNeighbour.readNeighbour,
   'ED': readWifiEdge.readWifiEdge,
   'NO': disconnectedNeighbour.disconnectedNeighbour
}   


def getRequest(self, pyld, espNum):
   # print(pyld)
   # print(espNum)
   try:
      cmd = pyld[1]
      requests[cmd](self, pyld, espNum)
   except:
      traceback.print_exc()    