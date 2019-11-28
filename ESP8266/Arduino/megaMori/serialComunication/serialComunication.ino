#include <SoftwareSerial.h>

SoftwareSerial gtSerial(8, 7); // Arduino RX, Arduino TX

bool roleAssigned = false;
char role; // Will contain "L" if Mori is a leader and "F" if it is a follower.
const int msgLen =128; //Arbitrary number
char payload[msgLen];
const int nbrSerialPorts = 3; //Each Mori har 3 serial ports (3 sides)

char* megaId;
char bossPort = '0';


void setup() {
    //Instanciate all serial ports
    Serial.begin(115200);
    Serial1.begin(115200);
    Serial2.begin(115200);
    Serial3.begin(115200);
    Serial.println("Define a leader");
  
    //Define an Id for the Mega boards
    pinMode(2,INPUT);
    pinMode(3,INPUT);
    pinMode(4,INPUT);
    delay(500);
    if(digitalRead(2)){
        megaId = "00777AAA";
    }
    if(digitalRead(3)){
        megaId = "00888BBB";
    }
    if(digitalRead(4)){
        megaId = "00999CCC";
    }
    Serial.println(megaId);
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
    }
    else if (!memcmp(pyld+1,"follow",sizeof("follow")-1)){
        Serial.println("Follower defined");
        role = "F";
        roleAssigned = true;
    }
    else if (!memcmp(pyld+1,"ping",sizeof("ping")-1)){
        Serial.println("Ping received");
        respondPing(pyld);
    }
    else if (!memcmp(pyld+1,"resp",sizeof("resp")-1)){
        Serial.println("Ping responded");
        //If a resp message is received, it must be transmitted to leader Mori (through boss port) for mapping
        transmitPing(pyld); 
    }
    else if (!memcmp(pyld+1,"resetMap",sizeof("resetMap")-1)){
        Serial.println("Reset map (bossPort = 0)");
        //If a resp message is received, it must be transmitted to leader Mori (through boss port) for mapping
        if (bossPort != '0'){
           bossPort = '0';
           Serial1.println("resetMap");
           Serial2.println("resetMap");
           Serial3.println("resetMap");
        }
    }
    else if (!memcmp(pyld+1,"pass",sizeof("pass")-1)){
        Serial.println("Pass message received (must pass the message to another mori)");
        //If a resp message is received, it must be transmitted to leader Mori (through boss port) for mapping
        passMessage(pyld); 
    }
    else{
        Serial.println("Unknown command");
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

    //Enter only if the ping is the first ping received (for loops in mori structures)
    if (bossPort == '0' || pyld[0] == bossPort){
         bossPort = pyld[0]; //A ping msg is only received by the Mori closest to the leader Mori
         //Send ping to all other ports
        sendPing(pyld[0]);
    }
 
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
    if (bossPort == '1'){
        //Serial.println("Resp sent");
        Serial1.println(buff);
    }
    else if (bossPort == '2'){
        Serial2.println(buff);
    }
    else if (bossPort == '3'){
        Serial3.println(buff);
    }
}

void sendPing(char portNbr){
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

void passMessage(char* pyld){
    char message[64];
    char pass[4] = "pass";
    int msgStart = 6;
    char portNbr = pyld[msgStart-1];
    int i = 0;

    //Check if the recived message must be transmitted once more or the next recipient is the final receiver.
    if(pyld[msgStart] == ' '){
        Serial.println("Last before final recipient");
        msgStart = 7;
    }
    else{
        //Necessary to keep the correct indexes for the message
        for(i ; i < 4 ; i++){
            message[i] = pass[i];
            msgStart--;
        }
    }
    //Copy the message
    while(pyld[i] != '\0' && i < 64){
        message[i] = pyld[i+msgStart];
        i++;
    }
    message[i] = '\0'; //Set end of array
    
    Serial.print("Message: ");
    Serial.println(message);

    //Send message through the correct port
    if(portNbr == '1'){
        Serial1.println(message);
    }
    if(portNbr == '2'){
        Serial2.println(message);
    }
    if(portNbr == '3'){
        Serial3.println(message);
    }
}

