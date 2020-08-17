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