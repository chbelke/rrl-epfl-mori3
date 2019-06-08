
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
#include <WiFiUdp.h>

const char* brn_ssid = "rrl_wifi";
const char* brn_password =  "Bm41254126";
const char* mqttServer = "192.168.0.50";
const int mqttPort = 1883;

const char* esp_role = "mori";

char clientName[16];
char publishName[36];
char recieveName[36];

const float softwareVersion = 0.5;

String stringIP;

bool verCheck = false;  //assumes we don't know the version
bool verGood = true;   //assumes we are up to date unless otherwise

char runState = 0;

unsigned long lastMessage = millis();
unsigned long lastMacPub = millis();
int moriShape[6] = {200, 200, 200, 0, 0, 0};

WiFiClient espClient;
PubSubClient client(espClient);

WiFiUDP UDP;

const int PACKET_SIZE = 42; //how many bytes the buffers hold
byte udpInBuff[PACKET_SIZE];
byte udpOutBuff[PACKET_SIZE];
const int IPLEN = 5;
char ipList[IPLEN][15]; //holds 5 ips of a length of 15udp/w/p100/i192.168.0.65/
int portList[IPLEN];

unsigned long lastOutUDP = millis();

//--------------------------- Start ----------------------------------------//
void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.print("ChipID: ");
  Serial.println(ESP.getChipId());
  Serial.print("WifiID: ");
  Serial.println(WiFi.macAddress());
  sprintf(clientName, "%08X", ESP.getChipId());
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

  IPAddress IP = WiFi.localIP();
  stringIP = String(IP[0]) + String(".") +  String(IP[1]) + String(".") +  String(IP[2]) + String(".") +  String(IP[3]);

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

  //WiFi.softAP(esp_ssid, esp_password);

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
    sprintf(message, "Start updating: %s", type);
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
    if (progress % 10 < 1)
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


//----------------------- UDP Handling--------------------------------------//
  for(int i=0; i <= IPLEN; i++)
  {
    ipList[i][0] = '\0';
  }
  
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
    case 4:
      normalOp();
      readUDP();
      break;
  }
  client.loop();
}


void enableOTA()
{
  Serial.println("StoppingLoop");
  client.publish(publishName, "ERR: Pausing functionality until updated");
  client.loop();
  ArduinoOTA.begin();
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  delay(1000);
}


//----------------------- Recieved Message --------------------------//
void callback(char* topic, byte* payload, unsigned int len)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message length: ");
  Serial.println(len);
  Serial.print("Message: ");
  char payload2[len]; //Convert the received message to char
  for (int i = 0; i < len; i++) {
    payload2[i] = (char)payload[i];
    Serial.print(payload2[i]);
  }
  Serial.println();

  if (!memcmp(payload2,"mac",sizeof("mac")-1))
  {
    pubMac("MAC: ");
    pubRole();
    pubShape();
    pubIP();
  }
  else if (!memcmp(payload2,"vg",sizeof("vg")-1))
  {
    runState = 3;
    pubMac("MAC: ");
    pubRole();
    pubIP();
    Serial.println("version Excellent");
  }
  else if (!memcmp(payload2,"vb",sizeof("vb")-1))
  {
    runState = 1;
    Serial.println("version bad");
  }

  else if (!memcmp(payload2,"hello",sizeof("hello")-1)) //respond hello
  {
    client.publish(publishName, "INFO: Hello!");
  }

  else if (!memcmp(payload2,"hand",sizeof("hand")-1)){ //'hand' is for handshake
    handshake(payload2, len);
  }

  else if (!memcmp(payload2,"shape",sizeof("shape")-1)) //Send shape
  {
    pubShape();
  }

  else if (!memcmp(payload2,"hand",sizeof("hand")-1)){ //'hand' is for handshake
    handshake(payload2, len);
  }

  else if ('0' == payload2[0] && '1' == payload2[1]) //Start byte for MORI command
  {
    command(payload2);
  }

  else if (!memcmp(payload2,"udp",sizeof("udp")-1))
  {
    establishUDP(payload2, len);
  }

  Serial.println();
  Serial.println("-----------------------");

}

