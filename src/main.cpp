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
#define Servo_1 7
#define Servo_2 6
#define Servo_3 5
#define Servo_4 4
#define Servo_5 3
#define Servo_6 2

#define SERVOMIN  70  // Minimum value
#define SERVOMAX  520  // Maximum value

void setup()
{
  Serial.begin(115200);
  initMotors();
  setupPS2controller();
  Serial.println("Done setup!");
}

void servoControl(void) {
    if(ps2x.ButtonPressed(PSB_SQUARE)) {
      pwm.setPWMFreq(50);
      pwm.setPWM(Servo_1,0,80);
      pwm.setPWM(Servo_2,0,440);
      pwm.setPWM(Servo_3,0,80);
      pwm.setPWM(Servo_4,0,440);
    }
    if(ps2x.ButtonPressed(PSB_CROSS)) {
      pwm.setPWM(Servo_1,0,440);
      pwm.setPWM(Servo_2,0,80);
      pwm.setPWM(Servo_3,0,80);
      pwm.setPWM(Servo_4,0,440);
    }
    if(ps2x.ButtonPressed(PSB_L1)) { 
      pwm.setPWM(Servo_3,0,0);
      pwm.setPWM(Servo_4,0,0);
      delay(200);
      pwm.setPWMFreq(1600);
     }
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  PS2control();
  delay(50);
}