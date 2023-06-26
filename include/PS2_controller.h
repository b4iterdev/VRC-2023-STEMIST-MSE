#include <PS2X_lib.h>
PS2X ps2x; // create PS2 Controller Class object

// calibration for different kinds of PS2 controller, this value only suitable for the PS2 controller comes with VRC2023 K12 Maker kit 
#define X_JOY_CALIB 127
#define Y_JOY_CALIB 128

#define PS2_DAT 12 // MISO  19
#define PS2_CMD 13 // MOSI  23
#define PS2_SEL 15 // SS     5
#define PS2_CLK 14 // SLK   18

#define TOP_SPEED 4095
#define NORM_SPEED 3276
#define PRECISE_SPEED 2048
#define TURNING_FACTOR 1

int speed = NORM_SPEED;
int c1 = 0, c2 = 0, c3 = 0, c4 = 0;

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
  if (ps2x.Button(PSB_R1)) {
    speed = TOP_SPEED;
  } else if (ps2x.Button(PSB_L1)) {
    speed = PRECISE_SPEED;
  } else {
    speed = NORM_SPEED;
  }
  int nJoyX = X_JOY_CALIB - ps2x.Analog(PSS_RX); // read x-joystick
  int nJoyY = Y_JOY_CALIB - ps2x.Analog(PSS_LY); // read y-joystick
  int nMotMixL;                          // Motor (left) mixed output
  int nMotMixR;                          // Motor (right) mixed output
  
  if(nJoyX == -1 && nJoyY == 0) // in case of lost connection with the wireless controller, only used in VRC2023 PS2 wireless controller 
  {
    setPWMMotors(0, 0, 0, 0);
    return 0;
  }

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

  #ifdef DEBUG
  Serial.print(F("Calculated value from joystick: "));
  Serial.print(nMotMixL);
  Serial.print("\t");
  Serial.println(nMotMixR);
  #endif
  c1 = 0, c2 = 0, c3 = 0, c4 = 0;
  if (nMotMixR > 0)
  {
    c3 = nMotMixR;
    c3 = map(c3, 0, 128, 0, speed);
  }

  else if (nMotMixR < 0)
  {
    c4 = abs(nMotMixR);
    c4 = map(c4, 0, 128, 0, speed);
  }

  if (nMotMixL > 0)
  {
    c1 = nMotMixL;
    c1 = map(c1, 0, 128, 0, speed);
  }
  else if (nMotMixL < 0)
  {
    c2 = abs(nMotMixL);
    c2 = map(c2, 0, 128, 0, speed);
  }
  setPWMMotors(c1, c2, c3, c4);
  return 1;
}

String getMotorOutput(unsigned int num) {
  switch (num) {
    case 1: 
    return String(c1);
    break;
    case 2:
    return String(c2);
    break;
    case 3:
    return String(c3);
    break;
    case 4:
    return String(c4);
    break;
    case 5:
    unsigned int perMaxMotor = map(speed,0,4095,0,100);
    return String(perMaxMotor);
  } 
  return "Error";
}