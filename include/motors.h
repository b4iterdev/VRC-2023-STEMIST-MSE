#include <stdio.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

#define MIN_PWM 0
#define MAX_PWM 4095

// PWM channels of pca9685 0-16
#define PWM_CHANNEL1 8
#define PWM_CHANNEL2 9
#define PWM_CHANNEL3 10
#define PWM_CHANNEL4 11

// pwm1 = map(arm_servo1_pos, 0, 180, SERVOMIN, SERVOMAX);

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

void setPWMMotors(int c1, int c2, int c3, int c4)
{
  // setPWM(channel, on_duty_cycle, off_duty_cycle)
  Serial.print(c1);
  Serial.print("\t");
  Serial.print(c2);
  Serial.print("\t");
  Serial.print(c3);
  Serial.print("\t");
  Serial.print(c4);
  Serial.println();

  pwm.setPWM(PWM_CHANNEL1, c1, MAX_PWM - c1);
  pwm.setPWM(PWM_CHANNEL2, c2, MAX_PWM - c2);
  pwm.setPWM(PWM_CHANNEL3, c3, MAX_PWM - c3);
  pwm.setPWM(PWM_CHANNEL4, c4, MAX_PWM - c4);
}

void initMotors()
{
  Wire.begin(); // SDA, SCL,400000);
  pwm.begin();
  pwm.setOscillatorFrequency(27000000);
  pwm.setPWMFreq(1600);
  // pwm.setPWMFreq(50); if controlling servo is needed.
  Wire.setClock(400000);
  setPWMMotors(0, 0, 0, 0);
}
