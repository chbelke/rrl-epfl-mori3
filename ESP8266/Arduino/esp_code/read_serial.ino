char serial_packet[100];
byte serial_buf_loc = 0;
int serial_case = 0;
byte readCase = 0;
byte serial_len;

void readSerial()
{
	
	if(Serial.available())
	{
		byte c = Serial.read();
		switch(readCase){
		case 0: 
    		if (c == char(5))
    		{
    			memset(serial_packet, 0, sizeof(serial_packet));
    			readCase = 1;
    		}
    		break;

		case 1:
			serial_len = c;
			readCase = 2;
			break;

		case 2:
			serial_packet[serial_buf_loc] = c;
			if ((c == char(14)) && (serial_buf_loc == serial_len))
			{
				publish(serial_packet);	
				readCase = 0;
				break;
			}
			if (serial_buf_loc >= serial_len)
			{
				publish("ERR: buf_loc > serial_len");
				
				char buff[50];
				sprintf(buff, "INFO: buf = %c, ser = %c", serial_buf_loc, serial_len);
				publish(buff);

				char duff[50];
				for(int i=0; i < serial_len; i++)
				{
					sprintf(duff, "INFO: packet = %c", serial_packet[i]);	
				}
				publish(duff);

				readCase = 0;
				break;					
			}
			serial_buf_loc++;
			break;

		}

	}
}