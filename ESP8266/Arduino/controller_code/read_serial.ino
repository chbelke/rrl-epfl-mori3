char msgBuf[PACKET_SIZE];
int bufLoc = 5;
int readCase = 0;
char* udp_header = "rel ";

void sendSerial()
{
	if(Serial.available())
	{
		Serial.print("Character: ");
		byte c = Serial.read();
		Serial.println((char)c);
		switch(readCase){
		case 0: 
    		if(c == char(13))
    		{
    			Serial.println("in 13 loop");
    			msgBuf[bufLoc] = c;
    			bufLoc++;
    			readCase = 1;
    		}
    		break;

		case 1:
			msgBuf[bufLoc] = c;
			bufLoc++;
			Serial.println("case1");

			if (c == char(14))
			{
				Serial.println("breaking out");
				for(int i=0; i<4; i++)
				{
					msgBuf[i+1] = udp_header[i];
				}
				Serial.println(bufLoc);
				Serial.println(msgBuf);
				msgBuf[0] = bufLoc-1;
				writeUDP(msgBuf);
				memset(msgBuf, 0, PACKET_SIZE);
				readCase = 0;
				bufLoc = 5;
			}
			break;
		}

	}
}