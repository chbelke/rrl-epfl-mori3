/* ********************************************************************************************** */
/* *** MORI JOYSTICK FIRMWARE ******************************************************************* */
/* ********************************************************************************************** */
#include <LiquidCrystal.h>
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
// wiring: https://www.arduino.cc/en/uploads/Tutorial/LCD_Base_bb_Schem.png

/* Initialise timer2 interrupt (see bottom for details) */
#define TIMER2_PRELOAD ((uint8_t) 200)      // timer preload
#define TIMER2_COUNT ((uint8_t) 2)          // timer overflow counter max
uint8_t Timer2OverflowCounter = 0;          // timer overflow counter

// CMD vars - interrupt 50 Hz & counter, CMD sent in loop when:
#define CMD_UpdateTrigger 5                 // 50 Hz / CMD_UpdateTrigger = 10 Hz
uint16_t CMD_UpdateCounter = 0;             // CMD interrupt counter
volatile boolean CMD_UpdateFlag = false;    // CMD flag for loop()
#define CMD_Beg 13                          // start verification byte
#define CMD_End 14                          // end verification byte

// potentiometer values
uint16_t MotLin[3] = {500, 500, 500};       // extension pot values
uint16_t Joystick[3] = {0, 0, 0};           // joystick pot values
uint8_t JoystickMapped0 = 0;                // joystick mapped to 8 bit
int8_t JoystickMapped1 = 0;                 // not array due to data types
int8_t JoystickMapped2 = 0;

// coupling button eval
bool CouplFlag[3] = {false, false, false};  // coupling button flag
uint8_t CouplDelayCount[3] = {0, 0, 0};     // count from coupling trigger
#define CouplDelay 150                      // coupling display after trigger
uint8_t CouplCount = 0;                     // open coupling count
char CouplInd[3] = "ABC";                   // string array displayed when coupling
uint8_t CouplOut = B10000000;              // coupling CMD byte

// driving edge button eval
uint8_t DriveEdge = 0;                      // driving edge selection
volatile uint8_t DriveEdgeDelayCount = 0;   // joystick button delay count
#define DriveEdgeDelay 50                   // debounce (of sorts) - 1 second
#define DriveDeadZone 30                    // joystick centre dead zone

// special LCD characters
byte upArrow[] = {B00000, B00100, B01110, B10101, B00100, B00100, B00000, B00000};
byte downArrow[] = {B00000, B00100, B00100, B10101, B01110, B00100, B00000, B00000};
byte twistLeft[] = {B01110, B10001, B10100, B10010, B01111, B00010, B00100, B00000};
byte twistRight[] = {B01110, B10001, B00101, B01001, B11110, B01000, B00100, B00000};
byte edgeChar[] = {B00000, B11111, B00000, B01010, B00000, B00100, B00000, B00000};


/* ********************************************************************************************** */
/* *** INIT ************************************************************************************* */
/* ********************************************************************************************** */
void setup() {
  pinMode(9, OUTPUT);
  pinMode(2, INPUT_PULLUP);
  pinMode(6, INPUT);
  pinMode(7, INPUT);
  pinMode(8, INPUT);
  pinMode(9, INPUT);
  pinMode(10, INPUT);
  pinMode(13, INPUT);

  Serial.begin(115200);
  lcd.begin(16, 2);
  lcd.createChar(0, upArrow);
  lcd.createChar(1, downArrow);
  lcd.createChar(2, twistLeft);
  lcd.createChar(3, twistRight);
  lcd.createChar(4, edgeChar);

  lcd.clear();
  lcd.print("Mori Drive Input");
  lcd.setCursor(0, 1);
  lcd.print("-------");
  lcd.write((byte)4);
  lcd.write((byte)4);
  lcd.print("-------");
  delay(3000);
  lcd.clear();

  /* Timer2 Init */
  TCCR2B = 0x00;                            // disable timer2 during setup
  TCNT2  = TIMER2_PRELOAD;                  // set timer2 preload
  TIFR2  = 0x00;                            // clear timer2 overflow interrupt flag
  TIMSK2 = 0x01;                            // enable timer2 overflow interrupt
  TCCR2A = 0x00;                            // set wave generator to normal mode
  TCCR2B = 0x07;                            // set the prescaler to 1024
}