void pubMac(String header)
{
  char buff[50];
  String SSIDstring = String(header) + WiFi.macAddress();
  SSIDstring.toCharArray(buff, 30);
  client.publish(publishName, buff);
}

void pubRole()
{
  char buff[40];
  String roleString = String("ROLE: ") + WiFi.macAddress() + String(" ") + String(esp_role);
  roleString.toCharArray(buff, 30);
  client.publish(publishName, buff);
}

void pubIP()
{
  char buff[40];
  String IPString = String("IP: ") + stringIP;
  IPString.toCharArray(buff, 30);
  client.publish(publishName, buff);
}

void pubShape()
{
  char buff[130];
  String shapeMsg = String("SHAPE: ") + WiFi.macAddress();

  for (int i = 0 ; i < 6 ; i++) {
    shapeMsg = String(shapeMsg) + String(" ") + String(moriShape[i]);
  }

  shapeMsg.toCharArray(buff, 130);
  Serial.println(buff);
  client.publish(publishName, buff);
}

void communication(char* payload, int len){
  char text[32];
  char* receiver = "esp/00000000/rec";

  //Define the positions and lengths of the different sections of the received message
  int receiverIDLength = 8;
  int receiverPublishIDStart = 4;
  int receiverMessageIDStart = 3;
  int textStart = 11;

  for (int i = 0 ; i < receiverIDLength ; i++) {
    receiver[i + receiverPublishIDStart] = payload[i + receiverMessageIDStart];
  }
  for (int i = textStart ; i < len ; i++) {
    text[i - textStart] = payload[i];
  }

  text[len-textStart] = '\0';

  Serial.println(text);
  Serial.println(receiver);
  client.publish(receiver, text);
}

void handshake(char* payload, int len){
  bool continueHandshake = false;
  bool handshakeCorrect = false;

  //Define the positions and lengths of the different sections of the received message
  int role = 4;
  int espIDStart = 5;
  int espIDLength = 8;
  int receiverPublishIDStart = 4; 
  
  char* followerID = "00000000";
  char* receiver = "esp/00000000/rec";
  char* message = "hand_00000000/00000000\0"; 
  //"_" will be the role of the receiver
  //==> The leader receives from the boss (computer): hal'otherID'
  //==> It then sends sends to follower: hal'otherID'/'selfID'
  //==> The follower receives the message, checks it's ID and sends (if ID correct): haf'selfID'/'otherID'
  //==> The leader receives the message, check its ID and the handshake is established (if ID correct)
  
  if (payload[role] == 'l'){ // 'l' is for leader (l is a letter)
    message[role] = 'f';
    if (len == espIDStart + espIDLength){ //Message received from boss (computer)
       continueHandshake = true;
       for (int i = 0 ; i < espIDLength ; i++){
          //Create the message to be sent to follower
          receiver[i + receiverPublishIDStart] = payload[i + espIDStart];
          message[i + espIDStart] = payload[i + espIDStart];
          message[i + espIDStart + espIDLength + 1] = recieveName[i + receiverPublishIDStart];
       }
    }
    else if (len == espIDStart + 2*espIDLength + 1){ //Message received from follower
      handshakeCorrect = true;
      for (int i = 0 ; i < espIDLength ; i++){
        //Check if own and received IDs are the same
        followerID[i] = payload[i + espIDStart];
        if (payload[i + espIDStart + espIDLength + 1] != recieveName[i + receiverPublishIDStart]){
          Serial.println("Leader ID error!");
          handshakeCorrect = false;
        }
      }
    }
  }
  else if (payload[role] == 'f'){ //'f' is for follower
     message[role] = 'l';
     continueHandshake = true;
     for (int i = 0 ; i < espIDLength ; i++){
        //Create the message to be sent to leader
        receiver[i + receiverPublishIDStart] = payload[i + espIDStart + espIDLength + 1];
        message[i + espIDStart] = payload[i + espIDStart];
        message[i + espIDStart + espIDLength + 1] = payload[i + espIDStart + espIDLength + 1];

        //Check if own and received IDs are the same
        if (message[i + espIDStart] != recieveName[i + receiverPublishIDStart]){
          Serial.println("Follower ID error!");
          continueHandshake = false;
        }
     }
     Serial.println(message);
  }
  else{
    Serial.println("Role definition error!");
  }
  if (continueHandshake){
    client.publish(receiver, message);
  }
  if (handshakeCorrect){
    Serial.println("HANDSHAKE ESTABLISHED!");
    char buff[20];
    String handshakeString = String("CONTROL: ") + followerID;
    handshakeString.toCharArray(buff, 20);
    //Serial.println(buff);
    client.publish(publishName, buff);
  }
}

