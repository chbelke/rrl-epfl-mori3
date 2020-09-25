void relay(byte* payload, unsigned int len)
{
  verbose_println();
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
  verbose_println(len);

  if(verbose_flag)
  {
    for (int j = byte_count; j < len; j++)
    {
      byte val = payload[j];
      for (int k = 0; k < 8; k++)
      {
          bool b = val & 0x80;
          Serial.print(b);
          val = val << 1;
      }
      Serial.println();
    }    
    Serial.println();
  }

  while(byte_count < len)
  {
    Serial.write(payload[byte_count]);
    byte_count++;
  }

  verbose_println();
}


void serial_write_one(byte alloc)
{  
  Serial.write(alloc);
  Serial.write(END_BYTE);


  if(verbose_flag)
  {
    byte val = alloc;
    for (int k = 0; k < 8; k++)
    {
        bool b = val & 0x80;
        Serial.print(b);
        val = val << 1;
    }
    Serial.println();
  }

  if(verbose_flag)
  {
    byte val = END_BYTE;
    for (int k = 0; k < 8; k++)
    {
        bool b = val & 0x80;
        Serial.print(b);
        val = val << 1;
    }
    Serial.println();
  }  

}


void serial_write_two(byte alloc, byte message)
{  
  Serial.write(alloc);
  Serial.write(message);
  Serial.write(END_BYTE);
}


void serial_write_to_hub(char* buff)
{
  // client.publish(publishName, "INFO: WHAT THE HELL");
  byte len = strlen(buff);

  Serial.write(0b11100110); //Relay to Wifi Edge
  Serial.write(byte(len+11));

  for (byte i=1; i<8; i++)
  {
    Serial.write(clientName[i]);
  }

  for (byte i=0; i<len; i++)
  {
    Serial.write(buff[i]);
  }
  Serial.write(END_BYTE);
  Serial.write(42);



  // char tmp[50];
  // tmp[0] = (0b11100110); //Relay to Wifi Edge
  // tmp[1] = len+8;

  // byte i;
  // for (i=2; i<8; i++)
  // {
  //   tmp[i] = (clientName[i]);
  // }

  // for (i=i; i<len+8; i++)
  // {
  //   tmp[i] = (buff[i]);
  // }

  // tmp[i] = Serial.write(END_BYTE);
  // tmp[i+1] = Serial.write(42);


  // char fuff[50];
  // sprintf(fuff, "INFO: Sent: ");
  // client.publish(publishName, tmp);
}


void serial_write_id()
{
  Serial.write(0b11010011);
  for(byte i=2; i < 8; i++)
    Serial.write(clientName[i]);
  Serial.write(END_BYTE);
}


void verbose_print(char* msg)
{
  if(verbose_flag)
  {
    Serial.print(msg);
  }
}

void verbose_print(char msg)
{
  if(verbose_flag)
  {
    Serial.print(msg);
  }
}

void verbose_println(char* msg)
{
  if(verbose_flag)
  {
    Serial.println(msg);
  }
}

void verbose_println(int msg)
{
  if(verbose_flag)
  {
    Serial.println(msg);
  }
}

void verbose_println()
{
  if(verbose_flag)
  {
    Serial.println();
  }
}