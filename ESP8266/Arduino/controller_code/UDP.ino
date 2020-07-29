
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>


const int PACKET_SIZE = 42; //how many bytes the buffers hold
IPAddress ipBroadcast(255, 255, 255, 255);
unsigned int portBroadcast = 50002; 

WiFiUDP UDP;

byte udpInBuff[PACKET_SIZE];
byte udpOutBuff[PACKET_SIZE];


void startUDP()
{
  UDP.begin(portBroadcast);
  verbose_println("began UDP");
}


void stopUDP()
{
  UDP.stop();
  verbose_println("Stopped UDP");
}


void writeUDP(char* buff)
{
  UDP.beginPacket(ipBroadcast, portBroadcast);
  UDP.write(buff);
  UDP.endPacket();
}
