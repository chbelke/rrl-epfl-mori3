
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const int PACKET_SIZE = 64; //how many bytes the buffers hold
IPAddress ip_address;
int ip_port = 50001;
int controller_port = 50002;
bool ip_flag = false;

WiFiUDP UDP;

byte udpInBuff[PACKET_SIZE];
byte udpOutBuff[PACKET_SIZE];

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
  
  UDP.read(udpInBuff, PACKET_SIZE);
  unsigned int len = int(udpInBuff[0]);
  byte udp_packet[len];
  for(int i=0; i< len; i++)
  {
    udp_packet[i] = udpInBuff[i+1];
  }

  commands(udp_packet, len);
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
