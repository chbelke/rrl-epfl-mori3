#include <SoftwareSerial.h>

SoftwareSerial gtSerial(8, 7); // Arduino RX, Arduino TX

bool roleAssigned = false;
char role; // Will contain "L" if Mori is a leader and "F" if it is a follower.
const int msgLen =128; //Arbitrary number
char payload[msgLen];
const int nbrSerialPorts = 3; //Each Mori har 3 serial ports (3 sides)

char* megaId;
char bossPort;


void setup() {
  //Instanciate all serial ports
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial3.begin(115200);
  Serial.println("Define a leader"); 
}

void loop() {
  // put your main code here, to run repeatedly:

  //Continuously listen on all serial ports
  if(recvSerial0()){
    decodeMsg();
  }
  if(recvSerial1()){
    decodeMsg();
  }
  if(recvSerial2()){
    decodeMsg();
  }
  if(recvSerial3()){
    decodeMsg();
  }
}

bool recvSerial0(){
    int i = 0;
    byte receivedByte;
    
    payload[0] = '\0'; //Reset the payload array
    while (Serial.available() > 0) {
        //Set the first element of the payload to the number of the port on which the message is received
        if (i == 0){ 
            payload[0] = 48; //ascii for "0"
            i++;
        } 
        receivedByte = Serial.read();
        
        // When a commamd is written through the console, a 13 and a 10 (bytes) are added
        // at the end of the command. The following condition is made to ignore them,
        // or to empty the buffer if the given command is too long.
        if(receivedByte == 13){   
            //Serial.println("TOO LONG");
            Serial.read();
            break;
        }
        Serial.print("Received: ");
        Serial.println(receivedByte);
        
        payload[i] = receivedByte;
        Serial.println(payload[i]);
        
        delay(10);//Tmortant to ensure the whole message has been received 
        
        i++;
    }
    payload[i] = '\0'; //Set the end of the message
    
    if (i > 0){ // Message received
      return true;
    }
    else{
      return false;
    }
}

bool recvSerial1(){
    int i = 0;
    byte receivedByte;
    
    payload[0] = '\0'; //Reset the payload array
    while (Serial1.available() > 0) {
        //Set the first element of payload to the number of the port on which the message is received
        if (i == 0){ 
            payload[0] = 49; //ascii for "1"
            i++;
        }
        receivedByte = Serial1.read();

        // When a commamd is written through the console, a 13 and a 10 (bytes) are added
        // at the end of the command. The following condition is made to ignore them,
        // or to empty the buffer if the given command is too long.
        if(receivedByte == 13){   
            //Serial.println("TOO LONG");
            Serial1.read();
            break;
        }
        payload[i] = receivedByte;
        
        //Serial.print("Serial 1 received: ");
        //Serial.println(receivedByte);
        Serial.println(payload[i]);

        delay(10);//Tmortant to ensure the whole message has been received 
        
        i++;
    }
    payload[i] = '\0'; //Set the end of the message
        
    if (i > 0){ // Message received
      return true;
    }
    else{
      return false;
    }
}

bool recvSerial2(){
    int i = 0;
    byte receivedByte;
    payload[0] = '\0'; //Reset the payload array
    while (Serial2.available() > 0) {
        //Set the first element of payload to the number of the port on which the message is received
        if (i == 0){ 
            payload[0] = 50; //ascii for "2"
            i++;
        }
        receivedByte = Serial2.read();

        // When a commamd is written through the console, a 13 and a 10 (bytes) are added
        // at the end of the command. The following condition is made to ignore them,
        // or to empty the buffer if the given command is too long.
        if(receivedByte == 13){   
            //Serial.println("TOO LONG");
            Serial2.read();
            break;
        }
        payload[i] = receivedByte;
        
        //Serial.print("Serial 2 received: ");
        //Serial.println(receivedByte);
        Serial.println(payload[i]);
        
        delay(10);//Tmortant to ensure the whole message has been received 
        
        i++;
    }
    payload[i] = '\0'; //Set the end of the message
        
    if (i > 0){ // Message received
      return true;
    }
    else{
      return false;
    }
}

bool recvSerial3(){
    int i = 0;
    byte receivedByte;
    
    payload[0] = '\0'; //Reset the payload array
    while (Serial3.available() > 0) {
        //Set the first element of payload to the number of the port on which the message is received
        if (i == 0){ 
            payload[0] = 51; //ascii for "3"
            i++;
        }
        receivedByte = Serial3.read();

        // When a commamd is written through the console, a 13 and a 10 (bytes) are added
        // at the end of the command. The following condition is made to ignore them,
        // or to empty the buffer if the given command is too long.
        if(receivedByte == 13){   
            //Serial.println("TOO LONG");
            Serial3.read();
            break;
        }
        payload[i] = receivedByte;
        
        //Serial.print("Serial 3 received: ");
        //Serial.println(receivedByte);
        Serial.println(payload[i]);

        delay(10);//Tmortant to ensure the whole message has been received 
        
        i++;
    }
    payload[i] = '\0'; //Set the end of the message
        
    if (i > 0){ // Message received
      return true;
    }
    else{
      return false;
    }
}

