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
  This version is heavily modified internally by STEMIST MSE 2023 Team.
    - Disabled nitro-like speed boost and switch driving mode.
    - Add additional motor control and servo control.
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

#define MIN_SERVO 93
#define MAX_SERVO 600
#define MAX_SERVO_RESERVE 200

// define desired servo position, it must be configured manually and applied before duty cycle implementation.
int servo1_pos,servo2_pos,servo3_pos,servo4_pos,servo5_pos,servo6_pos;
// duty cycle implementation.
//int servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);

void setup()
{
  Serial.begin(115200);
  initMotors();
  setupPS2controller();
  Serial.println("Done setup!");
}

/*
  This funtion will make selected PWM Channel to start from 1024 on first stage and full speed when reached 1 second of runtime.
*/
int delayedStart(unsigned int PWM_CHANNEL) {
  long long time = millis();
  if (millis() - time <= 200) {
    pwm.setPWM(PWM_CHANNEL,0,1024);
  } else if (millis() - time >= 200 && millis() - time <= 400) {
    pwm.setPWM(PWM_CHANNEL,0,2048);
  } else if (millis() - time >= 400 && millis() - time <= 1000) {
    pwm.setPWM(PWM_CHANNEL,0,3072);
  } else if (millis() - time >= 1000) {
    pwm.setPWM(PWM_CHANNEL,0,4095);
    return 0;
  } 
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  PS2control();
  if (ps2x.ButtonReleased(PSB_CROSS) || ps2x.ButtonReleased(PSB_CIRCLE)) {
    servo1_pos = 90;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
  } else if (ps2x.Button(PSB_CROSS)) {
    servo1_pos = 0;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
  } else if (ps2x.Button(PSB_CIRCLE)) {
    servo1_pos = 180;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
  } else if (!ps2x.Button(PSB_SELECT) && ps2x.ButtonPressed(PSB_SQUARE)) {
    delayedStart(PWM_CHANNEL5);
    pwm.setPWM(PWM_CHANNEL6,0,0);
  } else if (!ps2x.Button(PSB_SELECT) && ps2x.ButtonPressed(PSB_TRIANGLE)) {
    delayedStart(PWM_CHANNEL7);
    pwm.setPWM(PWM_CHANNEL8,0,0);
  } else if(ps2x.Button(PSB_SELECT) && ps2x.ButtonPressed(PSB_SQUARE)) {
    pwm.setPWM(PWM_CHANNEL5,0,0);
    delayedStart(PWM_CHANNEL6);
  } else if(ps2x.Button(PSB_SELECT) && ps2x.ButtonPressed(PSB_TRIANGLE)) {
    pwm.setPWM(PWM_CHANNEL7,0,0);
    delayedStart(PWM_CHANNEL8);
  } else if (ps2x.ButtonPressed(PSB_L1)) {
    pwm.setPWM(PWM_CHANNEL5,0,0);
    pwm.setPWM(PWM_CHANNEL6,0,0);
  } else if (ps2x.ButtonPressed(PSB_R1)) {
    pwm.setPWM(PWM_CHANNEL7,0,0);
    pwm.setPWM(PWM_CHANNEL8,0,0);
  }
  delay(50);
}