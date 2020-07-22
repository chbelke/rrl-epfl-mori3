
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const int PACKET_SIZE = 42; //how many bytes the buffers hold
char* ip_address = "RRLB201957";
int ip_port = 50001;

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

  // char msg[40];
  // int i;
  // for(i=0; udpInBuff[i] != 0 ;i++)
  // {
  //   msg[i] = (char)udpInBuff[i];
  // }
  // msg[i] = '\0';
  // verbose_print("MSG: ");
  // verbose_println(msg);

  // commands(msg);
}


void writeUDP(char* buff)
{
  UDP.beginPacket(ip_address, ip_port);
  UDP.write(buff);
  UDP.endPacket();
}
