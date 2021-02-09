void relay(uint8_t* payload, unsigned int len)
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
      uint8_t val = payload[j];
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
    write_to_buffer(payload[byte_count]);
    byte_count++;
  }

  verbose_println();
}


void serial_write_one(uint8_t alloc)
{  
  write_to_buffer(alloc);
  write_to_buffer(END_BYTE);


  if(verbose_flag)
  {
    uint8_t val = alloc;
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
    uint8_t val = END_BYTE;
    for (int k = 0; k < 8; k++)
    {
        bool b = val & 0x80;
        Serial.print(b);
        val = val << 1;
    }
    Serial.println();
  }  

}


void serial_write_two(uint8_t alloc, uint8_t message)
{  
  write_to_buffer(alloc);
  write_to_buffer(message);
  write_to_buffer(END_BYTE);
}

void serial_write_three(uint8_t alloc, uint8_t message1, uint8_t message2)
{  
  write_to_buffer(alloc);
  write_to_buffer(message1);
  write_to_buffer(message2);
  write_to_buffer(END_BYTE);
}


void serial_write_flags(uint8_t message)
{  
  write_to_buffer(0b11010001);
  write_to_buffer(message);
  write_to_buffer(END_BYTE);
}


void serial_write_to_hub(char* buff)
{
  // client.publish(publishName, "INFO: WHAT THE HELL");
  uint8_t len = strlen(buff);

  write_to_buffer(0b11100110); //Relay to Wifi Edge
  
  if (clientLetter == 255)
  {
    write_to_buffer(byte(len+11));
    for (uint8_t i=1; i<8; i++)
    {
      write_to_buffer(clientName[i]);
    }
  }
  else
  {
    write_to_buffer(byte(len+5));
    write_to_buffer(clientLetter);

  }

  for (uint8_t i=0; i<len; i++)
  {
    write_to_buffer(buff[i]);
  }
  write_to_buffer(END_BYTE);
  write_to_buffer(42);



  // char tmp[50];
  // tmp[0] = (0b11100110); //Relay to Wifi Edge
  // tmp[1] = len+8;

  // uint8_t i;
  // for (i=2; i<8; i++)
  // {
  //   tmp[i] = (clientName[i]);
  // }

  // for (i=i; i<len+8; i++)
  // {
  //   tmp[i] = (buff[i]);
  // }

  // tmp[i] = write_to_buffer(END_BYTE);
  // tmp[i+1] = write_to_buffer(42);


  // char fuff[50];
  // sprintf(fuff, "INFO: Sent: ");
  // client.publish(publishName, tmp);
}


void serial_write_id()
{
  write_to_buffer(0b11010011);
  for(uint8_t i=2; i < 8; i++)
    write_to_buffer(clientName[i]);
  write_to_buffer(END_BYTE);
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