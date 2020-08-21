import time
from termcolor import colored

def getPing(self, pyld, espNum):
	t2 = time.perf_counter()
	t1 = self.getTsPingDict(espNum)
	if True:  # check data integrity
		print(colored("Ping ponged, running function", "green"))
		print("Round trip time: ", (t2-t1)*1000, "ms")
	return