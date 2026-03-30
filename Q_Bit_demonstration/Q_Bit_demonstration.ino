/***********************************************************************
  Q-bit simulation – Gate Selection Display

  @author  Jana Ruch
  @version 0.2

  Controls stepper motors and shows selectable quantum gates
  on an SH1106 OLED display.

  SCL: GPIO 22
  SDA: GPIO 21
***********************************************************************/

// -- libraries --------------------------------------------------------
#include <Stepper.h>
#include <math.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// -- pin definitions --------------------------------------------------
#define PIN_JOYSTICK_Y 14
#define PIN_BUTTON 12
 #define PIN_RELAY 32

// -- stepper definitions ---------------------------------------------
#define STEPS_PER_REVOLUTION 2048
#define ROLE_PER_MINUTE 15

// -- display definitions ---------------------------------------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1

Adafruit_SH1106G display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// -- gates ------------------------------------------------------------
char gates[] = {'X', 'Y', 'Z', 'H', 'R'};
const int gateCount = sizeof(gates) / sizeof(gates[0]);
int gateIndex = 0;

// -- button states ----------------------------------------------------
unsigned int JoystickY;
unsigned int OldJoystickY;
bool Button;
bool OldButton;

// -- steppers ---------------------------------------------------------
Stepper Stepper1(STEPS_PER_REVOLUTION, 16, 17, 18, 19);       // IN1, IN3, IN2, IN4
Stepper Stepper2(STEPS_PER_REVOLUTION, 15, 0, 2, 4);          // IN1, IN3, IN2, IN4
Stepper Stepper3(STEPS_PER_REVOLUTION, 27, 26, 25, 5);          // IN1, IN3, IN2, IN4

// -------------------------------------------------------------------
void setup()
{
  Serial.begin(9600);

  // Buttons
  pinMode(PIN_JOYSTICK_Y, INPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);

  // Steppers
  Stepper1.setSpeed(ROLE_PER_MINUTE);
  Stepper2.setSpeed(ROLE_PER_MINUTE);
  Stepper3.setSpeed(ROLE_PER_MINUTE);

  // I2C
  Wire.begin(21, 22);

  // Display
  if (!display.begin(0x3C, true)) {
    Serial.println("SH1106 nicht gefunden");
    while (1);
  }

  display.setTextColor(SH110X_WHITE);
  display.clearDisplay();
  display.display();
}