void command(char* payload){
  //Typical message: "01 00100100 0650-030 150"
  //01 = start byte
  //00100100 = allocation byte (shape change for  extension A and angle alpha)
  //650-30 = data bytes (axtension A = 650 and angle alpha = -30)
  //150 = end byte (it is a number that is out of range for extension and angle)
  //Define the positions of the different sections of the message:
  int commandModeStart = 3;
  int commandModeEnd = commandModeStart + 1;
  int maxNbrCommands = 6;
  int allocationStart = commandModeEnd + 1;
  int allocationEnd = allocationStart + maxNbrCommands;
  int commandsStart = allocationEnd + 1;
  int commandSize = 4; //The shape commands are always of size 4
  char newValue[commandSize];
  int nbrNewValues = 0;

  if (payload[commandModeStart - 1] != ' ') {
    Serial.print("Command start bit error!");
  }
  if (payload[commandModeStart] == '0' && payload[commandModeEnd] == '0') //Shape command
  {
    for (int i = allocationStart ; i < allocationEnd ; i++) { //Go through the message to execute the command
      //Serial.println((char)payload[i]);
      if (payload[i] == '1') { //Extension or angle needs to be modified
        //Serial.print("Shape change");
        for (int ii = 0 ; ii < commandSize ; ii++) { //Save the desired shape
          newValue[ii] = payload[commandsStart + ii + (nbrNewValues * commandSize)];
          //Serial.println(payload[commandsStart + ii + (nbrNewValues * commandSize)]);
        }
        nbrNewValues += 1;
        moriShape[i - allocationStart] = atoi(newValue); //Convert char to int
        Serial.println(atoi(newValue));
      }
    }
    for (int ii = 0 ; ii < commandSize ; ii++) { //Save end byte
      newValue[ii] = payload[commandsStart + ii + (nbrNewValues * commandSize)];
    }
    //Serial.print(atoi(newValue));
    if (atoi(newValue) != 150) { //Check end byte
      Serial.println("Command end bit error!");
    }
    pubShape();
  }
  else if (payload[commandModeStart] == '0' && payload[commandModeEnd] == '1') //Drive command
  {

  }
  else if (payload[commandModeStart] == '1' && payload[commandModeEnd] == '0') //Coupling/LED command
  {

  }
}

