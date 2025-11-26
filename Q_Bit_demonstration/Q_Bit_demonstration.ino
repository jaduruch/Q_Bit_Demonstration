/***********************************************************************
  Q-bit simulation

  @author                 Jana Ruch
  @version                0.1

  @brief 
  This Programm controlls 3 Stepper Motors to simulate a Bloch Sphere

  Inpired by: Quantum Amsterdam

***********************************************************************/

// -- librarys ---------------------------------------------------------
#include <Stepper.h>
#include <math.h>

// -- defines ----------------------------------------------------------
#define PIN_BUTTON_X 22
#define PIN_BUTTON_Y 23

#define STEPS_PER_REVOLUTION 2048       // change this to fit the number of steps per revolution
#define ROLE_PER_MINUTE 15              // Adjustable range of 28BYJ-48 stepper is 0~17 rpm

bool ButtonX;
bool ButtonY;
bool OldButtonX;
bool OldButtonY;

// -- initialize the stepper ------------------------------------------- 
Stepper Stepper1(STEPS_PER_REVOLUTION, 16, 17, 18, 19);     // IN1, IN3, IN2, IN4
Stepper Stepper2(STEPS_PER_REVOLUTION, 15, 0, 2, 4);        // IN1, IN3, IN2, IN4

void setup() 
{
  Stepper1.setSpeed(ROLE_PER_MINUTE);
  Stepper2.setSpeed(ROLE_PER_MINUTE);
  Serial.begin(9600);                                     // initialize the serial port

  // initialize pins
  pinMode(PIN_BUTTON_X, INPUT_PULLUP);
  pinMode(PIN_BUTTON_Y, INPUT_PULLUP);
}

void loop() 
{  
  // step one revolution  in one direction:
  /*Serial.println("clockwise");
  Stepper1.step(STEPS_PER_REVOLUTION);
  delay(500);

  // step one revolution in the other direction:
  Serial.println("counterclockwise");
  Stepper1.step(-STEPS_PER_REVOLUTION);
  delay(500);*/
  ButtonX = digitalRead(PIN_BUTTON_X);
  ButtonY = digitalRead(PIN_BUTTON_Y);

  if(ButtonX > OldButtonX)
  {
    Serial.println("Rotation in X");
    // 1: -0.5; 2: 1; 3: -0.5
    Stepper1.step(-STEPS_PER_REVOLUTION/2);
    Stepper2.step(STEPS_PER_REVOLUTION);
  }

  // Y
  // 1: -sqrt3 / 2; 2: 0; 3: sqrt3 / 2
  if(ButtonY > OldButtonY)
  {
    Serial.println("Rotation in Y");
    // 1: -sqrt3 / 2; 2: 0; 3: sqrt3 / 2
    Stepper2.step(-STEPS_PER_REVOLUTION * pow(3, 0.5)/2); // -sqrt3 / 2
    Stepper2.step(STEPS_PER_REVOLUTION*0);
  }

  // Z
  // 1: 1; 2: 1; 3: 1

  OldButtonX = ButtonX;
  OldButtonY = ButtonY;
  delay(10);
}
