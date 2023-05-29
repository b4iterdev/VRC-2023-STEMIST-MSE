#include <PS2X_lib.h>

PS2X ps2x; // create PS2 Controller Class object

#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

#define SPEED 4095
#define TURNING_FACTOR 1

void setupPS2controller()
{
  int err = -1;
  while (err != 0)
  {
    err = ps2x.config_gamepad(PS2_CLK, PS2_CMD, PS2_SEL, PS2_DAT, true, true);
  }
}
bool PS2control()
{

  int nJoyX = 128 - ps2x.Analog(PSS_RX); // read x-joystick
  int nJoyY = 128 - ps2x.Analog(PSS_LY); // read y-joystick
  int nMotMixL;                          // Motor (left) mixed output
  int nMotMixR;                          // Motor (right) mixed output

  bool temp = (nJoyY * nJoyX > 0);
  if (nJoyX) // Turning
  {
    nMotMixL = -nJoyX + (nJoyY * temp);
    nMotMixR = nJoyX + (nJoyY * !temp);
  }
  else // Forward or Reverse
  {
    nMotMixL = nJoyY;
    nMotMixR = nJoyY;
  }

  // Serial.print(F("Calculated value from joystick: "));
  // Serial.print(nMotMixL);
  // Serial.print("\t");
  // Serial.println(nMotMixR);
  int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

  if (nMotMixR > 0)
  {
    c3 = nMotMixR;
    c3 = map(c3, 0, 128, 0, SPEED);
  }

  else if (nMotMixR < 0)
  {
    c4 = abs(nMotMixR) + 1;
    c4 = map(c4, 0, 128, 0, SPEED);
  }

  if (nMotMixL > 0)
  {
    c1 = nMotMixL;
    c1 = map(c1, 0, 128, 0, SPEED);
  }
  else if (nMotMixL < 0)
  {
    c2 = abs(nMotMixL)+1;
    c2 = map(c2, 0, 128, 0, SPEED);
  }
  setPWMMotors(c1, c2, c3, c4);
  return 1;
}
