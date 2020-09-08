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

for esp in pingData.keys(): #redundant?
    # Check how many data corruptions occured
    faultCount = 0
    for lv in pingData[esp]['arr_1']: # boolean array 'False' indicates error
        if not lv:
            faultCount += 1
    if faultCount == 0:
        print(colored(esp + " :No ping packets were corrupted", "green"))
    else:
        print(colored(esp + ": " + str(faultCount) + " ping packets were corrupted", "red"))

    # We can set the number of bins with the `bins` kwarg
    ax.hist(pingData[esp]['arr_0'], bins=len(pingData[esp]["arr_0"]), density=True, histtype='step', cumulative=-1)
    
plt.xticks(rotation='vertical')
plt.yscale("log")

plt.show()