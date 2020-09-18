//----------------------- Recieved Message --------------------------//
void commands(byte* payload, unsigned int len)
{
  for(int i=0; i<len; i++)
  {
    verbose_print((char)payload[i]);
  }
  

  char topic[3];
  for(int i=0; i < 3; i++)
  {
    topic[i] = (char)payload[i];
  }
  
  int sw_case = 33;
  for(int i=0; i < sw_case; i++)
  {
    if (!memcmp(topic, cmdLine[i], 3)) //4 is number of bytes in memory to compare (3 chars + stop)
    {
      sw_case = i;
      break;
    }
      
  }

  switch(sw_case){
    case 0:   //mac
      pubMac("MAC: ");
      pubIP();
      break;
    
    case 1:    //vg
      runState = 3;
      pubMac("MAC: ");
      pubIP();
      purgeSerial();
      verbose_println("Version Excellent");
      break;

    case 2:   //vb
      runState = 1;
      verbose_println("Version Bad");
      break;

    case 3:   //stopudp
      runState = 3;
      verbose_println("UDP communication paused");
      break;

    case 4:   //hello
      publish("INFO: Hello!");
      break;

    case 5:   //g_shape (set shape)
      pubShape();
      break;

    case 6:   //udp
      verbose_println("Recieved UDP Command");
      runState = 4;
      break;

    case 7:   //noudp
      verbose_println("Stop UDP Operation");
      runState = 6;
      break;

    case 8:   //verb
      verbose_flag = true;
      verbose_println("Starting Verbose Operation");
      break;

    case 9:   //noverb
      verbose_flag = false;
      verbose_println("Stopping Verbose Operation");
      break;

    case 10:  //shape (set shape)
      relay(payload, len);
      break;

    case 11:  //cont
      runState = 7;
      verbose_println("Starting Controller");
      break;

    case 12:  //nocon      
      runState = 9;
      verbose_println("Starting Controller");
      break;

    case 13:  //rled
      led_red.Blink();
      break;

    case 14:  //gled
      led_green.Blink();
      break;

    case 15:  //bled
      led_blue.Blink();
      break;

    case 16:  //wedge
      setWifiEdge(payload, len);
      break;

    case 17:  //rshape
      requestShape();
      break;

    case 18:  //redge
      requestEdge();
      break;

    case 19:  //rang",
      requestAngle();
      break;

    case 20:  //rorient
      requestOrientation();
      break;

    case 21:  //rwedge
      requestWifiEdge();
      break;      

    case 22:  //rneigh
      requestNeighbour(payload, len);
      break;

    case 23:
      runState = 10;
      break;

    case 24:
      runState = 12;
      break;

    case 25:
      ESP.reset();
      break;

    case 26:  //Restart Pic
      serial_write_one(0b11011111);
      break;

    case 27: //png
      // publish("INFO: Ping received by ESP, command understood!"); // debugging
      echoPing(payload, len);
      break;

    case 28: //LED
      setPicLED(payload, len);
      break;      

    case 29: //Couplings
      openPicCouplings(payload, len);
      break;      

    case 30: //Edges
      setPicEdges(payload, len);
      break;      

    case 31: //Angles
      setPicAngles(payload, len);
      break;                                    

    case 32: //Party
      PARTYMODE();
      break;                                   

    default:
      publish("ERR: Command not understood");
  }

  verbose_println("-----------------------");
}


void setWifiEdge(byte* payload, unsigned int len)
{
  int byte_count = 0;

  byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len){
    publish("ERR: payload has no space");
    return;
  }
  byte_count++;

  wifi_edge = payload[byte_count]-48;   //-48 = ascii to int conversion

  char buff[50];
  sprintf(buff, "INFO: WiFi Edge set to %d", int(wifi_edge));
  publish(buff);

  serial_write_two(0b11011010, wifi_edge);
}


void requestShape()
{
  serial_write_one(0b11010100);
}


void requestEdge()
{
  serial_write_one(0b11010100);
}


void requestAngle()
{
  serial_write_one(0b11010101);
}


void requestOrientation()
{
  serial_write_one(0b11010110);
}


void requestWifiEdge()
{
  serial_write_one(0b11011000);
}


void requestNeighbour(byte* payload, unsigned int len)
{
  int byte_count = 0;

  byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len){
    publish("ERR: payload has no space");
    return;
  }
  byte_count++;

  byte neighbour = payload[byte_count]-48;   //-48 = ascii to int conversion

  // char buff[50];
  // sprintf(buff, "INFO: Req Neighbour %d", int(neighbour));
  // publish(buff);

  serial_write_two(0b11010111, neighbour);
}

void echoPing(byte* payload, unsigned int len)
{
  // Echo the message back to the external computer
  char buff[] = "PNG: 12345678901234567890123456789012";
  // char buff[] = {a, b, c, d, e, f, g};
  // const char *tmp = "PING:";
  // memcpy(&buff, &tmp, 5);
  char newPayload[len];
  for (int lv = 0; lv < len; lv++)
    {
      newPayload[lv] = payload[lv];
      //Serial.print((char)payload[lv]);
      //Serial.print(' ');
    }
  Serial.print("Length is: ");
  Serial.println(len);
  memcpy(&buff[5], &newPayload[4], len-4);
  publish(buff);
}


