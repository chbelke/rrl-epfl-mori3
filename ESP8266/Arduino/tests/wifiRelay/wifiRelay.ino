#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* esp_ssid = "helloThere";
const char* esp_password =  "generalKenobi1";
const char* brn_ssid = "rrl_wifi";
const char* brn_password =  "Bm41254126";
const char* mqttServer = "192.168.0.51";
const int mqttPort = 1883;

const char* clientName  = "esp2";
const char* publishName = "esp2/pub";
const char* recieveName = "esp2/rec";

WiFiClient espClient;
PubSubClient client(espClient);
 
void setup() {
  Serial.begin(115200);
  WiFi.setOutputPower(5.0);
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

  WiFi.softAP(esp_ssid, esp_password);
}
 
void callback(char* topic, byte* payload, unsigned int len) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);  
  Serial.print("Message:");
  for (int i = 0; i < len; i++) {
    Serial.print((char)payload[i]);
  }

  if('m' == (char)payload[0])
  {
    Serial.println("Made it!");
    char buff[100];
    String SSIDstring = String("MAC: ")+ WiFi.macAddress();
    SSIDstring.toCharArray(buff,100);
    client.publish(publishName, buff);
  }
 
  Serial.println();
  Serial.println("-----------------------");
 
}

void loop() {
  scanWifis();
//  delay(2000);
  client.loop();
}

void scanWifis()
{
  int n = WiFi.scanNetworks();
  while(WiFi.scanComplete()<0)
  {
    delay(50);
  }
  for (int i = 0; i < n; i++)
  {
    char buff[100];
    String SSIDstring = WiFi.SSID(i);
    SSIDstring.toCharArray(buff,100);
    if(strcmp(buff,"helloThere")==0)
    {
      Serial.println("GENERAL KENOBI");

      uint8_t* bssid = WiFi.BSSID(i);
      char message[32];
      
      char* msgPtr = &message[0];
      snprintf(msgPtr,6,"DST: ");
      msgPtr += 5;
      for (byte j = 0; j < 6; j++){
        snprintf(msgPtr,3,"%02x",bssid[j]); 
        msgPtr += 2;
      }
      snprintf(msgPtr,3,", ");
      msgPtr += 2;

      char cstr[5];
      itoa(WiFi.RSSI(i), cstr, 10);

      snprintf(msgPtr,6,cstr);

      Serial.print("Message: ");
      Serial.println(message);      

      client.publish(publishName, message);
    }
  }
}

