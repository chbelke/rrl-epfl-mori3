'''
**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Most recent handler of

Scans for modules with SSID "helloThere", and then relays the measured signal 
strength and corresponding MAC address over the network.

Used in conjunction with wifiHandler.py, distanceRanger.py, getData.py, and
mqttAnalyzer.py

Before flashing to ESP8266, please change clientName, publishName, and recieveName
to their appropriate values.

**********************************************************************************
'''


import serial, time
import datetime
import sys, select, os
# from argparse import ArgumentParser
import argparse
import numpy as np
import paho.mqtt.client as mqtt
import copy
# from scipy.optimize import minimize, least_squares
import matplotlib.pyplot as plt
from termcolor import colored


clientName = "boss"
mqttServer = "192.168.0.51";
mqttPort = 1883;
Connected = False

macIds = [None]*10
macDict = {}

data = []

macCallTime = time.time()
verCallTime = time.time()

distance = 1.0

version = 1.5


def getArgs():
	parser = argparse.ArgumentParser(description='Process MQTT information from Mori.')
	parser.add_argument("-f", "--fileName", type=str, default='data',
                    	help='file to save accumulated data into')
	parser.add_argument("-n", "--numModules", type=int, default=10,
                    	help='Maximum number of ESP modules.')	
	return parser.parse_args()


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
	global macCallTime
	print(colored("Connected with result code "+str(rc),'green'))
	# Subscribing in on_connect() means that if we lose the connection and
	# reconnect then subscriptions will be renewed.
	# client.subscribe("$SYS/#")
	Connected = True
	# subTopics(args.numModules)
	subTopics(10)
	client.publish("esp/rec","mac")
	print("Calling for MAC addresses")
	macCallTime = time.time()


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
	global macCallTime
	global distance
	# print(msg.topic+" "+str(msg.payload))
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
				# print(espNum, macDict[macaddr], rssi)
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

		elif(pyld[0] == 'VER:'):
			espVer = float(pyld[1])
			if(espVer == version):
				print("ESP{} Software up to date".format(espNum))
				# client.publish("esp{}/rec".format(espNum),"vg")   #version good
				client.publish("esp/rec","vg")   #version good
			elif(espVer < version):
				print("ESP{} Software out date".format(espNum))
				print("ESP{}: {}".format(espNum,espVer))
				print("Curr: {}".format(version))
				client.publish("esp{}/rec".format(espNum),"vb")   #version bad
			elif(espVer > version):
				print("Kevin is an idiot and forgot to update this to match newest version")
				print("ESP{}: {}".format(espNum,espVer))
				print("Curr: {}".format(version))				
				client.disconnect()
				client.loop_stop()
				sys.exit()

		elif(pyld[0] == 'ERR:'):
			print(pyld[1])
			print(colored('hello', 'red'))



def subTopics(n):
	print("Connecting to: ")
	for i in range(n):
		print("esp"+str(i)+"/pub")
		client.subscribe("esp"+str(i)+"/pub")



def calcResults(fileName):
	dataCpy = copy.deepcopy(data)
	if not dataCpy:
		print(colored("No data recieved",'yellow'))
		return
	numEsps = len(dataCpy)
	result = np.zeros(shape=(numEsps, numEsps))
	for x in range(numEsps):
		for y in range(numEsps):
			if x == y:
				continue
			if(dataCpy[x][y] is not None):
				result[x,y] = len(dataCpy[x][y])
	print("Amount of Data: ")
	print(result)

	print("Saving...")
	np.save('test.npy', data)



client = mqtt.Client(clientName)
client.on_connect = on_connect
client.on_message = on_message


def main():
	startTime = time.time()
	args = getArgs()
	loopTime = time.time()
	print("Connecting...")
	client.connect(mqttServer, mqttPort)

	client.loop_start()

	try:
		while True:
			if time.time() - loopTime > 10:
				loopTime = time.time()
				print("Time Elapsed: {:.3f}".format(loopTime-startTime))
				print("")
				calcResults(args.fileName)
				print("")
			time.sleep(0.1)
	 
	except KeyboardInterrupt:
		print("exiting")
		client.disconnect()
		client.loop_stop()

if __name__ == '__main__':
	sys.exit(main())

