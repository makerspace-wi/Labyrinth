
/*-------------------------------------------------------------------------
   The Amazing Labyrinth project

   The components needed are: 
    - One Microcontroller AZ-Nano V3-Board mit USB-C.
    - One KY-023 Joystick module.
    - Two MG995 Micro Digital Servo Motor.
    - Jumper Wire cable 3 x 40 pcs. 20 cm M2M / F2M / F2F.
    - One MB102 Breadboard Power Supply Adapter  3.3V/5V 
  
    Miguel Torres Gordo                                                                               
    Getafe (Madrid) - ESPAÑA          Last revision 09-03-2025
	Michael (Wi) - Germany 			  Last revision 03.09.2025
    
-------------------------------------------------------------------------*/
/*----------------------
   Public Constants Tone
 -----------------------*/
#define NOTE_B0 31
#define NOTE_C1 33
#define NOTE_CS1 35
#define NOTE_D1 37
#define NOTE_DS1 39
#define NOTE_E1 41
#define NOTE_F1 44
#define NOTE_FS1 46
#define NOTE_G1 49
#define NOTE_GS1 52
#define NOTE_A1 55
#define NOTE_AS1 58
#define NOTE_B1 62
#define NOTE_C2 65
#define NOTE_CS2 69
#define NOTE_D2 73
#define NOTE_DS2 78
#define NOTE_E2 82
#define NOTE_F2 87
#define NOTE_FS2 93
#define NOTE_G2 98
#define NOTE_GS2 104
#define NOTE_A2 110
#define NOTE_AS2 117
#define NOTE_B2 123
#define NOTE_C3 131
#define NOTE_CS3 139
#define NOTE_D3 147
#define NOTE_DS3 156
#define NOTE_E3 165
#define NOTE_F3 175
#define NOTE_FS3 185
#define NOTE_G3 196
#define NOTE_GS3 208
#define NOTE_A3 220
#define NOTE_AS3 233
#define NOTE_B3 247
#define NOTE_C4 262
#define NOTE_CS4 277
#define NOTE_D4 294
#define NOTE_DS4 311
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_FS4 370
#define NOTE_G4 392
#define NOTE_GS4 415
#define NOTE_A4 440
#define NOTE_AS4 466
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_CS5 554
#define NOTE_D5 587
#define NOTE_DS5 622
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_FS5 740
#define NOTE_G5 784
#define NOTE_GS5 831
#define NOTE_A5 880
#define NOTE_AS5 932
#define NOTE_B5 988
#define NOTE_C6 1047
#define NOTE_CS6 1109
#define NOTE_D6 1175
#define NOTE_DS6 1245
#define NOTE_E6 1319
#define NOTE_F6 1397
#define NOTE_FS6 1480
#define NOTE_G6 1568
#define NOTE_GS6 1661
#define NOTE_A6 1760
#define NOTE_AS6 1865
#define NOTE_B6 1976
#define NOTE_C7 2093
#define NOTE_CS7 2217
#define NOTE_D7 2349
#define NOTE_DS7 2489
#define NOTE_E7 2637
#define NOTE_F7 2794
#define NOTE_FS7 2960
#define NOTE_G7 3136
#define NOTE_GS7 3322
#define NOTE_A7 3520
#define NOTE_AS7 3729
#define NOTE_B7 3951
#define NOTE_C8 4186
#define NOTE_CS8 4435
#define NOTE_D8 4699
#define NOTE_DS8 4978
// End Tones-----------------

#define Maxi 1023             // Value AD converter
#define Mini 0
#define Midi 511
#define DiffGrd 2             // Value at 02.09.2025 for difference degree +-
#define DiffLight 100         // Value at 02.09.2025 for difference value +-
#define MidGrdX 73            // Value at 02.09.2025 for zero X position
#define MidGrdY 84            // Value at 02.09.2025 for zero Y position

#define AdrDiff 5             // EEProm Adress
#define AdrGrdX 6
#define AdrGrdY 7
#define AdrLight 8

#define ServoX  3             // [2] Servo X-Achse
#define ServoY  2             // [3] Servo Y-Achse
#define Buzzer  8             // Buzzer

#define X_pin  A1             // [A0] Joystick X-Achse 
#define Y_pin  A0             // [A1] Joystick Y-Achse
#define B_pin  A2             // Joystick Button
#define LS_pin A3             // Photo sensor

#define dlyRead 20            // [20]ms Delay time between readings.

#include <Arduino.h>
#include <EEPROM.h>

/****************  Library and object for the servo  ****************/
#include <Servo.h>            // Library required for the servomotor.

Servo servo_x_axis_movement;  // Creation of a Servomotor object for x-axis movement.
Servo servo_y_axis_movement;  // Creation of a Servomotor object for y-axis movement.

/********************************************* variables  **********/
int val_x_axis;               // Variable to store the X-axis values.
int val_y_axis;               // Variable to store the Y-axis values.

int grd_x_axis;               // grad to store the X-axis values.
int grd_y_axis;               // grad to store the Y-axis values.

int LS_light;                 // Value of photo sensor
int LS_prev;                  // previous value of photo sensor

