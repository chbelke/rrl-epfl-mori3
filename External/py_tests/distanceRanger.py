import serial, time
import datetime
import sys, select, os
from optparse import OptionParser
import numpy as np
import paho.mqtt.client as mqtt
import copy
from scipy.optimize import minimize, least_squares
import matplotlib.pyplot as plt

clientName = "boss"
mqttServer = "192.168.0.51";
mqttPort = 1883;
Connected = False

macIds = [None]*10
macDict = {}

data = []
# data.append([])

macCallTime = time.time()

plt.ion()
fig = plt.figure()
ax = fig.add_subplot(111)
plt.axis('equal')
xPlot, yPlot = [],[]
sc = ax.scatter(xPlot,yPlot)
plt.draw()
pastLocs = np.zeros((1,2))


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
	global macCallTime
	print("Connected with result code "+str(rc))
	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	# client.subscribe("$SYS/#")
	Connected = True
	subTopics(10)
	client.publish("esp/rec","mac")
	print("Calling for MAC addresses")
	macCallTime = time.time()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	global macCallTime
	print(msg.topic+" "+str(msg.payload))
	if(msg.topic[:3]=="esp"):
		espNum = int(msg.topic[3])
		pyld = str(msg.payload)
		pyld = pyld[2:-1]
		pyld = pyld.rsplit(' ')
		cmd = pyld[0]

		if(pyld[0] == 'DST:'):
			macaddr = pyld[1][:-1]
			rssi = int(pyld[2])
			if(macDict.get(macaddr) is not None):	#check if we know other mac
				print(espNum, macDict[macaddr], rssi)
				if len(data[espNum][macDict[macaddr]]) > 9:
					del data[espNum][macDict[macaddr]][0]
				data[espNum][macDict[macaddr]].append(rssi)
			else:
				if time.time() - macCallTime > 3.0:
					client.publish("esp/rec","mac")
					print("Calling for MAC addresses")
					macCallTime = time.time()

		elif(pyld[0] == 'MAC:'):
			mac = pyld[1].replace(":", "")
			mac = mac.lower()
			mac = mac[:1] + 'e' + mac[2:] #Hack
			if macDict.get(mac) is None:
				macDict[mac] = espNum
				data.append([])
				for i in range(len(data)):
					data[i].append([])
				if(len(data)>1):
					data[-1] = [[] for _ in range(len(data[0]))]

def printData():
	print(data[0])
	if(data[1]):
		print(data[1])
	if(data[2]):
		print(data[2])

def subTopics(n):
	print("Connecting to: ")
	for i in range(n):
		print("esp"+str(i)+"/pub")
		client.subscribe("esp"+str(i)+"/pub")

client = mqtt.Client(clientName)
client.on_connect = on_connect
client.on_message = on_message


Tx = 5.0
freq = 2432.0
params = [15.24655733007094, 1.3407457780407428, -0.027669704372142324]
x0 = params[0]
gamma = params[1]
K = params[2]
def getDistance(Rx):
    expon = (Tx - Rx + x0 -(20.0*np.log10(freq)))/(20.0*gamma)
    return np.power(10, expon)+K


def findLoc(vals, dist, numMori):
	# print(dist)
	error = 0.0
	x = np.zeros(numMori)
	y = np.zeros(numMori)
	x[2:numMori] = vals[0:numMori-2]
	y[1:numMori] = vals[numMori-2:]
	for i in range(0,numMori):
		for j in range(0,numMori):
			if i >= j:
				continue
			distCalc = np.sqrt((x[i]-x[i-j])**2+(y[i]-y[i-j])**2)
			error += np.abs(dist[i,j]-distCalc)
	return error


def calcLeastSquare(dist):
	numMori = dist.shape[0]

	result = minimize(
		fun=findLoc,      
		x0=np.ones(numMori*2-3),
		# x0=np.array([1,2]),
		args=(dist,numMori),
		method='CG',
		# method='BFGS',
		options={
		  'maxiter': 500,
		  'disp': True
		})
	print("Success: ", result.success)
	print("Message: ", result.message)
	print("Iterats: ", result.nit)
	x = result.x

	print("********************\n")

	print("Result: ")
	# print(numMori)
	locations = np.zeros((numMori,2))
	for i in range(0,numMori):
		if(i==1):
			locations[i,1]=x[i-1]
		elif(i>1):
			locations[i,0]=x[i-1]
			locations[i,1]=x[i-1+numMori-2]
		print("Mori {}: ({}, {})".format(i, locations[i,0], locations[i,1]))

	print("********************\n")
	return locations