void decodeMsg(){
    char pyld[msgLen];
    //Store the payload in another variable in order to avoir using a global variable
    for(int i = 0 ; i < msgLen ; i++){
        pyld[i] = payload[i];
    }
    payload[0] = '\0';//Reset the payload
    Serial.print("Payload: ");
    Serial.println(pyld);

    //First element of the payload = port on which the message was received
    if (!memcmp(pyld+1,"leader",sizeof("leader")-1)){
        Serial.println("Leader defined");
        role = "L";
        roleAssigned = true;
        Serial1.println("follow"); // Other Moris will be followers
        Serial2.println("follow");
        Serial3.println("follow");
        megaId = "00777AAA";
    }
    else if (!memcmp(pyld+1,"follow",sizeof("follow")-1)){
        Serial.println("Follower defined");
        role = "F";
        roleAssigned = true;
        megaId = "00888BBB";
    }
    else if (!memcmp(pyld+1,"ping",sizeof("ping")-1)){
        Serial.println("Ping received");
        bossPort = pyld[0]; //A ping msg is only received by the Mori closest to the leader Mori
        respondPing(pyld);
    }
    else if (!memcmp(pyld+1,"resp",sizeof("resp")-1)){
        Serial.println("Ping responded");
        //If a resp message is received, it must be transmitted to leader Mori (through boss port) for mapping
        transmitPing(pyld); 
    }
    else{
        Serial.println("Unknown command");
    }
}

char* sendPing(char portNbr){
  //Send a ping to other ports
  char* buff = "pingXYYYYYYYY";
  //strcat(buff,megaId);
  int portIndex = 4;
  int espIdLen = 8;

  for(int i = 0 ; i < espIdLen ; i++){
    //Copy the Id of the mega board
    buff[portIndex+1+i] = megaId[i];
  }
  
  if (portNbr == '1'){
      buff[portIndex] = '2';
      Serial2.println(buff);
      buff[portIndex] = '3';
      Serial3.println(buff);
  }
  else if (portNbr == '2'){
      buff[portIndex] = '1';
      Serial1.println(buff);
      buff[portIndex] = '3';
      Serial3.println(buff);
  }
  else if (portNbr == '3'){
      buff[portIndex] = '1';
      Serial1.println(buff);
      buff[portIndex] = '2';
      Serial2.println(buff);
  }
}


void respondPing(char* pyld){
    //If a ping is received, respond to boss Mori for mapping
    int portIndex = 13;
    int espMsgStartIndex = 4;
    int espMsgLen = 9;

    //MSG: resp-espSendPort-espId-megaReceivePort-megaId
    char* buff = "respXYYYYYYYYPZZZZZZZZ";

    buff[portIndex] = pyld[0];

    //Send ping to all other ports
    sendPing(pyld[0]);

    //Create mapping message
    for(int i = 0 ; i < espMsgLen ; i++){
      buff[espMsgStartIndex+i] = pyld[espMsgStartIndex+1+i];
      if(i != espMsgLen-1){
        //This is to correctly copy the Id of the mega board
        buff[portIndex+1+i] = megaId[i];
      }
    }
    buff[portIndex] = pyld[0];
    buff[portIndex+espMsgLen] = '\0'; //Set end of array
    
    Serial.println(buff);

    //Send back message to boss
    if (pyld[0] == '1'){
        //Serial.println("Resp sent");
        Serial1.println(buff);
    }
    else if (pyld[0] == '2'){
        Serial2.println(buff);
    }
    else if (pyld[0] == '3'){
        Serial3.println(buff);
    }
}

void transmitPing(char* pyld){
    //Simply transmit the ping respond through the boss port
    int respLen = 22;
    char* buff = "respYXXXXXXXXP00000000";

    //Take out the first ele,ent og the payload (corresponds to receive port)
    for (int i = 0 ; i < respLen ; i++){
        buff[i] = pyld[i+1];
    }
    buff[22] = '\0'; //Set end of array
    
    Serial.println(buff);
    
    if (bossPort == '1'){
        Serial1.println(buff);
    }
    else if (bossPort == '2'){
        Serial2.println(buff);
    }
    else if (bossPort == '3'){
        Serial3.println(buff);
    }
}

