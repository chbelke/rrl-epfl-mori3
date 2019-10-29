byte receivedChar;
boolean newData = false;

void setup() {
    Serial.begin(115200);
}

void loop() {
    recvOneChar();
    showNewData();
}

void recvOneChar() {
    if (Serial.available() > 0) {
        receivedChar = Serial.read();
        newData = true;
    }
}

void showNewData() {
    if (newData == true) {
        Serial.println(receivedChar, DEC);
        newData = false;
    }
}
