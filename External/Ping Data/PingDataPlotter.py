import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
from termcolor import colored
import os

import names


def checkName(name):
    try:
        return names.idsToName[name]
    except KeyError:
        return name


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
    # print(arrayLength)

    for lv in range(0, len(pingData[esp]["arr_0"])): # boolean array 'False' indicates error
        if not pingData[esp]['arr_1'][lv]:
            faultCount += 1
            faultList.append(True)
            if math.isinf(pingData[esp]['arr_0'][lv]): # Check if timeout occured
                timeoutCount += 1
        else:
            faultList.append(False)
    if faultCount == 0:
        print(colored(checkName(esp) + ": " + str(arrayLength) + " ping packets read without faults", "green"))
    else:
        print(colored(checkName(esp) + ": " + str(faultCount) + " out of " + str(arrayLength) + " ping packets had faults, ("\
        + str(100.0*float(faultCount)/float(arrayLength)) + " %) of which " + str(timeoutCount) + " were timeouts", "red")) # timedout counts as corrupted

    pingDataTime = np.delete(pingData[esp]['arr_0'], faultList) # Remove faulty data
    
    # We can set the number of bins with the `bins` kwarg
    ax.hist(pingDataTime[1:], bins=len(pingDataTime[1:]), density=True, histtype='step', cumulative=-1)

labels = [checkName(x) for x in pingData.keys()]
print(labels)
    
plt.xticks(rotation='vertical')
# plt.legend(pingData.keys())
plt.legend(labels)
plt.yscale("log")
plt.xlabel("Latency (ms)")
plt.ylabel("CCDF")

figName = input("Please enter a file name for the saved figure:")
plt.savefig("Data/"+ dir + "/" + figName + ".png") #Save plot as image 
plt.show()