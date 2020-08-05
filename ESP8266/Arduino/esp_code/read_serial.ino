void readSerial()
{
  
  if(Serial.available())
  {
    static int serial_case = 0;    
    
    byte c = Serial.read();

    char state = ((c >> 5) & 0b00000111);
    switch(serial_case){
      case 0:
        if (state == char(0))
        {
          serial_case = 1;
          readVerbose(c);
        } 
        else if (state == char(1))
        {
          serial_case = 2;
          readInterpret(c);
        }
        else if (state == char(2))
        {
          serial_case = 3;
          setLEDs(c);
        }
        break;

      case 1:
        if(readVerbose(c))
        {
          serial_case = 0;
        }
        break;

      case 2:
        if(readInterpret(c))
        {
          serial_case = 0;
        }
        break;        

      case 3:
        if(setLEDs(c))
        {
          serial_case = 0;
        }
        break;
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
  
  switch(readCase){
    case 0: 
      memset(serial_packet, 0, sizeof(serial_packet));
      sprintf(serial_packet, "VBS: ");
      readCase = 1;
      break;
    case 1:
      serial_len = c;
      readCase = 2;
      break;

    case 2:
      serial_packet[serial_buf_loc+5] = c;
      if ((c == char(14)) && (serial_buf_loc == serial_len))
      {
        publish(serial_packet); 
        serial_buf_loc = 0;
        readCase = 0;
        return true;
      }
      // serial_packet[serial_buf_loc+5] = c;
      if (serial_buf_loc > serial_len)
      {
        publish("ERR: buf_loc > serial_len");
        
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


  // for (int kk = 0; kk < 8; kk++)
  // {
  //     bool f = c & 0x80;

  //     char buff[10];
  //     sprintf(buff, "INFO: %c", f);
  //     publish(buff);        
      
  //     c = c << 1;
  //     // delay(1000);
  // }
  // publish("INFO: ByteEND");

  // char nuff[50];
  // sprintf(nuff, "INFO: char = %d", int(c));
  // publish(nuff);  

  // char fuff[50];
  // sprintf(fuff, "INFO: buf = %d, ser = %d", int(byteCount), int(serial_len));
  // publish(fuff);  

  switch(readCase){
    case 0:
      publish("INFO: Case 0");
      if ((c == char(14)) && (byteCount == serial_len))
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
      publish("INFO: Case 1");
      if ((c == 0b0001110) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.On();
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
      publish("INFO: Case 2");
      if ((c == char(14)) && (byteCount == serial_len))
      {
        if(LED == 1)
        {
          led_red.Off();
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
      if ((c == char(14)) && (byteCount == serial_len))
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
      if ((c == char(14)) && (byteCount == serial_len))
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
    publish("ERR: buf_loc > serial_len, LED");
    
    char buff[50];
    sprintf(buff, "INFO: buf = %d, ser = %d", int(byteCount), int(serial_len));
    publish(buff);

    byteCount = 0;
    readCase = 0;
    return true;      
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

      if ((c == char(14)) && (serial_buf_loc == serial_len))
      {
        commands(serial_packet, (unsigned int) serial_len);
        serial_buf_loc = 0;
        readCase = 0;
        return true;
      }
      if (serial_buf_loc > serial_len)
      {
        publish("ERR: buf_loc > serial_len");
        
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
      serial_buf_loc++;
      break;
  }
  return false;
}