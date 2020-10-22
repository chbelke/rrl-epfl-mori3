void readSerial()
{
  
  if(Serial.available())
  {
    static int serial_case = 0;
    static bool alloc = true;    
    
    byte c = Serial.read();

    if (alloc)
    {
      if(c == END_BYTE)
      {
        return;
      }
      serial_case = ((c >> 5) & 0b00000111);
      alloc = false;
    }        
  

    switch(serial_case){
      case 0:
        if(readVerbose(c)) alloc = true;
        break;

      case 1:
        if(readInterpret(c)) alloc = true;
        break;        

      case 2:
        if(setLEDs(c)) alloc = true;
        break;
      
      case 4:
        if(stateInfo(c)) alloc = true;
        break;

      case 6:
        if(dataLog(c)) alloc = true;
        break;
      
      case 7:
        if(relayToComputer(c)) alloc = true;
        break;   

      default:
        if(c == byte(END_BYTE)) alloc = true;
    }
  }
}


bool readVerbose(byte c)
{
  static char serial_packet[100];
  static byte serial_buf_loc = 0;
  static byte serial_len;
  static byte readCase = 0;

  // char nuff[50];
  // sprintf(nuff, "INFO: char = %d", int(c));
  // publish(nuff);
  if(serial_buf_loc  > 16)
  {
    purgeSerial();
    serial_buf_loc = 0;
    readCase = 0;
    return true;
  }
  
  switch(readCase){
    case 0: 
      memset(serial_packet, 0, sizeof(serial_packet));
      sprintf(serial_packet, "VBS: ");
      readCase = 1;
      break;
    case 1:
      serial_len = c-1;
      readCase = 2;
      break;

    case 2:
      // serial_packet[serial_buf_loc+5] = c;
      if ((c == char(END_BYTE)) && (serial_buf_loc-1 == serial_len))
      {
        publish(serial_packet); 
        serial_buf_loc = 0;
        readCase = 0;
        return true;
      }
      // serial_packet[serial_buf_loc+5] = c;
      sprintf(serial_packet, "%s %x", serial_packet, c); 
      if (serial_buf_loc > serial_len)
      {
        if(serialErrorHandle(c))
        {
          publish("ERR: buf_loc > serial_len, vbs");
    
          char buff[50];
          sprintf(buff, "INFO: buf = %d, ser = %d", int(serial_buf_loc), int(serial_len));
          publish(buff);

          char duff[50];
          sprintf(duff, "INFO: packet = %s", serial_packet);  
          publish(duff);
          serial_buf_loc = 0;
          readCase = 0;
          return true;
        }
      }
      serial_buf_loc++;
      break;
  }
  return false;
}


bool setLEDs(byte d)
{
  static byte readCase = 0;
  static byte LED = 0;
  static byte byteCount = 0;
  static byte serial_len;
  static byte packet;

  byte c = d;


  if(LED == 0)
  {
    LED = ((c & 0b00011000) >> 3);
    readCase = (c & 0b0000111);
    if (readCase > 3)
    {
      serial_len = 2;
    } else {
      serial_len = 1;
    }
  }


  switch(readCase){
    case 0:
      // publish("INFO: LED Toggle");
      if ((c == char(END_BYTE)) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.Toggle();
        } else if (LED == 2)
        {
          led_green.Toggle();
        } else {
          led_blue.Toggle();
        }
        byteCount = 0;
        readCase = 0;
        LED = 0;
        return true;        
      }
      break;

    case 1:
      // publish("INFO: LED Off");
      if ((c == 0b0001110) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.On(); //backwards, I know
        } else if (LED == 2)
        {
          led_green.On();
        } else {
          led_blue.On();
        }
        byteCount = 0;
        readCase = 0;
        LED = 0;
        return true;        
      }
      break;    
    
    case 2:
      // publish("INFO: LED On");
      if ((c == char(END_BYTE)) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.Off();  //not a type - just bad code
        } else if (LED == 2)
        {
          led_green.Off();
        } else {
          led_blue.Off();
        }
        byteCount = 0;
        readCase = 0;
        LED = 0;
        return true;        
      }
      break; 

    case 3:
      if ((c == char(END_BYTE)) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.Blink();
        } else if (LED == 2)
        {
          led_green.Blink();
        } else {
          led_blue.Blink();
        }
        byteCount = 0;
        readCase = 0;
        LED = 0;
        return true;        
      }      
    
    case 4:
      if ((c == char(END_BYTE)) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          if(!led_red.getBlinkFlag())
            led_red.Blink();
          led_red.setBlinkFreq(packet);
        } else if (LED == 2) {
          if(!led_green.getBlinkFlag())
            led_green.Blink();
          led_green.setBlinkFreq(packet);
        } else {
          if(!led_blue.getBlinkFlag())
            led_blue.Blink();
          led_blue.setBlinkFreq(packet);
        }
        byteCount = 0;
        readCase = 0;
        packet = 0;
        LED = 0;
        return true;        
      } else {
        packet = c;
        break;
      }

  }

  if (byteCount > serial_len)
  {
    if(serialErrorHandle(c))
    {    
      publish("ERR: buf_loc > serial_len, LED");
      
      char buff[50];
      sprintf(buff, "INFO: buf = %d, ser = %d", int(byteCount), int(serial_len));
      publish(buff);

      byteCount = 0;
      readCase = 0;
      return true;      
    }
  }

  byteCount++;
  return false;
}