void setPicLED(byte* payload, unsigned int len)
{
  byte num_following = 0;
  byte alloc = 0b10000000;
  byte alloc_mask = 0b00000100;
  byte LED[3];
  
  if(!extractValuesForShape(payload, len, alloc_mask, &alloc, &num_following, LED)) {
    return;
  }

  Serial.write(0b11001101); //205
  Serial.write(alloc);
  for(byte i=0; i< num_following; i++)
  {
    Serial.write(LED[i]);
  }
  Serial.write(END_BYTE);
  return;
  // char buff[50];
  // sprintf(buff, "INFO: Set LEDS:");
  // byte j = 0;
  // for(byte i=0; i < 3; i++){
  //   if(alloc & (alloc_mask >> i)) {
  //     sprintf(buff, "%s %d", buff, LED[j]);  
  //     j++;
  //   } else {
  //     sprintf(buff, "%s -", buff);  
  //   }
  // }
  // sprintf(buff, "%s : %d %d %d, %d", buff, LED[0], LED[1], LED[2], num_following);
  // publish(buff);  
}


void openPicCouplings(byte* payload, unsigned int len)
{
  char tmp_payload[5];
  byte alloc = 0b10000000;
  
  byte byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len) {
    byte_count = len;
  }
  byte_count++;
  tmp_payload[0] = payload[byte_count];
  tmp_payload[1] = '\0';
  
  byte value = byte(atoi(tmp_payload))-1;

  if(value >3) {
    publish("ERR: Can\'t open coupling > 3");
  }

  alloc = alloc | (0b00100000 >> value);

  char buff[50];
  sprintf(buff, "INFO: Opening coupling %d", value+1);
  publish(buff);  

  Serial.write(0b11001101); //205
  Serial.write(alloc);
  Serial.write(END_BYTE);
  return;
}

void setPicEdges(byte* payload, unsigned int len)
{
  byte num_following = 0;
  byte alloc = 0b00000000;
  byte alloc_mask = 0b00100000;
  byte extensions[3];
  
  if(!extractValuesForShape(payload, len, alloc_mask, &alloc, &num_following, extensions)) {
    return;
  }

  Serial.write(0b11001101); //205
  Serial.write(alloc);
  for(byte i=0; i< num_following; i++)
  {
    Serial.write(extensions[i]);
  }
  Serial.write(END_BYTE);
  return;
}


void setPicAngles(byte* payload, unsigned int len)
{

  return;
}


bool extractValuesForShape(byte* payload, unsigned int len, byte alloc_mask, byte *alloc, byte *num_following, byte *values)
{
  // if((alloc == NULL) || (num_following == NULL) || (LED == NULL)) {
  //   return false; //some pointers pointing to NULL
  // }

  byte byte_count = 0;
  byte pre_count = 0;
  char tmp_payload[5];

  byte_count = detectSpaceChar(payload, byte_count, len);
  byte_count++;
  for(byte i=0; i<3; i++){ //For the 3 LEDs
    bool jumpFlag = false;
    pre_count = byte_count; //count between whitespace ("led 255" -> "255", len 3)
    
    byte_count = detectSpaceChar(payload, byte_count, len);
    if(byte_count > len) {
      byte_count = len;
    }
    if(byte_count > pre_count + 3) {
      publish("ERR: Shape value > 3 chars");
      return false;
    }

    byte j;
    for(j=0; j<byte_count-pre_count; j++) {
      if((payload[pre_count+j] < 48) || (payload[pre_count+j] > 57)) {  //not ascii number - byte(48) = "0"
        jumpFlag = true;
      }
      tmp_payload[j] = char(payload[pre_count+j]);
    }
    if(jumpFlag) {
      byte_count++;
      continue;
    }

    tmp_payload[j] = '\0';
    values[*num_following] = byte(atoi(tmp_payload));
    if(values[*num_following] > 255) {
      byte_count++;
      continue;
    }
    *num_following = *num_following + 1;

    *alloc = *alloc | (alloc_mask  >> i);
    byte_count++;
  }
  return true;
}


byte detectSpaceChar(byte* payload, int byte_count, unsigned int max_len)
{
  while(payload[byte_count] != 0b00100000)   //0b00100000 = whitespace
  {
    byte_count++;
    if(byte_count > max_len) {
      break;
    }
  }
  return byte_count;
}


void PARTYMODE()
{
  led_red.setBlinkFreq(random(10, 255));
  if(!led_red.getBlinkFlag())
    led_red.Blink();
  led_blue.setBlinkFreq(random(10, 255));
  if(!led_blue.getBlinkFlag())  
    led_blue.Blink();
  led_green.setBlinkFreq(random(10, 255));
  if(!led_green.getBlinkFlag())  
    led_green.Blink();
  serial_write_one(0b11010010);
}