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
  
  int sw_case = numCmds;
  for(int i=0; i < numCmds; i++)
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
      disableWifi();
      break;

    case 24:
      runState = 12;
      break;

    case 25:
      ESP.reset();
      break;

    case 26:  //Restart Pic
      serial_write_one(RUN_PIC_RESTART);
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

    case 33: //Set Hub
      setAsHub();
      break;

    case 34: //Disable Hub
      stopHub();
      break;                

    case 35: //flag
      enableRotMotors();
      break;  

    case 36: //flag
      disbleRotMotors();
      break;

    case 37: //flag
      enableLinMotors();
      break;

    case 38: //flag
      disbleLinMotors();
      break;

    case 39: //flag
      enableFlag1();
      break;

    case 40: //flag
      disableFlag1();
      break;

    case 41: //flag
      enableFlag2();
      break;

    case 42: //flag
      disableFlag2();
      break;      

    case 43: //flag
      enableFlag3();
      break;

    case 44: //flag
      disableFlag3();
      break;      

    case 45: //flag
      enableFlag4();
      break;

    case 46: //flag
      disableFlag4();
      break;      

    case 47: //flag
      enableFlag5();
      break;

    case 48: //flag
      disableFlag5();
      break;      

    case 49:
      wiggleCoupling(payload, len);
      break;      

    case 50:
      driveAndCouple(payload, len);
      break;

    case 51:
      setDatalogFlag(payload, len);
      break;      

    case 52:
      setDatalogPeriod(payload, len);
      break;         

    case 53:
      stopParty();
      break;

    case 54:
      pubName();
      break;

    case 55:  //Angular speed
      setPicSpeed(payload, len);      
      break;

    case 56:
      sendEmergencyStop();  
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

  byte tmp_wifi_edge = payload[byte_count]-48-1;   //-48 = ascii to int conversion

  if (tmp_wifi_edge > 3) {
    publish("ERR: Wifi edge > 4");
    return;
  }
  if (tmp_wifi_edge == 3){
    setAsHub();
    return;
  }
  else if (wifi_edge == 3){ //tmp_hub <= 2
    stopHub();
  }

  wifi_edge = tmp_wifi_edge;

  char buff[50];
  sprintf(buff, "INFO: WiFi Edge set to %d", int(wifi_edge)+1);
  publish(buff);

  serial_write_two(SET_CMD_WEDGE, wifi_edge);
}


// Depricated
void requestShape()
{
  serial_write_one(REQ_CMD_SHAPE);
}


void requestEdge()
{
  serial_write_one(REQ_CMD_EDGS);
}


void requestAngle()
{
  serial_write_one(REQ_CMD_ANGS);
}


void requestOrientation()
{
  serial_write_one(REQ_CMD_ORNT);
}


void requestWifiEdge()
{
  serial_write_one(REQ_CMD_WEDGE);
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

  serial_write_two(REQ_CMD_NBR, neighbour);
}


void disableWifi()
{
  if(wifi_edge < 3)
  {
    publish("WIFI: Off");
    runState = 10;
  } else {
    publish("ERR: WiFi edge not set");
  }
  
}


void echoPing(byte* payload, unsigned int len)
{
  // Echo the message back to the external computer
  char buff[85];// = "PNG: 12345678901234567890123456789012";
  sprintf(buff, "PNG: ");
  for (int i = 4; i < len; i++)
  {
    if(((payload[i] == 0x00) && (i!=len-1)) || (payload[i] == 0xFF))
    {
      publish("PNG: Err");
      return;
    }
    sprintf(buff, "%s%c", buff, payload[i]);
  }
  publish(buff);
}


void openPicCouplings(byte* payload, unsigned int len)
{
  setCouplingShapeCommand(payload, len, SHAPE_CMD_COUP_MASK, SHAPE_CMD_COUP_ALLOC);
}


void driveAndCouple(byte *payload, unsigned int len)
{
  setCouplingShapeCommand(payload, len, SHAPE_CMD_DRVE_MASK, SHAPE_CMD_DRVE_ALLOC);
}


