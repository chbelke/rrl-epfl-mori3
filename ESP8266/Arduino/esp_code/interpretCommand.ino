//----------------------- Recieved Message --------------------------//
void commands(byte* payload, unsigned int len)
{
  for(int i=0; i<len; i++)
  {
    verbose_print((char)payload[i]);
  }
  
  int sw_case = 27;

  char topic[3];
  for(int i=0; i < 3; i++)
  {
    topic[i] = (char)payload[i];
  }
  
  for(int i=0; i < 26; i++)
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


    default:
      publish("ERR: Command not understood");
  }

  verbose_println("-----------------------");
}


void setWifiEdge(byte* payload, unsigned int len)
{
  int byte_count = 0;

  while(payload[byte_count] != 0b00100000)   //0b00100000 = whitespace
  {
    byte_count++;
    if(byte_count > len)
    {
      publish("ERR: payload has no space");
      break;
    }
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

  while(payload[byte_count] != 0b00100000)   //0b00100000 = whitespace
  {
    byte_count++;
    if(byte_count > len)
    {
      publish("ERR: payload has no space");
      break;
    }
  }
  byte_count++;

  byte neighbour = payload[byte_count]-48;   //-48 = ascii to int conversion

  // char buff[50];
  // sprintf(buff, "INFO: Req Neighbour %d", int(neighbour));
  // publish(buff);

  serial_write_two(0b11010111, neighbour);
}

