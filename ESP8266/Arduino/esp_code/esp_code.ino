
/**********************************************************************************
  Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL.

  Tool to compare signal strength between two ESP8266 modules.

  Scans for modules with SSID "helloThere", and then relays the measured signal
  strength and corresponding MAC address over the network.

  Used in conjunction with wifiHandler.py, distanceRanger.py, getData.py, and
  mqttAnalyzer.py

  Before flashing to ESP8266, please change clientName, publishName, and recieveName
  to their appropriate values.

**********************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>
#include "Led.h"

const char* brn_ssid = "rrl_wifi";
const char* brn_password =  "Bm41254126";

// const char* brn_ssid = "dole11b";
// const char* brn_password =  "delormet24";

// const char* mqttServer = "192.168.0.52";
// const char* mqttServer = "RRLB201957";
// const char* mqttServer = "mqtt-host";
const char* mqttServer = "192.168.1.2";

//const char* mqttServer = "10.252.50.209";
const int mqttPort = 1883;

char clientName[16];
char publishName[36];
char recieveName[36];

const float softwareVersion = 0.5;

char* cmdLine[] = {"mac", "gver", "bver", "spudp", "hello", "g_shape", "udp", "noudp", "verb", "noverb", "rel", "hi1", "hi2", "hi3"};

char stringIP[16];
char charMAC[18];

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise

char runState = 0;

unsigned long lastMessage = millis();
unsigned long lastMacPub = millis();
int moriShape[6] = {200, 200, 200, 0, 0, 0};

WiFiClient espClient;
PubSubClient client(espClient);

bool flag_udp = false;
bool verbose_flag = false;

#define LED_PIN 0
Led wifi_ind_led(LED_PIN);

//--------------------------- Start ----------------------------------------//
void setup()
{
  Serial.begin(115200);
  delay(500);
  sprintf(clientName, "%08X", ESP.getChipId());
  sprintf(publishName, "esp/%s/pub", clientName);
  sprintf(recieveName, "esp/%s/rec", clientName);
  verbose_print("ChipID: ");
  verbose_println(clientName);
  verbose_println(publishName);
  verbose_println(recieveName);


  //--------------------------- Wifi ------------------------------------//
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(brn_ssid, brn_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    verbose_println("Connecting to WiFi..");
    wifi_ind_led.Toggle();
  }
  verbose_println("Connected to the WiFi network");

  for(int i = 0; i<=6; i++)
  {
    delay(100);
    wifi_ind_led.Toggle();
  }


  IPAddress IP = WiFi.localIP();
  sprintf(stringIP, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3] );
  verbose_print("IP address: ");
  verbose_println(stringIP);

  for(int i = 0; i < 17; i++)
    charMAC[i] = (char)WiFi.macAddress()[i];
  verbose_print("MAC: ");
  verbose_println(charMAC);  

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    verbose_println("Connecting to MQTT...");
    wifi_ind_led.Toggle();
    if (client.connect(clientName))
    {
      verbose_println("connected");
    } else {
      verbose_print("failed with state ");
      verbose_println(client.state());
      delay(2000);
    }
  }
  wifi_ind_led.On();

  //--------------------------- MQTT -----------------------------------------//

  client.publish(publishName, "INFO: Hello from ESP8266");
  client.subscribe(recieveName);
  client.subscribe("esp/rec");

  //----------------------- OAT Handling--------------------------------------//
  handleOTA();
  
  //----------------------- UDP Handling--------------------------------------//
  verbose_flag = false;
}


//----------------------- Loooooooop--------------------------------------//

void loop()
{
  switch(runState){
    case 0:   //booting up
      pubVersion();
      delay(500);
      break;
    case 1:   //version bad - enable auto updates
      enableOTA();
      runState = 2;
    case 2:   //handle OTA
      ArduinoOTA.handle();
      break;
    case 3:   //everything is good
      normalOp();
      break;
    case 4:   //WiFi hub
      startUDP();
      flag_udp = true;
      publish("UDP: Start");
      runState = 5;
      break;
    case 5:
      readUDP();
      normalOp();
      break;
    case 6:
      stopUDP();
      normalOp();
      publish("UDP: Stop");
      runState = 3;
  }

  client.loop();
}


void publish(char* buff)
{
  verbose_print("Sent ");
  verbose_print(buff);
  verbose_print(" via ");
  if(runState != 5)
  {
    client.publish(publishName, buff);
    verbose_println("MQTT");
  } 
  else
  {
    writeUDP(buff);
    verbose_println("UDP");
  }
}


void normalOp()
{
//  scanWifis();
  long unsigned currentTime = millis();
  if(currentTime - lastMacPub > 2000)
  {
    pubMac("ON: "); //Publish connexion message
    lastMacPub = millis();
  }
  
  if (abs(lastMessage - millis()) > 10000)
  {
    char buff[50];
    String runString = String("INFO: RunState: ") + int(runState);
    runString.toCharArray(buff, 30);
    publish(buff);
//    client.publish(publishName, "INFO: Just chillin");
    lastMessage = millis();
  }
}
