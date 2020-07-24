class Led
{
private:
  int ledPin;      // the number of the LED pin
  int ledState = LOW;                 // ledState used to set the LED
  unsigned long previousMillis = 0;   // will store last time LED was updated
 
public:
  Led(int pin);
  void Toggle();
  void On();
  void Off();
};

Led::Led(int pin){
  this->ledPin = pin;
  pinMode(this->ledPin, OUTPUT);
  Off();
};

 
void Led::Toggle()
{
  // check to see if it's time to change the state of the LED
  unsigned long currentMillis = millis();
   
  if(ledState == HIGH)
  {
    ledState = LOW;  // Turn it off
    digitalWrite(ledPin, ledState);  // Update the actual LED
  }
  else
  {
    ledState = HIGH;  // turn it on
    digitalWrite(ledPin, ledState);   // Update the actual LED
  }
}

void Led::On()
{
  digitalWrite(ledPin, LOW);
}
void Led::Off()
{
  digitalWrite(ledPin, HIGH);
}