import serial, time
import datetime
import sys, select, os
from optparse import OptionParser
import numpy as np
from matplotlib import pyplot as plt


def getOptions():
    parser = OptionParser()
    parser.add_option("-p", "--port", dest="serialPort",
                    help="Choose serial port")
    parser.add_option("-f", "--file", dest="fileName",
                    help="Select file name")
    (options, args) = parser.parse_args()
    return options


Tx = 5.0
freq = 2432.0
x0 = 14.34477324
gamma = 0.78611627

params = [15.24655733007094, 1.3407457780407428, -0.027669704372142324]
x0 = params[0]
gamma = params[1]
K = params[2]
def getDistance(Rx):
    # print(5.0 - signal - constN)
    # expon = (Tx - Rx - x0 -(20.0*np.log10(freq)))/20.0
    expon = (Tx - Rx + x0 -(20.0*np.log10(freq)))/(20.0*gamma)
    return np.power(10, expon)+K


def plotThings(data):
    ax = fig.add_subplot(111)

    mx = np.array([])
    mn = np.array([])
    for x in range(data.shape[0]):
        for y in range(data.shape[1]):
            data[x,y] = np.array(data[x,y])
            if x == y: 
                continue
            mx = np.append(mx, (np.amax(data[x,y])))
            mn = np.append(mx, (np.amin(data[x,y])))

    mx = np.amax(mx)
    mn = np.amin(mn)
    
    print(mx, mn)

    c = ['C0', 'C1', 'C2', 'C3', 'C4', 'C5', 'C6', 'C7', 'C8', 'C9']
    for x in range(data.shape[0]):
        for y in range(data.shape[1]):
            if x == y: 
                continue
            if x > y:
                a = 0.80
            else:
                a = 1.0
            data[x,y] = data[x,y][data[x,y] > -70,None]
            plt.hist(data[x,y], int(mx-mn), alpha=a, color=c[x+(y*data.shape[0])], label=str(x)+str(y))
    
    plt.xlabel('DBm')
    plt.ylabel('Count')
    plt.title('DBm Measurements at 0.8 meters')
    plt.legend(loc='upper left')
    plt.savefig('08m.png')
    plt.show()



if __name__ == '__main__':
    opt = getOptions()
    data = np.load("test.npy")
    fig = plt.figure()
    plotThings(data)

    # closeSerial(ser)