void setCouplingShapeCommand(byte *payload, unsigned int len, byte alloc_mask, byte alloc)
{
  char tmp_payload[5];
  
  byte byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len) {
    byte_count = len;
  }
  byte_count++;
  tmp_payload[0] = payload[byte_count];
  tmp_payload[1] = '\0';
  
  byte value = byte(atoi(tmp_payload))-1;

  if(value > 3) {
    publish("ERR: Can\'t open coupling > 3");
  }

  alloc = alloc | (alloc_mask & value);

  char buff[50];
  sprintf(buff, "INFO: Opening coupling %d", value+1);
  publish(buff);  

  write_to_buffer(SHAPE_COMMAND_ALLOC); //205
  write_to_buffer(alloc);
  write_to_buffer(END_BYTE);
  return;  
}


void setPicLED(byte *payload, unsigned int len)
{
  setShapeCommand(payload, len, SHAPE_CMD_LEDS_MASK, SHAPE_CMD_LEDS_ALLOC, false);
}


void setPicEdges(byte *payload, unsigned int len)
{
  setShapeCommand(payload, len, SHAPE_CMD_EDGS_MASK, SHAPE_CMD_EDGS_ALLOC, false);
}


void setPicSpeed(byte *payload, unsigned int len)
{
  setShapeCommand(payload, len, SHAPE_CMD_SPDS_MASK, SHAPE_CMD_SPDS_ALLOC, false);
}


void setPicAngles(byte *payload, unsigned int len)
{
  setShapeCommand(payload, len, SHAPE_CMD_ANGS_MASK, SHAPE_CMD_ANGS_ALLOC, true);
}


void setShapeCommand(byte *payload, unsigned int len, byte alloc_mask, 
          byte alloc, bool flg_two_bytes)
{
  byte num_following = 0;
  byte values[6];
  
  if(!extractValuesForShape(payload, len, alloc_mask, &alloc, &num_following, values, flg_two_bytes)) {
    return;
  }

  write_to_buffer(SHAPE_COMMAND_ALLOC); //205
  write_to_buffer(alloc);
  for(byte i=0; i< num_following; i++)
  {
    write_to_buffer(values[i]);
  }
  write_to_buffer(END_BYTE);
}


