#ifndef UDP_Connection_h
#define UDP_Connection_h

#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const int PACKET_SIZE = 42; //how many bytes the buffers hold
WiFiUDP UDP;

class UDP_Connection{
private:
  byte udpInBuff[PACKET_SIZE];
  byte udpOutBuff[PACKET_SIZE];
  char* ip_address;
  int ip_port_in;
  int ip_port_out;  
 
public:
  UDP_Connection(char* ip_address, int ip_port);
  void readUDP();
  void writeUDP(char* buff);
  void startUDP();
  void stopUDP();
};


UDP_Connection::UDP_Connection(char* ip_address, int ip_port){
  this->ip_address = ip_address;
  this->ip_port_out = ip_port;
};


void UDP_Connection::startUDP()
{
  UDP.begin(this->ip_port_out);
  Serial.println("began UDP");
}


void UDP_Connection::stopUDP()
{
  UDP.stop();
  Serial.println("Stopped UDP");
}


void UDP_Connection::readUDP()
{
  if(UDP.parsePacket() == 0) // If there's no response (yet)
    return;
  Serial.println("Received UDP");
  
  UDP.read(udpInBuff, PACKET_SIZE);
  Serial.println("Received message: ");
  for(int i=0; i< sizeof(udpInBuff);i++)
  {
    Serial.print(udpInBuff[i]);
  }
  Serial.println();

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
  // recieveCommand(msg);
}


void UDP_Connection::writeUDP(char* buff)
{
  UDP.beginPacket(this->ip_address, this->ip_port_out);
  UDP.write(buff);
  UDP.endPacket();
}

#endif