/* ********************************************************************************************** */
/* *** MAIN ************************************************************************************* */
/* ********************************************************************************************** */
void loop() {

  // coupling buttons only when extension input off
  if (digitalRead(6) == false) {
    if (digitalRead(10)) {                  // coupling A button
      CouplDelayCount[0] = 0;
      CouplFlag[0] = true;
      CouplOut = CouplOut | B00100000;
    }
    if (digitalRead(9)) {                   // coupling B button
      CouplDelayCount[1] = 0;
      CouplFlag[1] = true;
      CouplOut = CouplOut | B00010000;
    }
    if (digitalRead(8)) {                   // coupling C button
      CouplDelayCount[2] = 0;
      CouplFlag[2] = true;
      CouplOut = CouplOut | B00001000;
    }
  }

  // joystick button
  if (!digitalRead(13) && (DriveEdgeDelayCount >= DriveEdgeDelay)) {
    DriveEdge++;
    if (DriveEdge >= 3) {
      DriveEdge = 0;
    }
    DriveEdgeDelayCount = 0;
  }


  // CMD send interval
  if (CMD_UpdateFlag) {

    /* *** EXTENSION INPUT ********************************************************************** */
    if (digitalRead(6) == true) {
      MotLin[0] = map(analogRead(A3), 0, 1023, 200, 900);
      MotLin[1] = map(analogRead(A4), 0, 1023, 200, 900);
      MotLin[2] = map(analogRead(A5), 0, 1023, 200, 900);

      Serial.write(CMD_Beg);
      Serial.write(B00111000);
      Serial.write((uint8_t)(MotLin[0] >> 8));
      Serial.write((uint8_t)(MotLin[0] & 0x00FF));
      Serial.write((uint8_t)(MotLin[1] >> 8));
      Serial.write((uint8_t)(MotLin[1] & 0x00FF));
      Serial.write((uint8_t)(MotLin[2] >> 8));
      Serial.write((uint8_t)(MotLin[2] & 0x00FF));
      Serial.write(CMD_End);

      MotLin[0] = map(MotLin[0], 200, 900, 900, 200);
      MotLin[1] = map(MotLin[1], 200, 900, 900, 200);
      MotLin[2] = map(MotLin[2], 200, 900, 900, 200);

      // LCD
      lcd.setCursor(0, 1);
      lcd.print("Ext: ");
      lcd.print(MotLin[0]);
      lcd.print(" ");
      lcd.print(MotLin[1]);
      lcd.print(" ");
      lcd.print(MotLin[2]);
      // LCD


      /* *** COUPLING INPUT ********************************************************************* */
    } else if (CouplFlag[0] || CouplFlag[1] || CouplFlag[2]) {
      Serial.write(CMD_Beg);
      Serial.write(CouplOut);
      Serial.write(CMD_End);

      // LCD
      lcd.setCursor(0, 1);
      lcd.print("Coupl. ");
      CouplCount = 0;
      for (int j = 0; j <= 2; j++) {
        if (CouplFlag[j]) {
          lcd.print(CouplInd[j]);
          CouplCount++;
        }
      }
      lcd.print(" open");
      if (CouplCount == 1) {
        lcd.print("  ");
      } else if (CouplCount == 2) {
        lcd.print(" ");
      }
      lcd.print(" ");
      // LCD

    } else {
      // LCD
      lcd.setCursor(0, 1);
      lcd.print("                ");
      // LCD
    }

    /* *** DRIVE INPUT ************************************************************************** */
    if (digitalRead(7) == true) {
      // read joystick values
      Joystick[0] = analogRead(A2);
      Joystick[1] = analogRead(A1);
      Joystick[2] = analogRead(A0);

      for (int i = 0; i <= 2; i++) {
        if ((Joystick[i] > (512 - DriveDeadZone)) && (Joystick[i] < (512 + DriveDeadZone))) {
          Joystick[i] = 512;
        }
      }

      // LCD
      lcd.setCursor(0, 0);
      if (DriveEdge == 0) {
        lcd.print("A:  ");
      } else if (DriveEdge == 1) {
        lcd.print("B:  ");
      } else if (DriveEdge == 2) {
        lcd.print("C:  ");
      }
      // LCD

      Serial.write(CMD_Beg);
      if (Joystick[0] != 512) {
        if (Joystick[0] < 512) {            // --- REVERSE DRIVE -----------------------------------
          Serial.write(B01100000 | ((DriveEdge & B00000011) << 3));

          JoystickMapped0 = (uint8_t)map(Joystick[0], 0, 512, 255, 0);
          Serial.write(JoystickMapped0);

          // LCD
          lcd.write((byte)1); // down arrow
          lcd.print(" ");
          if (JoystickMapped0 < 10) {
            lcd.print("  ");
          } else if (JoystickMapped0 < 100) {
            lcd.print(" ");
          }
          lcd.print(JoystickMapped0);
          // LCD

        } else {                            // --- FORWARD DRIVE -----------------------------------
          Serial.write(B01100100 | ((DriveEdge & B00000011) << 3));
          JoystickMapped0 = (uint8_t)map(Joystick[0], 512, 1024, 0, 255);
          Serial.write(JoystickMapped0);

          // LCD
          lcd.write((byte)0); // up arrow
          lcd.print(" ");
          if (JoystickMapped0 < 10) {
            lcd.print("  ");
          } else if (JoystickMapped0 < 100) {
            lcd.print(" ");
          }
          lcd.print(JoystickMapped0);
          // LCD

        }

        if (Joystick[1] == 512) {           // --- NO TURN -----------------------------------------
          Serial.write((uint8_t)0);

          // LCD
          lcd.print("        ");
          // LCD

        } else {                            // --- TURN --------------------------------------------
          JoystickMapped1 = (int8_t)map(Joystick[1], 0, 1024, -127, 127);
          Serial.write(JoystickMapped1);

          // LCD
          lcd.print("  ");
          if (JoystickMapped1 > 0) {        // --- TURN RIGHT --------------------------------------
            lcd.print((char) 126); // right arrow
            lcd.print(" ");
            if (JoystickMapped1 < 10) {
              lcd.print("  ");
            } else if (JoystickMapped1 < 100) {
              lcd.print(" ");
            }
            lcd.print(JoystickMapped1);

          } else {                          // --- TURN LEFT ---------------------------------------
            lcd.print((char) 127); // left arrow
            lcd.print(" ");
            if (JoystickMapped1 > -10) {
              lcd.print("  ");
            } else if (JoystickMapped1 > -100) {
              lcd.print(" ");
            }
            lcd.print(abs(JoystickMapped1));
          }
          // LCD

        }

      } else if (Joystick[2] != 512) {      // --- TWIST -------------------------------------------
        Serial.write(B01000111);
        JoystickMapped2 = (int8_t)map(Joystick[2], 0, 1024, 127, -127);
        Serial.write(JoystickMapped2);
        Serial.write(JoystickMapped2);
        Serial.write(JoystickMapped2);

        // LCD
        if (JoystickMapped2 >= 0) {
          lcd.write((byte)2); // twist left
        } else {
          lcd.write((byte)3); // twist right
        }
        if (abs(JoystickMapped2) < 10) {
          lcd.print("  ");
        } else if (abs(JoystickMapped2) < 100) {
          lcd.print(" ");
        }
        lcd.print(" ");
        lcd.print(abs(JoystickMapped2));
        lcd.print("        ");
        // LCD

      } else {                              // --- NO DRIVE ----------------------------------------
        Serial.write(B01000111);
        Serial.write(B00000000);
        Serial.write(B00000000);
        Serial.write(B00000000);

        // LCD
        lcd.print("              ");
        // LCD

      }
      Serial.write(CMD_End);

    } else if (Serial.available()) {        // --- for debugging -----------------------------------
      uint8_t conRead = Serial.read();
      if (conRead == 0x0F) {
        while (!Serial.available());
        uint8_t bytRead1 = Serial.read();
        //          while (!Serial.available());
        //          uint8_t bytRead2 = Serial.read();
        lcd.setCursor(0, 0);
        lcd.print(((int16_t)(bytRead1)));
        lcd.print("                ");
      }
    } else {
      // LCD
      lcd.setCursor(0, 0);
      lcd.print("                ");
      // LCD

    }

    CMD_UpdateFlag = false;
  }
}


