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

const char* esp_ssid = "helloThere";
const char* esp_password =  "generalKenobi1";
const char* brn_ssid = "rrl_wifi";
const char* brn_password =  "Bm41254126";
const char* mqttServer = "192.168.0.50";
const int mqttPort = 1883;

char* clientName;
char* publishName;
char* recieveName;
const float softwareVersion = 0.5;

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise
bool stopLoop = false;  //assumes we are running the proper code

unsigned long lastMessage = millis();

WiFiClient espClient;
PubSubClient client(espClient);

//--------------------------- Start ----------------------------------------//
void setup()
{
//  iota(ESP.getChipId(), clientName, 10);
  sprintf(clientName,"%d",ESP.getChipId());
  publishName = concatID(clientName,"/pub");
  recieveName = concatID(clientName,"/rec");


  Serial.begin(115200);
  Serial.println(clientName);
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

  //--------------------------- MQTT -----------------------------------------//

  client.publish(publishName, "Hello from ESP8266");
  client.publish(publishName, clientName);
  client.subscribe(recieveName);
  client.subscribe("esp/rec");

  WiFi.softAP(esp_ssid, esp_password);

  //----------------------- OAT Handling--------------------------------------//
  ArduinoOTA.setPort(8266);
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
  ArduinoOTA.begin();
}



//----------------------- Loooooooop--------------------------------------//
void loop()
{
  if (verCheck == true) //have checked version
  {
    if (verGood == true)
    {
      scanWifis();
      if (abs(lastMessage - millis()) > 10000)
      {
        client.publish(publishName, "ERR: SSID not found within 10s");
        lastMessage = millis();
      }
    }
    else //Version is old
    {
      if (stopLoop == false)
      {
        Serial.println("StoppingLoop");
        client.publish(publishName, "ERR: Pausing functionality until updated");
        client.loop();
        delay(250);
        client.disconnect();
        delay(250);
        WiFi.mode(WIFI_STA);
        delay(250);
        //        ArduinoOTA.begin();
        Serial.println("Ready");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        //        Serial.flush();
        delay(1000);
        stopLoop = true;
      }
      //      Serial.println("GODDAMN SNAKES ON THIS GODDAMN PLANE");
      ArduinoOTA.handle();
    }
  }
  else
  {
    pubVersion();
    delay(2000);
  }
  if (stopLoop == false)
    client.loop();
}




//----------------------- Recieved Message --------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

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
    Serial.println("version Checked");
  }

  Serial.println();
  Serial.println("-----------------------");

}



//----------------------- Recieved Message -----------------------------//
void pubVersion()
{
  char buff[100];
  String IPstring = String("VER: ") + String(softwareVersion)
                    + String(" ") + WiFi.localIP();
  IPstring.toCharArray(buff, 100);
  client.publish(publishName, buff);
}


//--------------------------- Scans Network -----------------------------//
void scanWifis()
{
  int n = WiFi.scanNetworks();
  while (WiFi.scanComplete() < 0)
  {
    delay(50);
  }
  for (int i = 0; i < n; i++)
  {
    char buff[100];
    String SSIDstring = WiFi.SSID(i);
    SSIDstring.toCharArray(buff, 100);
    if (strcmp(buff, "helloThere") == 0)
    {
      Serial.println("GENERAL KENOBI");

      uint8_t* bssid = WiFi.BSSID(i);
      char message[32];

      char* msgPtr = &message[0];
      snprintf(msgPtr, 6, "DST: ");
      msgPtr += 5;
      for (byte j = 0; j < 6; j++) {
        snprintf(msgPtr, 3, "%02x", bssid[j]);
        msgPtr += 2;
      }
      snprintf(msgPtr, 3, ", ");
      msgPtr += 2;

      char cstr[5];
      itoa(WiFi.RSSI(i), cstr, 10);

      snprintf(msgPtr, 6, cstr);

      Serial.print("Message: ");
      Serial.println(message);

      client.publish(publishName, message);

      lastMessage = millis();
    }
  }
}


char* concatID(char* header, char* footer) {
//  char* header = String(ESP.getChipId()).c_str();
  char buf[30];
  strcpy(buf, "esp");
  strcpy(buf, header);
  strcat(buf, footer);
  return buf;
}