// -------------------------------------------------------------------
void loop()
{
  // -- read buttons --------------------------
  JoystickY = analogRead(PIN_JOYSTICK_Y);
  // Serial.print("Joystick pos: ");Serial.println(JoystickY);
  Button = !digitalRead(PIN_BUTTON);

  // -------- BUTTON X (Gate wechseln) --------
  if (JoystickY > 2800 && OldJoystickY <= 2800) {
    gateIndex = (gateIndex + 1) % gateCount;
    // Serial.print("Gate: ");
    // Serial.println(gates[gateIndex]);
  }

  if (JoystickY < 1200 && OldJoystickY >= 1200) {
    gateIndex = (gateIndex - 1 + gateCount) % gateCount;
    // Serial.print("Gate: ");
    // Serial.println(gates[gateIndex]);
  }
  

  // -------- DISPLAY --------
  drawGateSelection();

  if(Button > OldButton)
  {
    Serial.println("Button!");
    digitalWrite(PIN_RELAY, HIGH);
    switch (gates[gateIndex]) 
    {

      case 'X':           // X
          Serial.println("Rotation in X");
          // 1: -0.5; 2: 1; 3: -0.5
          /*Stepper1.step(-STEPS_PER_REVOLUTION/2);
          Stepper2.step(STEPS_PER_REVOLUTION);
          Stepper3.step(-STEPS_PER_REVOLUTION/2);*/
          moveSteppers(
            -STEPS_PER_REVOLUTION / 2,
            STEPS_PER_REVOLUTION,
            -STEPS_PER_REVOLUTION / 2
          );
      break;

      case 'Y':
          Serial.println("Rotation in Y");
          // 1: -sqrt3 / 2; 2: 0; 3: sqrt3 / 2
          /*Stepper1.step(-STEPS_PER_REVOLUTION * pow(3, 0.5)/2); // -sqrt3 / 2
          Stepper2.step(STEPS_PER_REVOLUTION*0);
          Stepper3.step(STEPS_PER_REVOLUTION * pow(3, 0.5)/2); // sqrt3 / 2*/
          moveSteppers(
            -STEPS_PER_REVOLUTION * 0.866, // sqrt(3)/2
            0,
            STEPS_PER_REVOLUTION * 0.866
          );
      break;

      case 'Z':
          Serial.println("Rotation in Z");
          // 1: 1; 2: 1; 3: 1
          for(int i = 0; i < STEPS_PER_REVOLUTION; i++)
          {
            Stepper1.step(1);
            Stepper2.step(1);
            Stepper3.step(1);
          }
          
      break;

      case 'H':
          Serial.println("Rotation in H");
          // 1: -sqrt3 / 4; 2: 0; 3: sqrt3 / 4
          Stepper1.step(STEPS_PER_REVOLUTION);
          Stepper2.step(STEPS_PER_REVOLUTION);
          Stepper3.step(STEPS_PER_REVOLUTION);
      break;

      case 'R':
          Serial.println("Rotation in R");
          // 1: 1; 2: 1; 3: 1
          Stepper1.step(STEPS_PER_REVOLUTION);
          Stepper2.step(STEPS_PER_REVOLUTION);
          Stepper3.step(STEPS_PER_REVOLUTION);
      break;

      default:
          Serial.println("error");
    }
    digitalWrite(PIN_RELAY, LOW);
    
  }
  // Serial.print("prev: ");Serial.print(OldButton);Serial.print(" new: ");Serial.println(Button);

  OldJoystickY = JoystickY;
  OldButton = Button;

  delay(10);  // kleines Debounce
}

// -------------------------------------------------------------------
void drawGateSelection()
{
  display.clearDisplay();

  const int y = 22;

  const int xLeft   = 28;
  const int xCenter = 56;
  const int xRight  = 92;

  int leftIndex = (gateIndex + 1) % gateCount;
  int rightIndex  = (gateIndex - 1 + gateCount) % gateCount;

  // LEFT (klein)
  display.setTextSize(2);
  display.setCursor(xLeft, y + 6);
  display.print(gates[leftIndex]);

  // CENTER (GROSS = aktiv)
  display.setTextSize(3);
  display.setCursor(xCenter, y);
  display.print(gates[gateIndex]);

  // RIGHT (klein)
  display.setTextSize(2);
  display.setCursor(xRight, y + 6);
  display.print(gates[rightIndex]);

  display.display();
}

void moveSteppers(long s1, long s2, long s3) {
  long maxSteps = max(abs(s1), max(abs(s2), abs(s3)));

  float acc1 = 0, acc2 = 0, acc3 = 0;

  float inc1 = (float)abs(s1) / maxSteps;
  float inc2 = (float)abs(s2) / maxSteps;
  float inc3 = (float)abs(s3) / maxSteps;

  int dir1 = (s1 >= 0) ? 1 : -1;
  int dir2 = (s2 >= 0) ? 1 : -1;
  int dir3 = (s3 >= 0) ? 1 : -1;

  for (long i = 0; i < maxSteps; i++) {
    acc1 += inc1;
    acc2 += inc2;
    acc3 += inc3;

    if (acc1 >= 1) {
      Stepper1.step(dir1);
      acc1 -= 1;
    }
    if (acc2 >= 1) {
      Stepper2.step(dir2);
      acc2 -= 1;
    }
    if (acc3 >= 1) {
      Stepper3.step(dir3);
      acc3 -= 1;
    }

    // delayMicroseconds(1000); // Geschwindigkeit anpassen!
  }
}
