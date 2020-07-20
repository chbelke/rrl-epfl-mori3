//----------------------- Recieved Message --------------------------//
void commands(char* payload)
{
  if (!memcmp(payload,"mac",sizeof("mac")-1))
  {
    pubMac("MAC: ");
    // pubShape();
    pubIP();
  }
  else if (!memcmp(payload,"vg",sizeof("vg")-1))
  {
    runState = 3;
    pubMac("MAC: ");
    pubIP();
    verbose_println("Version Excellent");
  }
  else if (!memcmp(payload,"vb",sizeof("vb")-1))
  {
    runState = 1;
    verbose_println("Version Bad");
  }

  else if (!memcmp(payload,"stopudp",sizeof("stopudp")-1))
  {
    runState = 3;
    verbose_println("UDP communication paused");
  }

  else if (!memcmp(payload,"hello",sizeof("hello")-1)) //respond hello
  {
    client.publish(publishName, "INFO: Hello!");
  }

  else if (!memcmp(payload,"shape",sizeof("shape")-1)) //Send shape
  {
    pubShape();
  }

  else if (!memcmp(payload,"udp",sizeof("udp")-1))
  {
    verbose_println("Recieved UDP Command");
    runState = 4;
  }

  else if (!memcmp(payload,"noudp",sizeof("udp")-1))
  {
    verbose_println("Stop UDP Operation");
    runState = 6;
  }
  else if (!memcmp(payload,"verb",sizeof("udp")-1))
  {
    verbose_flag = true;
    verbose_println("Starting Verbose Operation");
  }
  else if (!memcmp(payload,"noverb",sizeof("udp")-1))
  {
    verbose_flag = false;
    verbose_println("Stopping Verbose Operation");
  }  

  else if (!memcmp(payload,"hi0",sizeof("udp")-1))
  {
    write_serial("HELLO", 0);
  }
  else if (!memcmp(payload,"hi1",sizeof("udp")-1))
  {
    write_serial("WORLD", 1);
  }
  else if (!memcmp(payload,"hi2",sizeof("udp")-1))
  {
    write_serial("BIG", 2);
  }
  else if (!memcmp(payload,"hi3",sizeof("udp")-1))
  {
    write_serial("CHUNGUS", 3);
  }  

  verbose_println("-----------------------");

}