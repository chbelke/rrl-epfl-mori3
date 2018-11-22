/**********************************************************************************
Written by Kevin Holdcroft (kevin.holdcroft@epfl.ch). All rights reserved RRL EPFL. 

Used to test OAT programming functionality. Used in conjunction with wifiHandler.py

Checks version, and if version is not up to date, enters loop where it waits for an
update.

Before flashing to ESP8266, please change clientName, publishName, recieveName, and 
softwareVersion to their appropriate values.

/**********************************************************************************/

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoOTA.h>

const char* esp_ssid = "helloThere";
const char* esp_password =  "generalKenobi1";
const char* brn_ssid = "rrl_wifi";
const char* brn_password =  "Bm41254126";
const char* mqttServer = "192.168.0.51";
const int mqttPort = 1883; 

const char* clientName  = "esp0";
const char* publishName = "esp0/pub";
const char* recieveName = "esp0/rec";
const float softwareVersion = 1.0;

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise

WiFiClient espClient;
PubSubClient client(espClient);


//----------------------- Wifi and MQTT--------------------------------------//
void setup() {  
  Serial.begin(115200);
// WiFi.setOutputPower(5.0);
  WiFi.begin(brn_ssid, brn_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect(clientName))
    {
      Serial.println("connected");
    } else {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.publish(publishName, "Hello from ESP8266");
  client.subscribe("esp/rec");

  //  WiFi.softAP(esp_ssid, esp_password);


  //----------------------- OAT Handling--------------------------------------//
  ArduinoOTA.onStart([]()
  {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS 
    // using SPIFFS.end()
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]()
  {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });

}

//----------------------- Loooooooop--------------------------------------//
void loop()
{
  if (verCheck == true)
  {
    if (verGood == false)
    {
      client.publish(publishName, "Out of Date");
      ArduinoOTA.handle();
    }
  }
  else
  {
    pubVersion();
    delay(2000);
  }
  client.loop();
}



//----------------------- Recieved Message --------------------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }

  if ('m' == (char)payload[0])
  {
    Serial.println("Made it!");
    char buff[100];
    String SSIDstring = String("MAC: ") + WiFi.macAddress();
    SSIDstring.toCharArray(buff, 100);
    client.publish(publishName, buff);
  }
  else if ('v' == char(payload[0]))
  {
    if ('g' == char(payload[1]))
      verGood = true;
    if ('b' == char(payload[1]))
      verGood = false;
    verCheck = true;
  }

  Serial.println();
  Serial.println("-----------------------");

}

//----------------------- Recieved Message --------------------------------------//
void pubVersion()
{
  char buff[100];
  String IPstring = String("VER: ") + String(softwareVersion) 
                                    + String(" ") + WiFi.localIP();
  IPstring.toCharArray(buff, 100);
  client.publish(publishName, buff);
}
