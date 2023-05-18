#include <Arduino.h>
#include "motors.h"
#include "PS2_controller.h"

/*
 * A better version of MakerBotwPS2 example used with makerbot BANHMI hardware platform
 * This version simplify the joystick driving method, where the functionality stay the same.
 * Added new controling menthod, switch driving mode by pressing SELECT button
 * Added nitro like speed booster when holding L2
 * 
 * Writen by Tu Dang - Makerviet
 */

/*
  This version is modified internally by STEMIST MSE 2023 Team.
  Written by B4iter - STEMIST MSE Gen 5
*/
// define motor 3 and 4.
#define PWM_CHANNEL5 12
#define PWM_CHANNEL6 13
#define PWM_CHANNEL7 14
#define PWM_CHANNEL8 15
//define servo pinOut.
#define Servo_1_pin 7
#define Servo_2_pin 6
#define Servo_3_pin 5
#define Servo_4_pin 4
#define Servo_5_pin 3
#define Servo_6_pin 2

#define SERVOMIN  70  // Minimum value
#define SERVOMAX  520  // Maximum value

// define desired servo position, it must be configured manually and applied before duty cycle implementation.
int servo1_pos,servo2_pos,servo3_pos,servo4_pos,servo5_pos,servo6_pos;
// duty cycle implementation.
int servo1 = map(servo1_pos, 0, 180, SERVOMIN, SERVOMAX);

void setup()
{
  Serial.begin(115200);
  initMotors();
  setupPS2controller();
  Serial.println("Done setup!");
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  PS2control();
  delay(50);
}