bool readInterpret(byte c)
{
  static byte serial_packet[100];
  static byte serial_buf_loc = 0;
  static byte serial_len;
  static byte readCase = 0;
  
  switch(readCase){
    case 0: 
      memset(serial_packet, 0, sizeof(serial_packet));
      readCase = 1;
      break;
    case 1:
      serial_len = c;
      readCase = 2;
      break;

    case 2:
      serial_packet[serial_buf_loc] = byte(c);   

      if ((c == char(END_BYTE)) && (serial_buf_loc == serial_len))
      {
        commands(serial_packet, (unsigned int) serial_len);
        serial_buf_loc = 0;
        readCase = 0;
        return true;
      }
      if (serial_buf_loc > serial_len)
      {
        if(serialErrorHandle(c))
        {
          publish("ERR: buf_loc > serial_len, readInterpret");
          
          char buff[50];
          sprintf(buff, "INFO: buf = %d, ser = %d", int(serial_buf_loc), int(serial_len));
          publish(buff);

          char duff[50];
          sprintf(duff, "INFO: packet = %s", serial_packet);  
          publish(duff);

          serial_buf_loc = 0;
          readCase = 0;
          return true;      
        }
      }
      serial_buf_loc++;
      break;
  }
  return false;
}



bool stateInfo(byte c)
{
  static byte readCase = 0;
  static byte state;
  static byte count;
  static byte storage[10];
  static bool alloc = true;
  static char serial_packet[100];

  if (alloc)
  {
      state = (c & 0b00011111);
      alloc = false;
      count = 1;

      // char duff[50];
      // sprintf(duff, "INFO: case= = %d", (int)state);  
      // publish(duff);      
      return false;
  }  

  switch (state) {  //request

    case 18:    //Neighbour Disconnected
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: NO %d", int(storage[0])); 
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;          
      } else if (count == 1) {
        storage[0]=c;
        count++;
      } else {
        publish("ERR: Info Case 18");
        count = 0;
        alloc = true;
        return true;
      }
      break;

    case 19:    //ID
      publish("INFO: ID set");
      if (c == char(END_BYTE))
      {
          serial_write_id();
      }
      alloc = true;
      return true;
      break;

    case 20:  //read edges
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: EL %d %d %d", storage[0], storage[1], storage[2]);     
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;
      } else if (count < 5) {
        storage[count-1]=c;
        count++;
      } else {
        publish("ERR: Info Case 20");
        count = 0;
        alloc = true;
        return true;
      }
      break;

    case 21:  //read angles
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: AN %d %d %d", (storage[0]*256+storage[1]), 
                      (storage[2]*256+storage[3]), (storage[4]*256+storage[5]));
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;
      } else if (count < 8) {
        storage[count-1]=c;
        count++;
      } else {
        publish("ERR: Info Case 21");
        count = 0;
        alloc = true;
        return true;
      }
      break;

    case 22:  //read orientation  
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: OR %d %d %d", (storage[0]*256+storage[1]), 
                      (storage[2]*256+storage[3]), (storage[4]*256+storage[5]));
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;
      } else if (count < 8) {
        storage[count-1]=c;
        count++;
      } else {
        publish("ERR: Info Case 22");
        count = 0;
        alloc = true;
        return true;
      }
      break;

    case 23:  //read neighbour
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: NB %.2x%.2x%.2x%.2x%.2x%.2x%.2x", storage[0], storage[1], 
                            storage[2], storage[3], storage[4], storage[5], storage[6]);     
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;
      } else if (count < 8) {
        storage[count-1]=c;
        count++;
      } else {
        publish("ERR: Info Case 23");
        count = 0;
        alloc = true;
        return true;
      }
      break;


    case 24:  //read WiFIEDGE
      if (c == char(END_BYTE))
      {
        sprintf(serial_packet, "REQ: ED %d", int(storage[0])+1); 
        publish(serial_packet);
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        return true;
      } else if (count == 1) {
        storage[0]=c;
        count++;
      } else {
        publish("ERR: Info Case 24");
        count = 0;
        alloc = true;
        return true;
      }
      break;

    case 25:   //WiFiEdge
      if (c == char(END_BYTE))
      {
        if(wifi_edge < 3)
        {
          serial_write_two(0b11011010, wifi_edge);
        } else {
          publish("REQ: WE"); //Request WiFi Edge
        }
      }
      alloc = true;
      return true;
      break;
    
    default:
      if(serialErrorHandle(c))
      {
        memset(serial_packet, 0, sizeof(serial_packet));
        memset(storage, 0, sizeof(storage));        
        alloc = true;
        count = 0;        
        return true;
      }
  }
  return false;
}