//Used in setPicEdges setPicLED, and setPicAngles
bool extractValuesForShape(byte* payload, unsigned int len, byte alloc_mask, 
          byte *alloc, byte *num_following, byte *values, bool flg_two_bytes)
{
  byte byte_count = 0;
  byte pre_count = 0;
  char tmp_payload[5];

  byte_count = detectSpaceChar(payload, byte_count, len);
  byte_count++;
  for(byte i=0; i<3; i++){ //For the 3 Edges
    bool jumpFlag = false;
    pre_count = byte_count; //count between whitespace ("led 255" -> "255", len 3)
    
    byte_count = detectSpaceChar(payload, byte_count, len);

    if(byte_count > len) { // no space at the end of the command (e.g., "led 10 10 10")
      byte_count = len;
    }

    if(byte_count >= len && i < 2) { // no space at the end of the command (e.g., "led 10 10 10")
      publish("ERR: No values following");
      return false;
    }

    if(flg_two_bytes){
      if(byte_count > pre_count + 4) {
        publish("ERR: Angle value > 4 chars");
        return false;
      }
    } else {
      if(byte_count > pre_count + 3) {
        publish("ERR: Shape value > 3 chars");
        return false;
      }
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
    if(flg_two_bytes){
      int tmp_val = atoi(tmp_payload);
      values[*num_following] = highByte(tmp_val);
      values[*num_following + 1] = lowByte(tmp_val);
      if(tmp_val > 3600) { //skips loop if value too high
        byte_count++;
        continue;
      }
      *num_following = *num_following + 2;
    } else {
      values[*num_following] = byte(atoi(tmp_payload));
      if(values[*num_following] > 255) {  
        byte_count++;
        continue;
      }
      *num_following = *num_following + 1;
    }

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
  serial_write_one(SET_PARTY_HYPE);
}

void stopParty()
{
  led_red.On();
  led_green.On();
  led_blue.On();
  serial_write_one(ITS_THE_POPO);
}

void setAsHub()
{
  wifi_edge = 3;
  serial_write_two(SET_CMD_WEDGE, wifi_edge);
  publish("HUB: On");
  if((runState != 3) && (runState != 5)){
    runState = 4;
    flag_control = false;
  }
}

void stopHub()
{
  publish("HUB: Off");
}


void enableRotMotors() {
  serial_write_flags(FLG_CMD_ENBL_ROTM);
  return;
}

void disbleRotMotors() {
  serial_write_flags(FLG_CMD_DSBL_ROTM);
  return;
}

void enableLinMotors() {
  serial_write_flags(FLG_CMD_ENBL_LINM);
  return;
}

void disbleLinMotors() {
  serial_write_flags(FLG_CMD_DSBL_LINM);
  return;
}

void enableFlag1() {
  serial_write_flags(FLG_CMD_ENBL_FLG1);
  return;
}

void disableFlag1() {
  serial_write_flags(FLG_CMD_DSBL_FLG1);
  return;
}

void enableFlag2() {
  serial_write_flags(FLG_CMD_ENBL_FLG2);
  return;
}

void disableFlag2() {
  serial_write_flags(FLG_CMD_DSBL_FLG2);
  return;
}

void enableFlag3() {
  serial_write_flags(FLG_CMD_ENBL_FLG3);
  return;
}

void disableFlag3() {
  serial_write_flags(FLG_CMD_DSBL_FLG3);
  return;
}

void enableFlag4() {
  serial_write_flags(FLG_CMD_ENBL_FLG4);
  return;
}

void disableFlag4() {
  serial_write_flags(FLG_CMD_DSBL_FLG4);
  return;
}

void enableFlag5() {
  serial_write_flags(FLG_CMD_ENBL_FLG5);
  return;
}

void disableFlag5() {
  serial_write_flags(FLG_CMD_DSBL_FLG5);
  return;
}

void wiggleCoupling(byte* payload, unsigned int len)
{
  int byte_count = 0;

  byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len){
    publish("ERR: payload has no space");
    return;
  }
  byte_count++;

  byte coupling = payload[byte_count]-48-1;   //-48 == ascii to int conversion

  if (coupling > 3) { //0-2 == couplings, 3 == all couplings
    publish("ERR: Wifi edge > 4");
    return;
  } 

  serial_write_two(RUN_COUP_WIGGLE, coupling);
}


void setDatalogFlag(byte* payload, unsigned int len) 
{
  bool successFlag = true;
  byte flag = extractFollowingNumber(payload, len, &successFlag);
  if(successFlag)
    serial_write_two(DLG_CMD_FLAG_SET, flag);
}


void setDatalogPeriod(byte* payload, unsigned int len) 
{
  bool successFlag = true;
  byte freq = extractFollowingNumber(payload, len, &successFlag);
  if(successFlag)
    serial_write_two(DLG_CMD_PERD_SET, freq);
}


byte extractFollowingNumber(byte* payload, unsigned int len, bool *successFlag)
{
  int byte_count = 0;
  char tmp_payload[5];

  byte_count = detectSpaceChar(payload, byte_count, len);
  if(byte_count > len){
    *successFlag = false;
    publish("ERR: payload has no space");
    return 0;
  }
  byte_count++;
  int pre_count = byte_count; //count between whitespace ("led 255" -> "255", len 3)
  byte_count = detectSpaceChar(payload, byte_count, len);

  if(byte_count > len) { // no space at the end of the command (e.g., "led 10 10 10")
    byte_count = len;
  }

  byte j;
  for(j=0; j<byte_count-pre_count; j++) {
    if((payload[pre_count+j] < 48) || (payload[pre_count+j] > 57)) {  //not ascii number - byte(48) = "0"
      *successFlag = false;
      publish("ERR: Number not ascii");
      return 0;
    }
    tmp_payload[j] = char(payload[pre_count+j]);
  }
  tmp_payload[j] = '\0';
  return (byte)atoi(tmp_payload);
}

void sendEmergencyStop() {
  serial_write_one(SET_EMG_STOP);
  publish("ERR: STOPPING");
  enableWifi();
}