
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const int PACKET_SIZE = 64; //how many bytes the buffers hold
char* ip_address = "RRLB201957";
int ip_port = 50001;
int controller_port = 50002;

WiFiUDP UDP;

byte udpInBuff[PACKET_SIZE];
byte udpOutBuff[PACKET_SIZE];


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
  
  UDP.read(udpInBuff, PACKET_SIZE);
  unsigned int len = int(udpInBuff[0]);
  byte udp_packet[len];
  for(int i=0; i< len; i++)
  {
    udp_packet[i] = udpInBuff[i+1];
  }

  commands(udp_packet, len);
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
