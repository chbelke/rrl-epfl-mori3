# Joint-Utility-Mori-Program

**Authors:** Christoph Belke, Kevin Holdcroft, Alex Sigrist

This repo acts as all of the code relevant to the control and communication of RRL's Mori v3.

### Related Publications:
[Mori v1] C. H. Belke and J. Paik. **Mori: A Modular Origami Robot**. *IEEE/ASME Transactions on Mechatronics,* vol. 22, no. 5, pp. 2153-2164, Oct. 2017. **[LINK](http://ieeexplore.ieee.org/stamp/stamp.jsp?tp=&arnumber=7907345&isnumber=8067604)**


If you use Mori v3 within an academic work, please cite:

    @article{
    	*INSERT BIBTEX HERE*
     }


# Overview
 - MoriController.x: Internal electronics control
 - ESP8266: Control for ESP8266EX wifi chip
 - External: Overall external control


# Prerequisites
The microchip was programmed via MPLAB X IDE v.5.

 ## ESP8266 wifi
Firmware flashed via Arduino IDE.

ESP8266 hardware must be installed: https://arduino-esp8266.readthedocs.io/en/latest/installing.html

The above includes the following libraries:

`ESP8266WiFi.h`
`ArduinoOTA.h`

Other libraries:
`PubSubClient.h` (https://github.com/knolleary/pubsubclient, included in repo)

 ## Global Control:
Implemented in Python 3.6.6

Nonstandard libraries:
`paho.mqtt.client` (https://pypi.org/project/paho-mqtt/)
`termcolor` (https://pypi.org/project/termcolor/)

 ## Other:
 MQTT broker is handled by a Raspberry Pi B+
 (simple tutorial [HERE](https://randomnerdtutorials.com/how-to-install-mosquitto-broker-on-raspberry-pi/))
