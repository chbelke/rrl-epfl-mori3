import serial, time
import datetime
import sys, select, os
from argparse import ArgumentParser
import numpy as np
import paho.mqtt.client as mqtt
import copy
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
verCallTime = time.time()

distance = 1.0

version = 1.0


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
			print(pyld)
			print(espVer)
			if(espVer == version):
				print("ESP{} Software up to date".format(espNum))
				client.publish("esp{}/rec".format(espNum),"vg")   #version good
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



def subTopics(n):
	print("Connecting to: ")
	for i in range(n):
		print("esp"+str(i)+"/pub")
		client.subscribe("esp"+str(i)+"/pub")

client = mqtt.Client(clientName)
client.on_connect = on_connect
client.on_message = on_message


def main():
	startTime = time.time()
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
				calcResults()
				print("")
			time.sleep(0.1)
	 
	except KeyboardInterrupt:
		print("exiting")
		client.disconnect()
		client.loop_stop()

if __name__ == '__main__':
	sys.exit(main())