void establishUDP(char* payld, unsigned int len)
{
  bool readFlag = false;
  bool writeFlag = false;
  bool msgFlag = false;
  bool portFlag = false;
  bool serialFlag = false;
  int port=0;
  char ip[15];
  char msg[40];
  Serial.println("Recieved UDP Command");
  for (int i = 3; i < len; i++)   //start after udp
  {
    if(payld[i] == 'r')
      readFlag = true;
    if(payld[i] == 'w')
      writeFlag = true;
    if(payld[i] == 's')
      serialFlag = true;
    if(payld[i] == 'p')
    {
      for(int l=1; isDigit(payld[i+l]); l++)
      {
        port = port *10 + (payld[i+l]-'0');
        if(i+l+1 > len)
          break;
      }
      portFlag = true;
    }
    if(payld[i] == 'i')
    {
      int l;
      for(l=1; isDigit(payld[i+l])|| payld[i+l] == '.'; l++)
      {
        ip[l-1] = payld[i+l];
        if(i+l+1 > len)
          break;
      }
      for(l; l <= 16; l++)
      {
        ip[l-1] = '\0';
      }
    }
    if(payld[i] == 'm')
    {
      int m;
      for(m = 0 ; i+m+1 < len ; m++)
      {
        msg[m] = payld[i+m+1];
        //Serial.println(msg[m]);
      }
      msg[m] = '\0';
      msgFlag = true;
      break;
    }
  }
  if(readFlag)
  {
    if(port!=0)
    {
      UDP.begin(port);
      Serial.print("Listening to UDP on Port: ");
      Serial.println(UDP.localPort());
      runState = 4;
    }
  }
  if(writeFlag)
  {
    Serial.println("MORI ASKED TO SEND UDP MSG????");
    if(!memcmp(ip,"192",sizeof("192")-1) && portFlag)
    {
      if(!msgFlag)
      {
        Serial.print("NO MSG!");
        for(int i=0; i <= IPLEN; i++)
        {
          if(ipList[i][0] == '\0')
          {
            //ipList[i] = strcpy(ip);
            strcpy(ipList[i], ip);
            portList[i] = port;
            break;
          }
        }
      }
      else
      {
        memset(udpOutBuff, 0, PACKET_SIZE);  // set all bytes in the buffer to 0
        // Initialize values needed to form NTP request
        for(int i = 0; msg[i] != '\0' ; i++)
        {
          udpOutBuff[i] = (byte)msg[i];
        }
        Serial.println("Output buffer = ");
        for(int i = 0 ; i < PACKET_SIZE ; i++){
           Serial.println(udpOutBuff[i]);
        }
        
        // udpOutBuff[0] = 0b11100011;   // LI, Version, Mode
        // send a packet requesting a timestamp:
        UDP.beginPacket(ip, port); // NTP requests are to port 123
        UDP.write(udpOutBuff, PACKET_SIZE);
        UDP.endPacket();
        Serial.print("Sent: ");
        Serial.print(msg);
        Serial.print(" to: ");
        Serial.print(ip);
        Serial.print(":");
        Serial.print(port);
      } //else (!msg)       
    } //IP
  } //writeFlag
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

void readUDP()
{
  if (UDP.parsePacket() == 0) // If there's no response (yet)
  {
    return;
  }
  
  UDP.read(udpInBuff, PACKET_SIZE);
  Serial.println("Received message: ");
  for(int i=0; i< sizeof(udpInBuff);i++)
  {
    Serial.print(udpInBuff[i]);
  }
  Serial.println();

  callback("UDP", udpInBuff, sizeof(udpInBuff));

  char msg[40];
  int i;
  for(i=0; udpInBuff[i] != 0 ;i++)
  {
    msg[i] = (char)udpInBuff[i];
  }
  msg[i] = '\0';
  Serial.print("MSG: ");
  for(int i=0 ; msg[i] != '\0' ; i++){
    Serial.print(msg[i]);
  }
  Serial.println();
}

void writeUDPSerial()
{
  if (Serial.available() > 0) 
  {
    uint8_t bytRead1 = Serial.read();
    udpOutBuff[0] = bytRead1;
    for(int i=0; i <= IPLEN; i++)
    {
      if(ipList[i][0] == '\0')
      {
        break;
      }
      UDP.beginPacket(ipList[i], portList[i]);
      UDP.write(udpOutBuff, 1);
      UDP.endPacket();
    }
    Serial.print("Sent: ");
    Serial.print(udpOutBuff[0]);
    client.publish(publishName, "Sent Serial!");
  }
}


void normalOp()
{
//  scanWifis();
  long unsigned currentTime = millis();
  if(currentTime - lastMacPub > 2000)
  {
    pubMac("ON: "); //Publish connexion message
    //pubIP();
    lastMacPub = millis();
  }
  
  if (abs(lastMessage - millis()) > 10000)
  {
    client.publish(publishName, "ERR: SSID not found within 10s");
    lastMessage = millis();
  }
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

char* concatID(char* header, char* footer) {
  //  char* header = String(ESP.getChipId()).c_str();
  char buf[30];
  strcpy(buf, "esp/");
  strcpy(buf, header);
  strcat(buf, footer);
  return buf;
}
