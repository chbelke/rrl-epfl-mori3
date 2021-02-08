class Led
{
private:
  int ledPin;      // the number of the LED pin
  int ledState = LOW;                 // ledState used to set the LED
  unsigned long previousMillis = 0;   // will store last time LED was updated
  bool flg_blink;
  unsigned int freq_blink = 300;
 
public:
  Led(int pin);
  void Toggle();
  void On();
  void Off();
  void Blink();
  void Update();
  bool getBlinkFlag();
  void setBlinkFreq(byte);
};

Led::Led(int pin){
  this->ledPin = pin;
  pinMode(this->ledPin, OUTPUT);
  On();
};

 
void Led::Toggle()
{
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

void Led::Blink()
{
  flg_blink = !flg_blink;
}

void Led::Update()
{
  if(!flg_blink)
  {
    return;
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis > freq_blink)
  {
    Toggle();
    previousMillis = currentMillis;
  }
}

bool Led::getBlinkFlag()
{
  return flg_blink;
}

void Led::setBlinkFreq(uint8_t input)
{
  freq_blink = (unsigned int)(input*4);
}

void Led::On()
{
  
  flg_blink = false;
  digitalWrite(ledPin, LOW);
}
void Led::Off()
{
  flg_blink = false;
  digitalWrite(ledPin, HIGH);
}