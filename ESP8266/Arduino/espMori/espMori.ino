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

//char* clientName = (char*)malloc(8*sizeof(char));
//char* publishName = (char*)malloc(16*sizeof(char));
//char* recieveName = (char*)malloc(16*sizeof(char));

char clientName[16];
char publishName[36];
char recieveName[36];

const float softwareVersion = 0.5;

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise
bool stopLoop = false;  //assumes we are running the proper code

unsigned long lastMessage = millis();
int moriShape[6] = {200, 200, 200, 0, 0, 0};

WiFiClient espClient;
PubSubClient client(espClient);

//--------------------------- Start ----------------------------------------//
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.print("ChipID: ");
  Serial.println(ESP.getChipId());
  Serial.print("WifiID: ");
  Serial.println(WiFi.macAddress());
  sprintf(clientName,"%08X",ESP.getChipId());
  sprintf(publishName, "esp/%s/pub", clientName);
  sprintf(recieveName, "esp/%s/rec", clientName);
  Serial.println(clientName);
  Serial.println(publishName);
  Serial.println(recieveName);
  

  WiFi.mode(WIFI_STA);

  delay(500);
  
//  Serial.println(clientName);
  // WiFi.setOutputPower(5.0);
  WiFi.begin(brn_ssid, brn_password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

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
    char* type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
    // using SPIFFS.end()
    char message[30];
    sprintf(message,"Start updating: %s",type);
    client.publish(publishName, message);
    Serial.println(message);
    client.loop();
  });

  ArduinoOTA.onEnd([]()
  {
    client.publish(publishName, "Finished Updating");
    Serial.println("\nEnd");
    client.loop();
    delay(500);
    ESP.restart();
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
  {
    Serial.printf("Progress: %i/100", (progress / (total / 100)));
    if(progress % 10 < 1)
    {
      char message[30];
      sprintf(message, "Progress: %i/100", (progress / (total / 100))); 
      client.publish(publishName, message);
      client.loop();
    }
      
  });

  ArduinoOTA.onError([](ota_error_t error)
  {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      client.publish(publishName, "ERR: Auth Failed");
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      client.publish(publishName, "ERR: Begin Failed");
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      client.publish(publishName, "ERR: Connect Failed");
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      client.publish(publishName, "ERR: Finished Updating");
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      client.publish(publishName, "ERR: Receive Failed");
      Serial.println("End Failed");
    }
    client.loop();
  });
  // ArduinoOTA.begin();
}



//----------------------- Loooooooop--------------------------------------//
void loop()
{
  if (verCheck == true) //have checked version
  {
    if (verGood == true)
    {
      scanWifis();
      pubMac("ON: "); //Publish connexion message
      //pubShape();
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
        WiFi.mode(WIFI_STA);
        ArduinoOTA.begin();
        Serial.println("Ready");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());
        delay(1000);
        stopLoop = true;
      }
      //      Serial.println("GODDAMN SNAKES ON THIS GODDAMN PLANE");
    }
  }
  else
  {
    pubVersion();
    delay(500);
  }
  if (stopLoop == true)
    ArduinoOTA.handle();
  client.loop();
}




//----------------------- Recieved Message --------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  int byteSize = 8;
  char text[6];
  char* receiver = "esp/00000000/rec";
  
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.println(len);
  Serial.print("Message:");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();

  if ('m' == (char)payload[0])
  {
    pubMac("MAC: ");
    pubShape();
  }
  else if ('v' == char(payload[0]))
  {
    if ('g' == char(payload[1]))
      {
        verGood = true;
        pubMac("MAC: ");
      }
    if ('b' == char(payload[1]))
      verGood = false;
    verCheck = true;
    Serial.println("version Checked");
  }
  else if ('h' == (char)payload[0])
  {
    client.publish(publishName, "INFO: Hello!");
  }
  else if ('c' == (char)payload[0])
  { 
    if(len > 10){
      for (int i = 0 ; i < 8 ; i++){
        receiver[i + 4] = (char)payload[i + 1];  
      }
      for (int i = 9 ; i < len ; i++){
        text[i - 9] = (char)payload[i];
      }
      
      Serial.println(text);
      Serial.println(receiver);
      client.publish(receiver, text);
    }
    else{
      for (int i = 0 ; i < len ; i++){
        Serial.println((char)payload[i]);
      }
    }
  }
  else if ('0' == (char)payload[0] && '1' == (char)payload[1]) //Start byte for MORI command
  {
    int commandModeStart = 3;
    int commandModeEnd = commandModeStart + 1;
    int maxNbrCommands = 6;
    int allocationStart = commandModeEnd + 1;
    int allocationEnd = allocationStart + maxNbrCommands;
    int commandsStart = allocationEnd + 1;
    int commandSize = 3;

    char newValue[3];
    int nbrNewValues = 0;

    if((char)payload[commandModeStart-1] != ' '){
      Serial.print("Command start bit error!");
    }
    
    if ((char)payload[commandModeStart] == '0' && (char)payload[commandModeEnd] == '0') //Shape command
    {
      Serial.println("NEW SHAPE");
      for (int i = allocationStart ; i < allocationEnd ; i++){ //Go through the message to execute the command
        //Serial.println((char)payload[i]);
        if ((char)payload[i] == '1'){ //Extension or angle needs to be modified
          Serial.print("Shape change");
          for (int ii = 0 ; ii < commandSize ; ii++){
            newValue[ii] = (char)payload[commandsStart + ii + (nbrNewValues * commandSize)];
            Serial.println((char)payload[commandsStart + ii + (nbrNewValues * commandSize)]);
          }
          nbrNewValues += 1;
          moriShape[i-allocationStart] = atoi(newValue);
          Serial.print(atoi(newValue));
        }
      }
      pubShape();
    }
    /*
    if ((char)payload[i] == '1'){
          newValue[0] = (char)payload[18+(valuesSeparation*nbrNewValues)];
          newValue[1] = (char)payload[19+(valuesSeparation*nbrNewValues)];
          nbrNewValues += 1;   
          moriShape[i-11] = atoi(newValue);   
          //Serial.println((char)payload[18+(valuesSeparation*nbrNewValues)]);
          //Serial.println((char)payload[19+(valuesSeparation*nbrNewValues)]);
          //Serial.println(newValue);
          //Serial.println(atoi(newValue));

    */
  }
  
  Serial.println();
  Serial.println("-----------------------");

}


void pubMac(String header)
{
    char buff[30];
    String SSIDstring = String(header) + WiFi.macAddress();
    SSIDstring.toCharArray(buff, 30);
    client.publish(publishName, buff);
}

void pubShape()
{
  char buff[130];
  String shapeMsg = String("SHAPE: ") + WiFi.macAddress();
  
  for (int i = 0 ; i < 6 ; i++){
    shapeMsg = String(shapeMsg) + String(" ") + String(moriShape[i]);
  }
  
  shapeMsg.toCharArray(buff, 130);
  Serial.println(buff);
  client.publish(publishName, buff);
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
      //Serial.println("GENERAL KENOBI");

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

      //Serial.print("Message: ");
      //Serial.println(message);

      client.publish(publishName, message);

      lastMessage = millis();
    }
  }
}


void getIP()
{
  
}


char* concatID(char* header, char* footer) {
//  char* header = String(ESP.getChipId()).c_str();
  char buf[30];
  strcpy(buf, "esp/");
  strcpy(buf, header);
  strcat(buf, footer);
  return buf;
}