int countBoLo;                // counter for button = low

int x_axis_max_value = Maxi;  // Maximum constant value of the joystick X-axis (left side position).
int x_axis_min_value = Mini;  // Minimum constant value of the joystick X-axis (right side position).
int y_axis_max_value = Maxi;  // Maximum constant value of the joystick Y-axis (upper position).
int y_axis_min_value = Mini;  // Minimum constant value of the joystick Y-axis (lower position).

String inStr = "";            // a string to hold incoming data
unsigned int val;

// notes in the melody:
int melody[] = {
//   NOTE_C6, NOTE_C5, NOTE_C5, NOTE_C5, NOTE_B5, 0, 0, 0  // Fanfar?
 NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4 // orgi
//  NOTE_C6, NOTE_G5, NOTE_G5, NOTE_A5, NOTE_G5, 0, NOTE_B5, NOTE_C6 // higher
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
//   1, 8, 8, 8, 1, 4, 4, 4
 4, 8, 8, 4, 4, 4, 4, 4
};

// FUNCTIONS ------------------------------------
int getNum(String strNum) // Check if realy numbers
{
  strNum.trim();
  for (byte i = 0; i < strNum.length(); i++)
  {
    if (!isDigit(strNum[i])) 
    {
      return 0;
    }
  }
  return strNum.toInt();
}
// End Funktions --------------------------------

// Funktions Serial Input (Event) ---------------
void evalSerialData()
{
      inStr.toUpperCase();
      if (inStr.startsWith("G") && inStr.length() > 1)
      {
            val = getNum(inStr.substring(1));
            if (val >= 1 && val <= 10)
            {
                  EEPROM.update(AdrDiff, val);
                  delay(10);
                  Serial.print(F("Diff G:"));
                  Serial.println(EEPROM.read(AdrDiff));
            }
            else
            {
                  Serial.println("?" + inStr);
                  Serial.print(F("Diff G:"));
                  Serial.println(EEPROM.read(AdrDiff));
            }
      }
      else if (inStr.startsWith("L") && inStr.length() > 1)
      {
            val = getNum(inStr.substring(1));
            if (val >= 25 && val <= 250)
            {
                  EEPROM.update(AdrLight, val);
                  delay(10);
                  Serial.print(F("Diff L:"));
                  Serial.println(EEPROM.read(AdrLight));
            }
            else
            {
                  Serial.println("?" + inStr);
                  Serial.print(F("Diff L:"));
                  Serial.println(EEPROM.read(AdrLight));
            }
      }
      else if (inStr.startsWith("X") && inStr.length() > 2)
      {
            val = getNum(inStr.substring(1));
            if (val >= 60 && val <= 120)
            {
                  EEPROM.update(AdrGrdX, val);
                  delay(10);
                  Serial.print(F("NullX:"));
                  Serial.println(EEPROM.read(AdrGrdX));
            }
            else
            {
                  Serial.println("?" + inStr);
                  Serial.print(F("NullX:"));
                  Serial.println(EEPROM.read(AdrGrdX));
            }
      }
      else if (inStr.startsWith("Y") && inStr.length() > 2)
      {
            val = getNum(inStr.substring(1));
            if (val >= 60 && val <= 120)
            {
                  EEPROM.update(AdrGrdY, val);
                  delay(10);
                  Serial.print(F("NullY:"));
                  Serial.println(EEPROM.read(AdrGrdY));
            }
            else
            {
                  Serial.println("?" + inStr);
                  Serial.print(F("NullY:"));
                  Serial.println(EEPROM.read(AdrGrdY));
            }
      }
      else if (inStr.startsWith("B") && inStr.length() == 1)
      {
            Serial.print(F("Butt:"));
            Serial.println(digitalRead(B_pin));
      }
}
// End Funktions Serial Input -------------------

void setup()
{
      Serial.begin(115200);
      inStr.reserve(10);    // reserve for instr serial input
  
      pinMode(Buzzer, OUTPUT);
      digitalWrite(Buzzer, LOW);

      countBoLo = 0;
	  
      servo_x_axis_movement.attach(ServoX);    // Pin of the microcontroller where the servomotor for the X-axis movement is connected.  
      servo_y_axis_movement.attach(ServoY);    // Pin of the microcontroller where the servomotor for the Y-axis movement is connected.

      if (EEPROM.read(AdrDiff) < 1 || EEPROM.read(AdrDiff) > 10)
      {
            // Test ---------------------
            // Serial.println("Set first ");
            // --------------------------
            EEPROM.update(AdrDiff, DiffGrd);
            delay(10);
            EEPROM.update(AdrGrdX, MidGrdX);
            delay(10);
            EEPROM.update(AdrGrdY, MidGrdY);
            delay(10);
            EEPROM.update(AdrLight, DiffLight);
            delay(10);
      } 
      LS_prev = analogRead(LS_pin);            // Define start value

      Serial.println("Set Null: 'X00'; 'Y00'; Diff: 'G0'; 'L0'");
}