bool dataLog(byte c) 
{
  static bool alloc = true;
  static byte count;
  static byte len;
  static byte storage[PACKET_SIZE];
  long time = 0;

  if (alloc) {
    alloc = false;
    storage[0] = (c & 0b00011111);
    len = dataLogCalcLen(storage[0]);
    time = millis();
    storage[1] = (byte) (time >> 24);
    storage[2] = (byte) (time >> 16);
    storage[3] = (byte) (time >> 8);
    storage[4] = (byte) time;
    count = 1;
    return false;
  }

  if (count == 1) {
    storage[5] = c;
    count++;
    return false;
  }

  if ((c == END_BYTE) && (count == len)) {
    char buff[50];
    sprintf(buff, "DLG: ");
    for(int i=0; i<len+4; i++)  
    {
      sprintf(buff, "%s%.2x", buff, storage[i]);
    }
    publish(buff);
    memset(storage, 0, sizeof(storage));
    count = 0;
    alloc = true;
    len = 0;
    return true;
  } else if (count < len) {
    storage[count+4]=c;
    count++;
  } else {
    if(serialErrorHandle(c))
    {
      publish("ERR: Unable to Log");
      memset(storage, 0, sizeof(storage));               
      count = 0;
      alloc = true;
      len = 0;
      return true;
    }        
  }  
  return false;
}


byte dataLogCalcLen(byte alloc_byte)
{
  byte len = 2; // alloc +  end byte
  if(alloc_byte & 0b00000001) len += 6; //angles = 3*16
  if(alloc_byte & 0b00000010) len += 3; //edges = 3*8
  if(alloc_byte & 0b00000100) len += 6; //Orient = 3*16
  return len;
}

bool relayToComputer(byte c)
{
  static byte readCase = 0;
  static byte state;
  static byte count;
  static byte len;
  static byte storage[PACKET_SIZE];
  static bool alloc = true;
  static char wireless_packet[100];

  if (alloc)
  {
      state = (c & 0b00011111);
      alloc = false;
      count = 1;
      return false;
  } else if (len == 0)
  {
    len = c;
    count = 2;
    return false;
  }

  count++;

  switch (state) {  //request

    case 6:  //Relay to WiFi Edge
      if ((c == char(42)) && (count == len))
      {
        // publish("INFO: IN LOOP");
        sprintf(wireless_packet, "REL: ");
        for(int i=0; i<len-3; i++)  
        {
          sprintf(wireless_packet, "%s%c", wireless_packet, storage[i]);
        }
        publish(wireless_packet);
        memset(wireless_packet, 0, sizeof(wireless_packet));
        memset(storage, 0, sizeof(storage));
        count = 0;
        alloc = true;
        len = 0;
        return true;
      } else if (count < len) {
        storage[count-2]=c;
      } else {
        if(serialErrorHandle(c))
        {
          publish("ERR: Unable to Relay");
          memset(wireless_packet, 0, sizeof(wireless_packet));
          memset(storage, 0, sizeof(storage));               
          count = 0;
          alloc = true;
          len = 0;
          return true;
        }        
        break;
      }
      break;
  }
  return false;
}


void purgeSerial()
{
  while(Serial.read() >= 0);
}


bool serialErrorHandle(byte c)
{
  if(c == END_BYTE)
  {
    return true;
  }
  return false;
}