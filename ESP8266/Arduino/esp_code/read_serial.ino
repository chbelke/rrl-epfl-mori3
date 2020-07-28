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
    			sprintf(serial_packet, "INFO: ");
    			readCase = 1;
    		}
    		break;

		case 1:
			serial_len = c;
		
			readCase = 2;
			break;

		case 2:
			serial_packet[serial_buf_loc+6] = c;

			if ((c == char(14)) && (serial_buf_loc == serial_len))
			{
				publish(serial_packet);	
				serial_buf_loc = 0;
				readCase = 0;
				break;
			}
			if (serial_buf_loc > serial_len)
			{
				publish("ERR: buf_loc > serial_len");
				
				char buff[50];
				sprintf(buff, "INFO: buf = %d, ser = %d", int(serial_buf_loc), int(serial_len));
				publish(buff);

				char duff[50];
				sprintf(duff, "INFO: packet = %s", serial_packet);	
				publish(duff);

				serial_buf_loc = 0;
				readCase = 0;
				break;					
			}
			serial_buf_loc++;
			break;

		}

	}
}