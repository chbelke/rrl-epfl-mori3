//----------------------- Recieved Message -----------------------------//
void pubVersion()
{
  char buff[40];
  sprintf(buff, "VER: %f %s", softwareVersion, clientName);
  publish(buff);
}

void pubMac(char* header)
{
  char buff[50];
  sprintf(buff, "%s%s", header, charMAC);
  publish(buff);
}


void pubIP()
{
  char buff[40] = "IP: ";
  strcat(buff, stringIP);
  publish(buff);
}