void loop()
{
      if (Serial.available() > 0)
      {
            char inChar = (char)Serial.read();
            if (inChar == '\x0d')
            {
                  evalSerialData();
                  inStr = "";
            }
            else if (inChar != '\x0a')
            {
                  inStr += inChar;
            }
      }

      val_x_axis = analogRead(X_pin);
      val_y_axis = analogRead(Y_pin);

      if (val_x_axis > Midi)
      { // X-axis movement to left side from center (center value 511).
            x_axis_max_value = Maxi - val_x_axis + Midi;         // The maximum value constant for the X-axis will be modified with the new value.
            x_axis_min_value = Mini;                             // The constant of the minimum value of the X-axis does not change.
      } 
      else if (val_x_axis < Midi)
      { // X-axis movement to right side from center (center value 511).
            x_axis_max_value = Maxi;                             // The constant of the maximum value of the X-axis does not change.
            x_axis_min_value = Midi - val_x_axis;                // The minimum value constant for the X-axis will be modified with the new value.
      }
      
      if (val_y_axis > Midi)
      { // Y-axis movement upward from the center (center value 511).
            y_axis_max_value = Maxi - val_y_axis + Midi;         // The maximum value constant for the Y-axis will be modified by the new value.
            y_axis_min_value = Mini;                             // The constant of the minimum value of the Y-axis does not change.
      } 
      else if (val_y_axis < Midi) 
      { // Downward movement of the Y-axis from the center (center value 511).
            y_axis_max_value = Maxi;                             // The constant of the maximum value of the Y-axis does not change.
            y_axis_min_value = Midi - val_y_axis;                // The minimum value constant for the Y-axis will be modified with the new value.
      }

      /*****  Position of the servo motors with references to the values obtained in the conditionals  *****/
      // The val_x_axis variable will have the value in degrees to position the X-axis servomotor, with the minimum and maximum values of movement.
      grd_x_axis = map (val_x_axis, x_axis_min_value, x_axis_max_value, EEPROM.read(AdrGrdX) - EEPROM.read(AdrDiff), EEPROM.read(AdrGrdX) + EEPROM.read(AdrDiff));
      servo_x_axis_movement.write (grd_x_axis);                  // X-axis servomotor positioning.
      // The val_y_axis variable will have the value in degrees to position the Y-axis servomotor, with the minimum and maximum values of movement.
      grd_y_axis = map (val_y_axis, y_axis_min_value, y_axis_max_value, EEPROM.read(AdrGrdY) - EEPROM.read(AdrDiff), EEPROM.read(AdrGrdY) + EEPROM.read(AdrDiff));
      servo_y_axis_movement.write (grd_y_axis);                  // X-axis servomotor positioning.

      LS_light = analogRead(LS_pin); 
      if (abs(LS_prev - LS_light) > EEPROM.read(AdrLight)) 
      {
            // iterate over the notes of the melody:
            for (int thisNote = 0; thisNote < 8; thisNote++)
            {
                  // to calculate the note duration, take one second divided by the note type.
                  //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
                  int noteDuration = 1000 / noteDurations[thisNote];
                  tone(Buzzer, melody[thisNote], noteDuration);

                  // to distinguish the notes, set a minimum time between them.
                  // the note's duration + 30% seems to work well:
                  int pauseBetweenNotes = noteDuration * 1.30;
                  delay(pauseBetweenNotes);
                  // stop the tone playing:
                  noTone(Buzzer);
            }
            // Test --------------------
            // Serial.print(" Licht:");
            // Serial.println(LS_light);
            // Serial.print(" p:");
            // Serial.println(LS_prev);
            // -------------------------
      }
      LS_prev = analogRead(LS_pin); 

      if (digitalRead(B_pin) == LOW )
      {
            if (countBoLo >= 5000 / dlyRead)
            {
                  tone(Buzzer, NOTE_C4, 1000 / 8);
                  EEPROM.update(AdrGrdX, grd_x_axis);
                  delay(10);
                  EEPROM.update(AdrGrdY, grd_y_axis);
                  delay(152);
                  noTone(Buzzer);
                  tone(Buzzer, NOTE_B3, 1000 / 4);
                  delay(325);
                  noTone(Buzzer);
                  countBoLo = 0;
                  // Test ---------------------
                  // Serial.print(F("NullX:"));
                  // Serial.print(grd_x_axis);
                  // Serial.print(F(";"));
                  // Serial.println(EEPROM.read(AdrGrdX));
                  // Serial.print(F("NullY:"));
                  // Serial.print(grd_y_axis);
                  // Serial.print(F(";"));
                  // Serial.println(EEPROM.read(AdrGrdY));
                  // --------------------------

            }
            ++countBoLo;
      }
      // Test --------------------
      // Serial.print("X ");
      // Serial.print(val_x_axis);
      // Serial.print(" ServX ");
      // Serial.print(grd_x_axis);
      // Serial.print(" Y ");
      // Serial.print(val_y_axis);
      // Serial.print(" ServY ");
      // Serial.println(grd_y_axis);
      // Serial.print(" Licht:");
      // Serial.println(LS_light);
      // -------------------------
      delay (dlyRead);
}
