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
            'xkcd:lavender',
            'xkcd:light navy',
            'xkcd:green',
            'xkcd:gold',
            'xkcd:crimson',
            'xkcd:dark beige',
            'xkcd:lavender',
            'xkcd:light navy',
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
    "000AC172": "1 Away",
    "00D00BD5": "4 Away",
    "000AC175": "3 Away",
    "000AC170": "Hub",
    "000AC173": "2 Away"
}

order_dict =  {
    "1 Away": 1,
    "4 Away": 4,
    "3 Away": 3,
    "Hub": 0,
    "2 Away": 2
}

pltNum = [None]*18

def checkName(name):
    try:
        return names.idsToName[name]
    except KeyError:
        return name


filename = input("Please enter the Ping Data directory name:")
print()

#read csv into dict
pingData = {}
# print(filename[-4:])
if filename[-4:] == '.csv':
    fileTitle = 'Data/' + filename
else:
    fileTitle = 'Data/' +filename + '.csv'
with open(fileTitle, 'r',newline='') as f:
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

# fig, ax = plt.subplots(tight_layout=True)
fig, ax = plt.subplots()

# print(pingData.keys())
for esp in pingData.keys():
    # print(esp)
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
    if data_size == 0:
        continue
    data_set=sorted(set(pingDataTime))
    bins=np.append(data_set, data_set[-1]+1)
    counts, bin_edges = np.histogram(pingDataTime, bins=bins, density=False)
    counts=counts.astype(float)/data_size
    cdf = 1-np.cumsum(counts)
    pltNum[color_iter] = plt.plot(bin_edges[0:-1], cdf, linestyle='-', linewidth=2, color=colors[color_iter], label=checkName(esp))    
    # pltNum[color_iter] = plt.plot(bin_edges[0:-1], cdf, linestyle='-', linewidth=2, color=colors[color_iter], label=checkName(esp), clip_on=False)    
    color_iter += 1


# # labels = [checkName(x) for x in pingData.keys()]
# labels = [hub_name_dict[x] for x in pingData.keys()]
# handles, _ = ax.get_legend_handles_labels()
# order = [4, 0, 1, 2, 3,4]
# # labels, handles = zip(*sorted(zip(labels, handles), key=lambda t: t[0]))
# order = [order_dict[x] for x in labels]
# print(order)
# print(labels)
# print()
# labels = [label for _, label in sorted(zip(order,labels))]
# handles = [handles for _, handles in sorted(zip(order,handles))]

# # handles = [pltNum[idx][0] for idx in order]
# # labels = [labels[idx] for idx in order]
# print(handles, labels)

handles, labels = ax.get_legend_handles_labels()

# handles, labels = plt.gca().get_legend_handles_labels()
# order = [1,0,2,3]
# print(handles, labels)
# plt.legend([handles[idx] for idx in order],[labels[idx] for idx in order])
    
plt.xticks(rotation='vertical')
leg = plt.legend(handles, labels, loc='upper right', fontsize=14, markerscale=5, frameon=False)
for legobj in leg.legendHandles:
    legobj.set_linewidth(4.0)

ax.set_ylim(0,1.01)
ax.set_xlim(0,300)
# ax.set_xlim(0,160)
# plt.yscale("log")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.tick_params(axis='both', which='major', labelsize=11)

# plt.tight_layout()

plt.xlabel("Latency (ms)", fontsize=14)
plt.ylabel("CCDF", fontsize=14)

fig.set_size_inches(6, 3.5)

figName = input("Please enter a file name for the saved figure:")
if figName != "":
    plt.savefig("Data/"+ figName + ".png", dpi=256) #Save plot as image 
else:
    print("Not saving picture")
plt.show()