def calcResults():
	dataCpy = copy.deepcopy(data)
	if not dataCpy:
		print("returning")
		return
	numEsps = len(dataCpy)
	result = np.zeros(shape=(3,numEsps, numEsps))
	for x in range(numEsps):
		for y in range(numEsps):
			if x == y:
				continue
			if(dataCpy[x][y] is not None):
				result[0,x,y] = len(dataCpy[x][y])
				result[1,x,y] = np.mean(dataCpy[x][y])
				result[2,x,y] = np.std(dataCpy[x][y])
	print("Result:")
	print(result)

	avgBoth = np.zeros(shape=(numEsps, numEsps))
	distEst = np.zeros(shape=(numEsps, numEsps))
	for x in range(numEsps):
		for y in range(numEsps):
			if x == y or x > y:
				continue
			ttl = result[0,x,y]+result[0,y,x]
			avgBoth[x,y] = (result[0,x,y]/ttl)*result[1,x,y]+(result[0,y,x]/ttl)*result[1,y,x]
			distEst[x,y] = getDistance(avgBoth[x,y])
	print("Avg: ")
	print(avgBoth)
	print("Sig: ")
	print(distEst)

	if np.isnan(distEst).any():
		print("has NaNs")
		return
	if distEst.shape[0] < 2:
		print("needs 2 Mori")
		return
	locations = calcLeastSquare(distEst)

	plotLocations(locations)
	plotStDev(locations, result[1], result[2])


def plotLocations(locs):
	global pastLocs
	ax.clear()
	if len(pastLocs) >= 2:
		plt.scatter(pastLocs[:,0], pastLocs[:,1], s=10, marker='o')
		pastLocs = np.append(pastLocs,locs, axis=0)
	else:
		pastLocs = copy.deepcopy(locs)
	
	plt.scatter(locs[:,0], locs[:,1], s=30, marker='^')
	for i in range(locs.shape[0]):
		plt.text(locs[i,0] * (1 + 0.01), 
				 locs[i,1] * (1 + 0.01),
				 i, fontsize=12)

	sc.set_offsets(np.c_[locs[:,0],locs[:,1]])


def plotStDev(locs, means, stDev):
	numEsps = stDev.shape[0]
	print(locs)
	print("")
	for x in range(numEsps):
		for y in range(numEsps):
			if x == y:
				continue
			distLong = getDistance(means[x,y] + stDev[x,y])
			distShort = getDistance(means[x,y] + stDev[x,y])
			
			
			# if x < y:
			a = locs[y] - locs[x]
			# else:
			# 	a = locs[x] - locs[y]

			a = a/(5*np.sum(a))
			varLoc = locs[x]+a
			plt.plot((locs[x,0],varLoc[0]),(locs[x,1],varLoc[1]), color='black')
			print(x,y, a, locs[x], varLoc)
			# else:
			# 	plt.plot(locs[y],locs[y]+a)
			# 	print(x,y, a, locs[y]+a)
	fig.canvas.draw_idle()
	plt.pause(0.1)
	# input()




def main():
	# calcLeastSquare([1])
	# return

	startTime = time.time()
	loopTime = time.time()
	# client = mqtt.Client(clientName)
	print("Connecting...")
	client.connect(mqttServer, mqttPort)

	client.loop_start()

	try:
		while True:
			if time.time() - loopTime > 3:
				loopTime = time.time()
				print("Time Elapsed: {:.3f}".format(loopTime-startTime))
				print("")
				calcResults()
				print("")
			time.sleep(0.1)
	 
	except KeyboardInterrupt:
		print("exiting")
		client.disconnect()
		client.loop_stop()

if __name__ == '__main__':
	sys.exit(main())

