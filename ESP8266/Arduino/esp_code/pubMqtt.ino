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

void pubShape()
{
  char buff[130];
  String shapeMsg = String("SHAPE: ") + WiFi.macAddress();

  for (int i = 0 ; i < 6 ; i++) {
    shapeMsg = String(shapeMsg) + String(" ") + String(moriShape[i]);
  }

  shapeMsg.toCharArray(buff, 130);
  verbose_println(buff);
  publish(buff);
}
