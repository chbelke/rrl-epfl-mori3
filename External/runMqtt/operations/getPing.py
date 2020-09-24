import time
from termcolor import colored

def getPing(self, pyld, espNum):
	t2 = time.perf_counter()
	t1 = self.pingHandler.getTsPingDict(espNum)
	data = self.pingHandler.getDataPingDict(espNum)
	delta = float((t2-t1)*1000) #in ms
	# print(data)
	# print(pyld[1])
	# print("".join(hex(ord(n)) for n in (pyld[1])))
	if (pyld[1] == data):  # check data integrity
		#print(colored("Round trip time: " + delta + "ms", "magenta"))
		self.pingHandler.resetPingBusy(espNum)
		self.pingHandler.addPingResult(espNum, delta, True)
	else:
		print(colored("Data integrity check failed... " + str(delta) + "ms", "red"))		
		self.pingHandler.resetPingBusy(espNum)
		self.pingHandler.addPingResult(espNum, delta, False)
	return