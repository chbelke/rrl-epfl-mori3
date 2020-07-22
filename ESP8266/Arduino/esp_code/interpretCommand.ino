//----------------------- Recieved Message --------------------------//
void commands(byte* payload, unsigned int len)
{
  int sw_case = 14;

  char topic[3];
  for(int i=0; i < 3; i++)
  {
    topic[i] = (char)payload[i];
  }
  
  for(int i=0; i < 13; i++)
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
      client.publish(publishName, "INFO: Hello!");
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

    case 11:  //hi1
      break;

    case 12:  //hi2      
      break;

    case 13:
      break;

    default:
      publish("ERR: Command not understood");
  }

  verbose_println("-----------------------");
}


