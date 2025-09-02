
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
	Michael (Wi) - Germany 			  Last revision 31.08.2025
    
-------------------------------------------------------------------------*/

#define Maxi 1023
#define Mini 0
#define Midi 511
#define DiffGrd 5
#define DiffLight 100
#define MidGrdX 84
#define MidGrdY 77

#define AdrDiff 5
#define AdrGrdX 6
#define AdrGrdY 7
#define AdrLight 8

#define ServoX  2             // Servo X-Achse
#define ServoY  3             // Servo Y-Achse
#define Buzzer  8             // Buzzer


#define X_pin  A0             // Joystick X-Achse
#define Y_pin  A1             // Joystick Y-Achse
#define B_pin  A2             // Joystick Button
#define LS_pin A3             // Lichtschranke

#include <Arduino.h>
#include <EEPROM.h>

/****************  Library and object for the servo  ****************/
#include <Servo.h>                                    // Library required for the servomotor.

Servo servo_x_axis_movement;                          // Creation of a Servomotor object for x-axis movement.
Servo servo_y_axis_movement;                          // Creation of a Servomotor object for y-axis movement.

/********************************************* variables  **********/
int val_x_axis;                                       // Variable to store the X-axis values.
int val_y_axis;                                       // Variable to store the Y-axis values.

int grd_x_axis;                                       // grad to store the X-axis values.
int grd_y_axis;                                       // grad to store the Y-axis values.

int LS_light;
int LS_prev;

int x_axis_max_value = Maxi;                          // Maximum constant value of the joystick X-axis (left side position).
int x_axis_min_value = Mini;                          // Minimum constant value of the joystick X-axis (right side position).
int y_axis_max_value = Maxi;                          // Maximum constant value of the joystick Y-axis (upper position).
int y_axis_min_value = Mini;                          // Minimum constant value of the joystick Y-axis (lower position).

String inStr = "";      // a string to hold incoming data
unsigned int val;

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
            if (val >= 5 && val <= 20)
            {
                  EEPROM.update(AdrDiff, val);
                  delay(5);
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
            if (val >= 10 && val <= 250)
            {
                  EEPROM.update(AdrLight, val);
                  delay(5);
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
                  delay(5);
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
                  delay(5);
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

      servo_x_axis_movement.attach(ServoX);    // Pin of the microcontroller where the servomotor for the X-axis movement is connected.  
      servo_y_axis_movement.attach(ServoY);    // Pin of the microcontroller where the servomotor for the Y-axis movement is connected.

      if (EEPROM.read(AdrDiff) < 5 || EEPROM.read(AdrDiff) > 20)
      {
            EEPROM.update(AdrDiff, DiffGrd);
            delay(5);
            EEPROM.update(AdrGrdX, MidGrdX);
            delay(5);
            EEPROM.update(AdrGrdY, MidGrdY);
            delay(5);
            EEPROM.update(AdrLight, DiffLight);
            delay(5);
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
            tone(Buzzer, 1046);
            delay (600); // mit der Dauer von einer Sekunde
            noTone(Buzzer); // Ausschalten des Tons.
            delay(60);

            tone(Buzzer, 1046);
            delay (180); // mit der Dauer von einer Sekunde
            noTone(Buzzer); // Ausschalten des Tons.
            delay(60);

            tone(Buzzer, 1046);
            delay (180); // mit der Dauer von einer Sekunde
            noTone(Buzzer); // Ausschalten des Tons.
            delay(60);

            tone(Buzzer, 1046);
            delay (180); // mit der Dauer von einer Sekunde
            noTone(Buzzer); // Ausschalten des Tons.
            delay(60);

            tone(Buzzer, 880);
            delay (1200); // mit der Dauer von einer Sekunde
            noTone(Buzzer); // Ausschalten des Tons.

            delay(1000);
      }
      LS_prev = analogRead(LS_pin); 

      if (joystick.getButton() == LOW )
      {
            noTone(Buzzer); // Ausschalten des Tons.
      }

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

      delay (50);  // Waiting time of 50 ms between readings.
}
