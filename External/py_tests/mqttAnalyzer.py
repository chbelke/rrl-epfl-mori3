import time
import datetime
import sys, select, os
from optparse import OptionParser
import numpy as np
import paho.mqtt.client as mqtt

clientName = "boss"
mqttServer = "192.168.0.51";
mqttPort = 1883;
Connected = False

macIds = [None]*10
macDict = {}

data = []
# data.append([])


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
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
				data[espNum][macDict[macaddr]].append(rssi)
				# print(data[espNum][macDict[macaddr]])
				# print(data)
			else:
				# if time.time() - macCallTime > 3.0:
				print("Calling for MAC addresses")
				client.publish("esp/rec","mac")

		elif(pyld[0] == 'MAC:'):
			print(data)
			mac = pyld[1].replace(":", "")
			mac = mac.lower()
			mac = mac[:1] + 'e' + mac[2:] #Hack
			print(len(data))
			if macDict.get(mac) is None:
				print("FAAAAAAAK: ", len(data))
				macDict[mac] = espNum
				data.append([])
				print(data)
				for i in range(len(data)):
					data[i].append([])
				if(len(data)>1):
					data[-1] = [[] for _ in range(len(data[0]))]
	print(macDict)
	printData()
	print("")

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

def showResults():
	print("showresults")

def main():
	startTime = time.time()
	loopTime = time.time()
	# client = mqtt.Client(clientName)
	print("Connecting...")
	client.connect(mqttServer, mqttPort)

	client.loop_start()

	try:
		while True:
			if time.time() - loopTime > 10:
				loopTime = time.time()
				print("Time Elapsed: {:.3f}".format(loopTime-startTime))
				# showResults()

			# client.publish("esp/rec","mac")
			time.sleep(0.1)
	 
	except KeyboardInterrupt:
		print("exiting")
		client.disconnect()
		client.loop_stop()

if __name__ == '__main__':
	sys.exit(main())

