
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const int PACKET_SIZE = 255; //how many bytes the buffers hold
IPAddress ip_address;
int ip_port = 50001;
int controller_port = 50002;
bool ip_flag = false;

WiFiUDP UDP;

uint8_t udpInBuff[PACKET_SIZE];
uint8_t udpOutBuff[PACKET_SIZE];

unsigned long lastIpCall = millis();

void startUDP()
{
  UDP.begin(ip_port);
  verbose_println("began UDP");
}


void stopUDP()
{
  UDP.stop();
  verbose_println("Stopped UDP");
}


void readUDP()
{
  if(UDP.parsePacket() == 0) // If there's no response (yet)
    return;
  verbose_println("Received UDP");

  if(!ip_flag)
  {
    ip_address = UDP.remoteIP();
    ip_flag = true;
  }  
  
  int len = UDP.read(udpInBuff, PACKET_SIZE);
  if (len > 0)
  {
    udpInBuff[len] = '\0';
  }  

  commands(udpInBuff, len+1);
}


void getCompIP()
{
  if(ip_flag)
  {
    return;  
  }
  unsigned long currentMillis = millis();
  if (currentMillis - lastIpCall > 1000)
  {
    publish("UDP: Start");
    lastIpCall = currentMillis;
  } 
}

void startController()
{
  UDP.begin(controller_port);
  verbose_println("began Controller UDP");  
}


void stopController()
{
  UDP.stop();
  verbose_println("Stopped Conteroller UDP");
}



void writeUDP(char* buff)
{
  UDP.beginPacket(ip_address, ip_port);
  UDP.write(buff);
  UDP.endPacket();
}
