//----------------------- Recieved Message -----------------------------//
void pubVersion()
{
  char buff[100];
  String IPstring = String("VER: ") + String(softwareVersion)
                    + String(" ") + String(clientName);
  IPstring.toCharArray(buff, 100);
  publish(buff);
}

void pubMac(String header)
{
  char buff[50];
  String SSIDstring = String(header) + WiFi.macAddress();
  SSIDstring.toCharArray(buff, 30);
  publish(buff);
}


void pubIP()
{
  char buff[40] = "IP: ";
  strcat(buff, stringIP);
  Serial.println(stringIP);
  Serial.println(buff);
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
