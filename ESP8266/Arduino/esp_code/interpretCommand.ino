//----------------------- Recieved Message --------------------------//
void commands(char* payload)
{
  int sw_case = 14;
  
  for(int i=0; i < 13; i++)
  {
    if (!memcmp(payload, cmdLine[i], 4)) //4 is number of bytes in memory to compare (3 chars + stop)
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

    case 5:   //shape
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

    case 10:  //hi0
      write_serial("HELLO", 0);
      break;

    case 11:  //hi1
      write_serial("WORLD", 0);
      break;

    case 12:  //hi2      
      write_serial("BIG", 2);
      break;

    case 13:
      write_serial("CHUNGUS", 3);
      break;

    default:
      publish("ERR: Command not understood");
  }

  verbose_println("-----------------------");

}