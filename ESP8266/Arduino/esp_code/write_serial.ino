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
  Serial.println(len);

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

  for (int jj = byte_count; jj < len; jj++)
  {
    byte val = payload[jj];
    for (int kk = 0; kk < 8; kk++)
    {
        bool f = val & 0x80;

        char buff[10];
        sprintf(buff, "INFO: %c", f);
        publish(buff);        
        
        val = val << 1;
        // delay(1000);
    }
    publish("INFO: ");
  }  


  while(byte_count < len)
  {
    Serial.write(payload[byte_count]);
    byte_count++;
  }

  verbose_println();
}


void write_serial(char* msg, int mode)
{
  int b_size = (PACKET_SIZE + 1)*2;
  byte msgShift[b_size];
  memset(msgShift, 0b00000000, b_size);
  // Serial.write(msgShift);
  
  int i;
  for(i=0; msg[i] != 0; i++)
  {
    msgShift[1+i] = msg[i];
  }

  switch(mode){
  case 0:  //Verbose
    msgShift[0] = 0b10000000;
    msgShift[i+1] = 0b11111111;
    break;
  case 1: // Recieve and relay
    msgShift[0] = 0b00000100;
    break;
  case 2: // Relay
    msgShift[0] = 0b00000010;
    break;
  case 3: //Normal - interpret
    msgShift[0] = 0b00000001;
    break;
  }


  for(int j=0; msgShift[j] != 0b00000000; j++)
  {
    Serial.write(msgShift[j]);
  }

}

void verbose_print(char* msg)
{
  if(verbose_flag)
  {
    Serial.print(msg);
    // write_serial(msg, 0);
  }
}

void verbose_print(char msg)
{
  if(verbose_flag)
  {
    Serial.print(msg);
    // write_serial(msg, 0);
  }
}

void verbose_println(char* msg)
{
  if(verbose_flag)
  {
    Serial.println(msg);
    // write_serial(msg, 0);
    // write_serial(nl, 0);
  }
}

void verbose_println(int msg)
{
  if(verbose_flag)
  {
    // char buf[4];
    // sprintf (buf, "%03i", msg);
    // write_serial(buf, 0);
    Serial.println(msg);
  }
}

void verbose_println()
{
  if(verbose_flag)
  {
    // write_serial(nl, 0);
    Serial.println();
  }
}