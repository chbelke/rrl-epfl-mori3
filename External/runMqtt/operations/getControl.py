
def getControl(self, pyld, espNum):

    if not(self.controllerDict.get(espNum) is None):#Enter if the leader has followers
        for i in range(len(self.controllerDict.get(espNum))):
            if pyld[1] == self.controllerDict.get(espNum)[i]: #Enter if the leader is already leading the follower (error)
                print(colored("ERROR: ESP" + espNum + " is already the leader of ESP" + pyld[1], "red"))
                return

    #Conditions verified => add the follower to the leader's list
    if self.controllerDict.get(espNum) is None: #First follower for the leader
        self.controllerDict[espNum] = [pyld[1]]
        self.controllerOrder.append(espNum)
    else:
        self.controllerDict[espNum].append(pyld[1])
        self.controllerOrder.append(espNum)
    #Establish a UDP communication between the controller and the MORI
    message = "udp/r/p" + str(self.udpPort)
    self.publishLocal(pyld[1], message) #The MORI will read the messages   
    return