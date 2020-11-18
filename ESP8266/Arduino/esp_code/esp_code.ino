
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
char clientLetter = 255;

const float softwareVersion = 0.5;

char *cmdLine[] = {"mac", "gver", "bver", "spudp", "hello",
                "g_shape", "udp", "noudp", "verb", "noverb",
                "rel", "cont", "nocon", "rled", "gled",
                "bled", "wedge", "rshape", "redge","rang",
                "rorient", "rwedge", "rneigh", "nowifi", "wifi",
                "reset", "preset", "png", "led", "coup", 
                "extension", "angle", "party", "hub", "nohub",
                "frt", "frf", "flt", "flf", "f1t", //frt = flag rot true; flf - flag lin false
                "f1f", "f2t", "f2f", "f3t", "f3f", 
                "f4t", "f4f", "f5t", "f5f", "wiggles",
                "drcoup", "dlflag", "dlperiod", "cops", "name",
                "spd", "stp"};
int numCmds;

char stringIP[16];
char charMAC[18];

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise

char runState = 0;

int moriShape[6] = {200, 200, 200, 0, 0, 0};

WiFiClient espClient;
PubSubClient client(espClient);

bool flag_udp = false;
bool flag_control = false;
bool verbose_flag = false;
bool led_cycle = false;

unsigned long lastMessage = millis();
unsigned long lastMacPub = millis();
unsigned long led_sel_time = millis();
unsigned long lastStabPub = millis();

byte wifi_edge = 255; //Stored as 0-2, but publishes as 1-3
byte stable_status = 0;

Led wifi_ind_led(4);
Led led_red(14);
Led led_green(12);
Led led_blue(5);

//--------------------------- Start ----------------------------------------//
void setup()
{
  numCmds = sizeof(cmdLine)/sizeof(cmdLine[0]);
  Serial.begin(115200);
  Serial.setRxBufferSize(1024);
  // Serial.setTxBufferSize(1024);
  delay(500);
  sprintf(clientName, "%08X", ESP.getChipId());
  sprintf(publishName, "%s/p", clientName);
  sprintf(recieveName, "%s/r", clientName);
  verbose_print("ChipID: ");
  verbose_println(clientName);
  verbose_println(publishName);
  verbose_println(recieveName);


  //--------------------------- Wifi ------------------------------------//
  startInternet(); 

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  startMQTT();


  //----------------------- OAT Handling--------------------------------------//
  handleOTA();
  
  //----------------------- UDP Handling--------------------------------------//
  // verbose_flag = false;
  // verbose_flag = true;
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
    case 4:   //Set to UDP
      startUDP();
      flag_udp = true;
      normalOp();
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
      break;
    case 7:   //Start Controller
      startController();
      flag_control = true;
      normalOp();
      publish("INFO: Controller Started");
      runState = 8;
      break;
    case 8: //controller (note publish is via MQTT if not case 5)
      readUDP();
      normalOp();
      break;
    case 9:
      flag_control = false;
      stopUDP();
      normalOp();
      publish("INFO: Controller Stopped");
      runState = 3;
      break;
    case 10:  //disable wireless
      wifi_ind_led.Off();
      stopMQTT();
      stopUDP();
      stopInternet();
      normalOp();
      runState = 11;
      break;
    case 11:  //run without wireless
      normalOp();
      break;
    case 12:  //restart wireless
      normalOp();
      startInternet();
      normalOp();
      startMQTT();
      normalOp();
      runState = 13;
    case 13:
      publish("WIFI: On");
      runState = 3;
      break;
  }

  client.loop();
}


void publish(char* buff)
{
  verbose_print("Sent ");
  verbose_print(buff);
  verbose_print(" via ");
  if ((runState == 10) || (runState == 11) || (runState == 12))
  {
    serial_write_to_hub(buff);
  }
  else if(runState != 5)
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
  readSerial();
  update_tx_buffer();
  long unsigned currentTime = millis();
  if(currentTime - lastStabPub > 2000)
  {
    // pubOn(); //Publish connection message
    publishStaticState();
    lastStabPub = millis();
  }
  
  if ((clientLetter == 255) && (millis() - lastMessage  > 2000))
  {
    serial_write_one(0b10100011);
    lastMessage = millis();
  }

  if (Serial.hasOverrun())
  {
    publish("ERR: SERIAL OVERRAN");
  }

  if(led_cycle)
  {
    static int led_sel = 0;
    switch(led_sel){
      case 0:
        led_red.On();
        led_green.On();
        led_blue.Off();
        led_sel = 1;
        break;
      case 1:
        led_red.Off();
        led_green.On();
        led_blue.On();
        led_sel = 2;
        break;
      case 2:
        led_red.On();
        led_green.Off();
        led_blue.On();
        led_sel = 0;
        break;
    }
    led_sel_time = millis();
  } else {
    led_red.Update();
    led_green.Update();
    led_blue.Update();
  }

}


void startInternet()
{
  WiFi.mode(WIFI_STA);
  delay(100);
  WiFi.begin(brn_ssid, brn_password);

  unsigned long connectionTime = millis();
  while (WiFi.status() != WL_CONNECTED)
  {
    if(millis() - connectionTime > 10000) //if longer than 10s to connect
      ESP.reset();
    if (runState == 12)
      normalOp();
    delay(500);
    purgeSerial();
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
}


void startMQTT()
{
  unsigned long connectionTime = millis();
  while (!client.connected()) {
    if(millis() - connectionTime > 10000) //if longer than 10s to connect
      ESP.reset();
    if (runState == 12)
      normalOp();    
    verbose_println("Connecting to MQTT...");
    wifi_ind_led.Toggle();
    if (client.connect(clientName))
    {
      verbose_println("connected");
    } else {
      purgeSerial();
      verbose_print("failed with state ");
      verbose_println(client.state());
      delay(1000);  // changed (could be too quick)
    }
  }
  serial_write_one(0b10100011);
  wifi_ind_led.On();

  //--------------------------- MQTT -----------------------------------------//

  client.publish(publishName, "INFO: Hello from ESP8266");
  client.subscribe(recieveName);
  client.subscribe("esp/rec");
}


void stopMQTT()
{
  client.disconnect();
}


void stopInternet()
{
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin();  
}

void enableWifi()
{
  if((runState == 10) || (runState == 11))
    runState = 12;
}