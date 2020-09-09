import time
from termcolor import colored

def getPing(self, pyld, espNum):
	t2 = time.perf_counter()
	t1 = self.getTsPingDict(espNum)
	data = self.getDataPingDict(espNum)
	delta = float((t2-t1)*1000) #in ms
	if (pyld[1] == data):  # check data integrity
		#print(colored("Round trip time: " + delta + "ms", "magenta"))
		self.resetPingBusy(espNum)
		self.addPingResult(espNum, delta, True)
	else:
		print(colored("Data integrity check failed... " + str(delta) + "ms", "red"))		
		self.addPingResult(espNum, delta, False)
	return