import time
from termcolor import colored

def getPing(self, pyld, espNum):
	t2 = time.perf_counter()
	t1 = self.getTsPingDict(espNum)
	data = self.getDataPingDict(espNum)
	delta = str((t2-t1)*1000) #in ms
	if (pyld[1] == data):  # check data integrity
		print(colored("Round trip time: " + delta + "ms", "magenta"))
	else:
		print(colored("Data integrity check failed... " + delta + "ms", "red"))		
	return delta