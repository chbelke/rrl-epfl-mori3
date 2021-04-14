import matplotlib.pyplot as plt
import numpy as np
import math
from matplotlib import colors
from matplotlib.ticker import PercentFormatter
from termcolor import colored
import os
import csv

import names

colors = ['xkcd:navy',
            'xkcd:green',
            'xkcd:gold',
            'xkcd:crimson',
            'xkcd:beige',
            'xkcd:lavender',
            'xkcd:pink',
            'xkcd:purple',
            'xkcd:orange',
            'xkcd:turquoise',
            'xkcd:grey',
            'xkcd:navy',
            'xkcd:green',
            'xkcd:gold',
            'xkcd:crimson',
            'xkcd:beige',
            'xkcd:lavender',
            'xkcd:pink',
            'xkcd:purple',
            'xkcd:orange',
            'xkcd:turquoise',
            'xkcd:grey'
            ]
dark_colors = ['xkcd:dark navy',
            'xkcd:dark green',
            'xkcd:dark gold',
            'xkcd:dark red',
            'xkcd:dark beige',
            'xkcd:dark lavender',
            'xkcd:dark pink',
            'xkcd:dark purple',
            'xkcd:dark orange',
            'xkcd:dark turquoise',
            'xkcd:dark grey',
            'xkcd:dark navy',
            'xkcd:dark green',
            'xkcd:dark gold',
            'xkcd:dark red',
            'xkcd:dark beige',
            'xkcd:dark lavender',
            'xkcd:dark pink',
            'xkcd:dark purple',
            'xkcd:dark orange',
            'xkcd:dark turquoise',
            'xkcd:dark grey'
            ]

# pltNum = [None]*6

def checkName(name):
    try:
        return names.idsToName[name]
    except KeyError:
        return name


filename = input("Please enter the Ping Data directory name:")
print()

#read csv into dict
logData = {}
init_time = {}
# print(filename[-4:])
if filename[-4:] == '.csv':
    fileTitle = filename
else:
    fileTitle = filename + '.csv'
with open(fileTitle, 'r',newline='') as f:
    reader = csv.reader(f)
    line_count = 0
    for row in reader:
        if line_count < 1:
            line_count += 1
            continue
        if row[0] not in logData:
            logData[row[0]] = []
            init_time[row[0]] = row[1]

        logData[row[0]].append(row)

fig, ax = plt.subplots()
print([checkName(esp) for esp in logData.keys()])
print("Count: {}".format(len(logData.keys())))

print("".rjust(10) + "Angle".rjust(5) + "Ext".rjust(10) + "Orient".rjust(10) + "Count".rjust(10))
for i, esp in enumerate(logData.keys()):
    #if checkName(esp) != 'Carl':
    # if checkName(esp) != 'Carl':
    #     continue

    init = int(init_time[esp])
    time_ang = []
    ang1 = []
    ang2 = []
    ang3 = []
    time_ext = []
    ext1 = []
    ext2 = []
    ext3 = []
    time_orient = []
    orient1 = []
    orient2 = []
    orient3 = []    
    for row in logData[esp]:
        # print(row, row[2:4], row[1])
        
        if any([int(x)!=0 for x in row[2:4]]):
            time_ang.append((int(row[1])-init)/1000)
            ang1.append(int(row[2]))
            ang2.append(int(row[3]))
            ang3.append(int(row[4]))

        if any([int(x)!=0 for x in row[5:7]]):
            time_ext.append((int(row[1])-init)/1000)
            ext1.append(int(row[5]))
            ext2.append(int(row[6]))
            ext3.append(int(row[7]))

        if any([int(x)!=0 for x in row[8:10]]):
            time_orient.append((int(row[1])-init)/1000)
            orient1.append(int(row[8]))
            orient2.append(int(row[9]))
            orient3.append(int(row[10]))            


    deltas = [j-i for i,j in zip(time_ang, time_ang[1:])]
    avg_ang = int(np.mean(deltas))

    # print(checkName(esp).ljust(10)+ "{}".format(avg_ang).rjust(5))

    # deltas = [j-i for i,j in zip(time_ext, time_ext[1:])]
    # avg_ext = int(np.mean(deltas))

    #deltas = [j-i for i,j in zip(time_orient, time_orient[1:])]
    #avg_orient = int(np.mean(deltas))


    

    # print(checkName(esp).ljust(10)+ "{}".format(avg_ang).rjust(5) + 
    #     "{}".format(avg_ext).rjust(10) + "{}".format(avg_orient).rjust(10) +
    #     "{}".format(len(time_ang)).rjust(10))
    # print(time_orient)
    # print(deltas)




    plt.figure(1)
    plt.plot(time_ang, ang1, linestyle='-', linewidth=2, marker=".", label="a1 "+checkName(esp), color=colors[i])
    plt.plot(time_ang, ang2, linestyle='-', linewidth=2, marker=".", label="a2 "+checkName(esp), color=colors[i])
    plt.plot(time_ang, ang3, linestyle='-', linewidth=2, marker=".", label="a3 "+checkName(esp), color=dark_colors[i])
    
    # plt.figure(2)
    # plt.plot(time_orient, orient1, linestyle='-', linewidth=2, label="o1 "+esp)  
    # plt.plot(time_orient, orient2, linestyle='-', linewidth=2, label="o2 "+checkName(esp), color=colors[i])  
    # plt.plot(time_orient, orient3, linestyle='-', linewidth=2, label="o3 "+checkName(esp), color=dark_colors[i])  
    # plt.plot(time_ext, ext2, linestyle='-', linewidth=2, label=esp)  
    # plt.plot(time_ext, ext3, linestyle='-', linewidth=2, label=esp)  
    # plt.plot(time_ext, ext1, linestyle='-', linewidth=2, label=esp)  
    # plt.plot(time_orient, orient1, linestyle='-', linewidth=2, label=checkName(esp))  
	
    print(esp)
    print("average values")
    print(np.mean(orient1))
    print(np.mean(orient2))
    print(np.mean(orient3))

lgnd = plt.legend(bbox_to_anchor=(1.01, 1), loc='upper left', numpoints=1, fontsize=10)
lgnd.legendHandles[0]._legmarker.set_markersize(8)
lgnd.legendHandles[1]._legmarker.set_markersize(8)
plt.ylim(600, 3000)
plt.show()
    
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

# handles, labels = ax.get_legend_handles_labels()

# handles, labels = plt.gca().get_legend_handles_labels()
# order = [1,0,2,3]
# print(handles, labels)
# plt.legend([handles[idx] for idx in order],[labels[idx] for idx in order])

# for i in range(1,2):
#     plt.figure(i)

plt.xticks(rotation='vertical')
leg = plt.legend(loc='upper right', fontsize=14, markerscale=5, frameon=False)
for legobj in leg.legendHandles:
    legobj.set_linewidth(4.0)

# ax.set_xlim(0,160)
# plt.yscale("log")
ax.spines["top"].set_visible(False)
ax.spines["right"].set_visible(False)
ax.tick_params(axis='both', which='major', labelsize=11)


# fig.set_size_inches(6, 3.5)

plt.show()