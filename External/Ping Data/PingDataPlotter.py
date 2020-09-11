import matplotlib.pyplot as plt
import numpy as np
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
from termcolor import colored
import os

dir = input("Please enter the Ping Data directory name:")

pingData = {}

# Load the data from .npz files saved in subdirectory
for filename in os.listdir("Data/" + dir):
    if filename.endswith(".npz") and filename.startswith("PD_"):
        esp = filename.split("_", 2)[1] 
        pingData[esp] = np.load("Data/"+ dir + "/" + filename)
        continue
    else:
        continue


n_bins = 50

fig, ax = plt.subplots(tight_layout=True)

# Check length of the Ping Data Times and Integrity are the same
if not (len(pingData[esp]["arr_0"]) == len(pingData[esp]["arr_1"])):
    print(colored("'Time' and 'Integrity' data array lengths mismatched", "red"))

arrayLength = len(pingData[esp]["arr_0"])

for esp in pingData.keys(): #redundant?
    # Check how many data corruptions occured
    faultCount = 0
    faultList = []
    timeoutCount = 0

    for lv in range(0, arrayLength): # boolean array 'False' indicates error
        if not pingData[esp]['arr_1'][lv]:
            faultCount += 1
            faultList.append(True)
            if pingData[esp]['arr_0'][lv] == np.inf: # Check if timeout occured
                timeoutCount += 1
        else:
            faultList.append(False)
    if faultCount == 0:
        print(colored(esp + ": No ping packets had faults", "green"))
    else:
        print(colored(esp + ": " + str(faultCount) + " ping packets had faults, (" + str(100.0*float(faultCount)/float(arrayLength)) + " %) of which "\
        + str(timeoutCount) + " were timeouts", "red")) # timedout counts as corrupted

    pingDataTime = np.delete(pingData[esp]['arr_0'], faultList) # Remove faulty data
    
    # We can set the number of bins with the `bins` kwarg
    ax.hist(pingDataTime, bins=len(pingDataTime), density=True, histtype='step', cumulative=-1)
    
plt.xticks(rotation='vertical')
plt.legend(pingData.keys())
plt.yscale("log")
plt.xlabel("Latency (ms)")
plt.ylabel("CCDF")

plt.show()