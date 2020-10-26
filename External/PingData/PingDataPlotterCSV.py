import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
from termcolor import colored
import os
import csv

import names

colors = ['xkcd:light navy',
            'xkcd:green',
            'xkcd:gold',
            'xkcd:crimson',
            'xkcd:dark beige',
            'xkcd:lavender']          

color_iter = 0


hub_name_dict =  {
    "00056375": "Huzzah1",
    "0005659D": "Huzzah2",
    "000AC171": "Alfred",
    "000AC172": "Hub",
    "00D00BD5": "2 Away",
    "000AC175": "Darlene",
    "000AC170": "3 Away",
    "000AC173": "1 Away"
}

pltNum = [None]*6

def checkName(name):
    try:
        return names.idsToName[name]
    except KeyError:
        return name


filename = input("Please enter the Ping Data directory name:")
print()

#read csv into dict
pingData = {}
with open('Data/' +filename + '.csv', 'r',newline='') as f:
    reader = csv.reader(f)
    line_count = 0
    for row in reader:
        if line_count < 1:
            line_count += 1
            continue
        if row[0] not in pingData:
            pingData[row[0]] = {}
            pingData[row[0]]['time'] = []
            pingData[row[0]]['integrity'] = []
        pingData[row[0]]['time'].append(float(row[1]))
        pingData[row[0]]['integrity'].append(str(row[2])==" True") 


#remove first and last ping
for module in pingData:
    pingData[module]['time'] = pingData[module]['time'][1:-1]
    pingData[module]['integrity'] = pingData[module]['integrity'][1:-1]

fig, ax = plt.subplots(tight_layout=True)

for esp in pingData.keys():
    # Check how many data corruptions occured
    faultCount = 0
    faultList = []
    timeoutCount = 0

    arrayLength = len(pingData[esp]["time"])

    for lv in range(0, len(pingData[esp]['integrity'])): # boolean array 'False' indicates error
        if pingData[esp]['integrity'][lv] == False:
            faultCount += 1
            faultList.append(True)
            if math.isinf(pingData[esp]['time'][lv]): # Check if timeout occured
                timeoutCount += 1
        else:
            faultList.append(False)
    if faultCount == 0:
        print(colored(checkName(esp) + ": " + str(arrayLength) + " ping packets read without faults", "green"))
    else:
        print(colored(checkName(esp) + ": " + str(faultCount) + " out of " + str(arrayLength) + " ping packets had faults ("\
        + str(100.0*float(faultCount)/float(arrayLength)) + " %), of which " + str(timeoutCount) + " were timeouts", "red")) # timedout counts as corrupted

    pingDataTime = np.delete(pingData[esp]['time'], faultList) # Remove faulty data

    data_size=len(pingDataTime)
    data_set=sorted(set(pingDataTime))
    bins=np.append(data_set, data_set[-1]+1)
    counts, bin_edges = np.histogram(pingDataTime, bins=bins, density=False)
    counts=counts.astype(float)/data_size
    cdf = 1-np.cumsum(counts)
    pltNum[color_iter] = plt.plot(bin_edges[0:-1], cdf, linestyle='-', linewidth=2, color=colors[color_iter], label=checkName(esp))    
    color_iter += 1


labels = [checkName(x) for x in pingData.keys()]
labels = [hub_name_dict[x] for x in pingData.keys()]
handles, _ = ax.get_legend_handles_labels()
# labels, handles = zip(*sorted(zip(labels, handles), key=lambda t: t[0]))
order = [1, 0, 2, 3]
handles = [pltNum[idx][0] for idx in order]
labels = [labels[idx] for idx in order]
print(handles, labels)

# handles, labels = plt.gca().get_legend_handles_labels()
# order = [1,0,2,3]
# print(handles, labels)
# plt.legend([handles[idx] for idx in order],[labels[idx] for idx in order])
    
plt.xticks(rotation='vertical')
leg = plt.legend(handles, labels, loc='upper right', fontsize=14, markerscale=5, frameon=False)
for legobj in leg.legendHandles:
    legobj.set_linewidth(4.0)

ax.set_ylim(0,1)
ax.set_xlim(0,160)
# plt.yscale("log")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.tick_params(axis='both', which='major', labelsize=11)

plt.xlabel("Latency (ms)", fontsize=14)
plt.ylabel("CCDF", fontsize=14)

fig.set_size_inches(6, 3.5)

figName = input("Please enter a file name for the saved figure:")
if figName != "":
    plt.savefig("Data/"+ figName + ".png", dpi=256) #Save plot as image 
else:
    print("Not saving picture")
plt.show()