/* ********************************************************************************************** */
/* *** TIMER2 OVERFLOW, (50 Hz) ***************************************************************** */
/* ********************************************************************************************** */
ISR(TIMER2_OVF_vect) {
  /* Count overflows up to MAX to generate specific frequency */
  Timer2OverflowCounter = Timer2OverflowCounter + 1;
  if (Timer2OverflowCounter >= TIMER2_COUNT) {
    TCNT2 = TIMER2_PRELOAD;               // Preload the timer
    TIFR2 = 0x00;                         // Reset overflow register
    Timer2OverflowCounter = 0;            // Reset overflow counter

    // CMD counter eval
    CMD_UpdateCounter++;
    if (CMD_UpdateCounter >= CMD_UpdateTrigger) {
      CMD_UpdateFlag = true;
      CMD_UpdateCounter = 0;
    }

    // drive edge debounce delay (of sorts)
    if (DriveEdgeDelayCount < DriveEdgeDelay) {
      DriveEdgeDelayCount++;
    }

    // coupling 'open' delay
    for (int k = 0; k <= 2; k++) {
      if (CouplFlag[k]) {
        CouplDelayCount[k]++;
        if (CouplDelayCount[k] >= CouplDelay) {
          CouplFlag[k] = false;
          CouplDelayCount[k] = 0;
        }
      }
    }
  }
}


/* ********************************************************************************************** */
/* *** INFO ************************************************************************************* */
/* ********************************************************************************************** */
/*  --- Timer2 ---
   Timer1 is used by softSerial.h library.
   We are using an 8-bit timer (timer2) on an 16 MHz chip with a 1024 prescaler so there will be
   (fclk / prescaler) / maxValue = (16*10^6 / 1024) / 256 = 61.035 ticks per second, we want
   50 Hz so: 61.035 / 50 = 1.2207 is the number of overflows required for 50 Hz, after
   the second overflow, there are 0.2207 * 256 = 56 ticks so we need to set the timer to
   256 - 56 = 200 every time and wait for the forth overflow. */
