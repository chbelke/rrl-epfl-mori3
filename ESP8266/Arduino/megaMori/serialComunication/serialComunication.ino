#include <SoftwareSerial.h>

SoftwareSerial gtSerial(8, 7); // Arduino RX, Arduino TX

bool roleAssigned = false;
char role; // Will contain "L" if Mori is a leader and "F" if it is a follower.
const int msgLen = 64; //Arbitrary number
char payload[msgLen];
const int nbrSerialPorts = 3; //Each Mori har 3 serial ports (3 sides)
const int maxNbrMoris = 4;
int moriMap[maxNbrMoris][nbrSerialPorts];

const int nbrMegaBoards = 2;
const int moriStructLength = 6;
//The following array is used to emulate the connexions between Moris 
//char moriStruct[nbrSerialPorts*nbrMegaBoards][3] = 
char moriStruct[moriStructLength][3] = 
             { {"1a2"},//[nbrSerialPorts(2 MEGAs with 3 ports each] [Sender Mori Id + connected port + receiver Mori Id]
               {"1b3"},//The first element is the Id of the Mori, the second is the connected side of a mori('a','b' or 'c')
               {"3c1"},//and the last is the receiver Mori
               {"2b1"},//};/*,
               {"1c4"},
               {"4c1"} };//*/


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

  //COntinuously listen on all serial ports
  if(recvSerial0()){
    Serial.print("Payload0 = ");
    Serial.println(payload);
    decodeMsg();
  }
  if(recvSerial1()){
    Serial.print("Payload1 = ");
    Serial.println(payload);
    decodeMsg();
  }
  if(recvSerial2()){
    Serial.print("Payload2 = ");
    Serial.println(payload);
    decodeMsg();
  }
  if(recvSerial3()){
    Serial.print("Payload3 = ");
    Serial.println(payload);
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

        Serial.print("Serial 1 received: ");
        Serial.println(receivedByte);
        
        payload[i] = receivedByte;
        Serial.println(payload[i]);
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

        Serial.print("Serial 2 received: ");
        Serial.println(receivedByte);
        
        payload[i] = receivedByte;
        Serial.println(payload[i]);
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

        Serial.print("Serial 3 received: ");
        Serial.println(receivedByte);
        
        payload[i] = receivedByte;
        Serial.println(payload[i]);
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
    //Serial.print("Payload: ");
    //Serial.println(payload);

    //First element of the payload = port on which the message was received
    if (!memcmp(payload+1,"leader",sizeof("leader")-1)){
        Serial.println("Leader defined");
        role = "L";
        roleAssigned = true;
        Serial1.println("follow"); // Other Moris will be followers
        Serial2.println("follow");
        Serial3.println("follow");
        sendPing(49); //49 is ascii for 1, which is the Mori's Id (leader will always be Id "1" in the emulation)
    }
    else if (!memcmp(payload+1,"follow",sizeof("follow")-1)){
        Serial.println("Follower defined");
        role = "F";
        roleAssigned = true;
    }
    else if (!memcmp(payload+1,"ping",sizeof("ping")-1)){
        Serial.println("Ping received");
        respondPing(payload[0], payload[sizeof("ping")]);
    }
    else if (!memcmp(payload+1,"resp",sizeof("resp")-1)){
        Serial.println("Ping responded");
        mapMoris(payload[5],payload[6],payload[0], payload[7]);
    }
    else{
        Serial.println("Unknown command");
    }
    payload[0] = '\0'; //Reset the payload
}

void sendPing(char moriId){
    char buf[5] = "pingX";
    
    //Normally, a ping would be sent from eache port of each Mori. But here with only 2 Mega boards,
    //an emulation is needed and the following code cannot be used.
    /*
    strcat(buf,ID);
  
    Serial1.println(buf);
    Serial2.println(buf);
    Serial3.println(buf);
    */
  
    //Send ping messages only to Moris connected (using the correctly emulated ports)
    for (int i = 0 ; i < moriStructLength ; i++){
        if (moriStruct[i][0] == moriId){
            buf[4] = moriId;
            buf[5] = '\0'; //Set end of array
            if (moriStruct[i][1] == 'a'){
                Serial1.println(buf);
            }
            else if (moriStruct[i][1] == 'b'){
                Serial2.println(buf);
            }
            else if (moriStruct[i][1] == 'c'){
                Serial3.println(buf);
            }
        }
    }
}

void respondPing(char portNbr, char senderId){
    //Normally, a simple respond of the Mori's Id number would be enough but since
    //the Mega boards do not have an Id, we must look into our emulated system to 
    //find the correct Mori Id.
    char buf[6] = "respXY";

    for (int i = 0 ; i < moriStructLength ; i++){
        if (moriStruct[i][2] == senderId && moriStruct[i][1] == portNbr + 48){ //48 is to convert to ascii
            buf[4] = moriStruct[i][2];
            buf[5] = moriStruct[i][0]; //Only way to find the emulated receiver Mori Id
            buf[6] =  moriStruct[i][1]; //Send receiver Mori's port
            buf[7] = '\0'; //Set end of array
            Serial.println(buf);
            if (moriStruct[i][1] == 'a'){
                Serial1.println(buf);
            }
            else if (moriStruct[i][1] == 'b'){
                Serial2.println(buf);
            }
            else if (moriStruct[i][1] == 'c'){
                Serial3.println(buf);
            }
        }
    }
}

void mapMoris(char senderMori, char receiverMori, char senderPort, char receiverPort){
    //This function will create the map of the connected Moris according to the pings sent and received.
    
    //Conversions to fill the moriMap
    moriMap[senderMori-'1'][senderPort-'1'] = receiverMori-'0';
    moriMap[receiverMori-'1'][receiverPort-'a'] = senderMori-'0';

    //Display moriMap
    Serial.println("Mori Map: ");
    for (int i = 0 ; i < maxNbrMoris ; i++){
        for (int j = 0 ; j < 3 ; j++){
            Serial.print(moriMap[i][j]);
        }
        Serial.println();
    }
}

