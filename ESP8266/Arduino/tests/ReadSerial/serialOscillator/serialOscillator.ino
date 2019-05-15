
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

}

void loop() 
{ 
  for(byte i=0;i<255;i++)
  {
      Serial.write(i);
//      Serial.println(i);
      delay(10);
  }
}
