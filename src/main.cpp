#include <Arduino.h>
#include "motors.h"
#include "PS2_controller.h"
#include <DNSServer.h>
#include <ESPUI.h>
#include <Wire.h>

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;
#include <WiFi.h>

const char *ssid = "STEMIST MSE";
const char *password = "stemistclub";

const char *hostname = "stemist.mse";

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

void generalCallback(Control *sender, int type) {

}

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
  ESPUI.setVerbosity(Verbosity::VerboseJSON);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  Serial.print("\n\nTry to connect to existing network");
  {
    uint8_t timeout = 10;
    // Wait for connection, 5s timeout
    do
    {
      delay(500);
      Serial.print(".");
      timeout--;
    } while (timeout && WiFi.status() != WL_CONNECTED);

    // not connected -> create hotspot
    if (WiFi.status() != WL_CONNECTED)
    {
      Serial.print("\n\nCreating hotspot");

      WiFi.mode(WIFI_AP);
      WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
      WiFi.softAP(ssid);

      timeout = 5;

      do
      {
        delay(500);
        Serial.print(".");
        timeout--;
      } while (timeout);
    }
  }

  dnsServer.start(DNS_PORT, "*", apIP);

  Serial.println("\n\nWiFi parameters:");
  Serial.print("Mode: ");
  Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
  Serial.print("IP address: ");
  Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());
  // Debug Panel Begin here. 
  auto maintab = ESPUI.addControl(Tab, "Debug Info", "Debug Info");
  ESPUI.addControl(Label, "Status", "System status: OK", Alizarin, maintab, generalCallback);
  uint16_t PWMMotor1s1 = ESPUI.addControl(ControlType::Slider, "Motor 1 Pin A", getMotorOutput(1), Alizarin, maintab, generalCallback);
  ESPUI.addControl(Min, "", "0", None, PWMMotor1s1);
  ESPUI.addControl(Max, "", "4095", None, PWMMotor1s1);
  uint16_t PWMMotor1s2 = ESPUI.addControl(ControlType::Slider, "Motor 1 Pin B", getMotorOutput(2), Alizarin, maintab, generalCallback);
  ESPUI.addControl(Min, "", "0", None, PWMMotor1s2);
  ESPUI.addControl(Max, "", "4095", None, PWMMotor1s2);
  uint16_t PWMMotor2s1 = ESPUI.addControl(ControlType::Slider, "Motor 2 Pin A", getMotorOutput(3), Alizarin, maintab, generalCallback);
  ESPUI.addControl(Min, "", "0", None, PWMMotor2s1);
  ESPUI.addControl(Max, "", "4095", None, PWMMotor2s1);
  uint16_t PWMMotor2s2 = ESPUI.addControl(ControlType::Slider, "Motor 2 Pin B", getMotorOutput(1), Alizarin, maintab, generalCallback);
  ESPUI.addControl(Min, "", "0", None, PWMMotor2s2);
  ESPUI.addControl(Max, "", "4095", None, PWMMotor2s2);
  uint16_t servo1pos = ESPUI.addControl(ControlType::Slider, "Servo 1", String(servo1_pos), Alizarin, maintab, generalCallback);
  ESPUI.addControl(Min, "", "0", None, servo1pos);
  ESPUI.addControl(Max, "", "180", None, servo1pos);
	//Make sliders continually report their position as they are being dragged.
	ESPUI.sliderContinuous = true;
  ESPUI.begin("STEMIST MSE - VRC 2023 Debug Panel");
  Serial.println("Done setup!");
}


void loop()
{
  dnsServer.processNextRequest();
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