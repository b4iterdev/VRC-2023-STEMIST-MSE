#include <Arduino.h>
#include "motors.h"
#include "PS2_controller.h"
#include <DNSServer.h>
#include <ESPUI.h>
#include <Wire.h>
#include <WiFi.h>
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
    - Disabled switch driving mode so only 2-analog driving mode is used.
    - Add additional motor control and servo control.
    - Add a Control Panel for monitoring.
  Written by B4iter - STEMIST MSE Gen 5
*/

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

// define motor 3 and 4.
#define PWM_CHANNEL5 12
#define PWM_CHANNEL6 13
#define PWM_CHANNEL7 14
#define PWM_CHANNEL8 15

unsigned int addMotorSpeed = 4095;
// define desired servo position, it must be configured manually and applied before duty cycle implementation.
int servo1_pos,servo2_pos,servo3_pos,servo4_pos,servo5_pos,servo6_pos;
// duty cycle implementation.
//int servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);

const byte DNS_PORT = 53;
IPAddress apIP(192, 168, 4, 1);
DNSServer dnsServer;

const char *ssid = "Stemist Club MSE";
const char *password = "stemistclub";

const char *hostname = "stemist.mse";

void addMotorControlCallback(Control *sender, int type) {
  Serial.print("Slider: ID: ");
  Serial.print(sender->id);
  Serial.print(", Value: ");
  int switchval = sender->value.toInt();
  Serial.print(switchval);
  addMotorSpeed = switchval;
}
void espActionCallback(Control *sender, int type) {
  Serial.print("Button: ID: ");
  Serial.print(sender->id);
  switch (sender->id) {
    case 33:
    Serial.print("ESP is now cleanly restarting...");
    ESP.restart();
    break;
  }
}
uint16_t PWMMotor1s1,PWMMotor1s2,PWMMotor2s1,PWMMotor2s2,servo1pos,PWMMotor3s1,PWMMotor3s2,PWMMotor4s1,PWMMotor4s2,sliMaxMotor,addMotorControl,configWarning,disabledWarning,authorsection,creditsection,teamsection1,teamsection2,specialthanks,espAction;

void initPanel() {
  ESPUI.setVerbosity(Verbosity::Quiet);
  WiFi.setHostname(hostname);
  WiFi.begin(ssid, password);
  Serial.print("\n\nCreating hotspot");
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(ssid,password);
  dnsServer.start(DNS_PORT, "*", apIP);
  Serial.println("\n\nWiFi parameters:");
  Serial.print("Mode: ");
  Serial.println(WiFi.getMode() == WIFI_AP ? "Station" : "Client");
  Serial.print("IP address: ");
  Serial.println(WiFi.getMode() == WIFI_AP ? WiFi.softAPIP() : WiFi.localIP());
  // Debug Panel Begin here. 
  auto maintab = ESPUI.addControl(Tab, "Verbose", "Verbose");
  ESPUI.addControl(ControlType::Separator, "Main motors status", "", ControlColor::None, maintab);
  sliMaxMotor = ESPUI.addControl(ControlType::Slider, "Max Motor Power", getMotorOutput(5), Peterriver, maintab);
  ESPUI.addControl(Min, "", "0", None, sliMaxMotor);
  ESPUI.addControl(Max, "", "100", None, sliMaxMotor);
  ESPUI.setPanelWide(sliMaxMotor, true);
  PWMMotor1s1 = ESPUI.addControl(ControlType::Slider, "Motor 1 Pin A", getMotorOutput(1), Alizarin, maintab);
  ESPUI.addControl(Min, "", "0", None, PWMMotor1s1);
  ESPUI.addControl(Max, "", "4096", None, PWMMotor1s1);
  PWMMotor1s2 = ESPUI.addControl(ControlType::Slider, "Motor 1 Pin B", getMotorOutput(2), Emerald, maintab);
  ESPUI.addControl(Min, "", "0", None, PWMMotor1s2);
  ESPUI.addControl(Max, "", "4096", None, PWMMotor1s2);
  PWMMotor2s1 = ESPUI.addControl(ControlType::Slider, "Motor 2 Pin A", getMotorOutput(3), Alizarin, maintab);
  ESPUI.addControl(Min, "", "0", None, PWMMotor2s1);
  ESPUI.addControl(Max, "", "4096", None, PWMMotor2s1);
  PWMMotor2s2 = ESPUI.addControl(ControlType::Slider, "Motor 2 Pin B", getMotorOutput(1), Emerald, maintab);
  ESPUI.addControl(Min, "", "0", None, PWMMotor2s2);
  ESPUI.addControl(Max, "", "4096", None, PWMMotor2s2);
  ESPUI.addControl(ControlType::Separator, "Additional Motors Status", "", ControlColor::None, maintab);
  PWMMotor3s1 = ESPUI.addControl(ControlType::Switcher, "Motor 3 Pin A", "0", Alizarin, maintab);
  PWMMotor3s2 = ESPUI.addControl(ControlType::Switcher, "Motor 3 Pin B", "0", Emerald, maintab);
  PWMMotor4s1 = ESPUI.addControl(ControlType::Switcher, "Motor 4 Pin A", "0", Alizarin, maintab);
  PWMMotor4s2 = ESPUI.addControl(ControlType::Switcher, "Motor 4 Pin B", "0", Emerald, maintab);
  ESPUI.addControl(ControlType::Separator, "Servo Status", "", ControlColor::None, maintab);
  servo1pos = ESPUI.addControl(ControlType::Slider, "Servo 1", String(servo1_pos), Alizarin, maintab);
  ESPUI.addControl(Min, "", "0", None, servo1pos);
  ESPUI.addControl(Max, "", "180", None, servo1pos);
  auto configtab = ESPUI.addControl(Tab, "Configuration", "Configuration");
  disabledWarning = ESPUI.addControl(Label,"Warning","This configuration tab is disabled from controller <br> Please reboot the machine to unlock it",ControlColor::Alizarin,configtab);
  configWarning = ESPUI.addControl(Label,"Warning","Only use this if you know what you're doing",ControlColor::Alizarin,configtab);
  ESPUI.setPanelWide(configWarning, true);
  addMotorControl = ESPUI.addControl(ControlType::Slider, "Additional Motor Speed", String(addMotorSpeed), Peterriver, configtab, addMotorControlCallback);
  ESPUI.addControl(Min, "", "0", None, addMotorControl);
  ESPUI.addControl(Max, "", "4095", None, addMotorControl);
  ESPUI.setPanelWide(addMotorControl, true);
  espAction = ESPUI.addControl(Button,"ESP32 Maintainance","Restart ESP32",ControlColor::Turquoise,configtab,espActionCallback);
  //ESPUI.addControl(Button,"","Reset ESP32",ControlColor::None,espAction,espActionCallback);
  auto abouttab = ESPUI.addControl(Tab, "About", "About");
  teamsection1 = ESPUI.addControl(Label,"About Stemist Club - VRC 2023 Team","Official Team Members <br> Nguyen Minh Thai (Leader) <br> Dang Duy Khanh (Co-Leader) <br> Ha Tien Trieu <br> Khuat Dang Quang <br> Khuat Thi Khanh Ly <br> Kieu Nhat Linh <br> Nguyen Quang Minh",Emerald,abouttab);
  teamsection2 = ESPUI.addControl(Label,"About Stemist Club - VRC 2023 Team","Members - Contributor <br> Nguyen Hong Quang <br> Tran Tuan Duong <br> Nguyen Gia Huy <br> Pham Quoc Thinh ",Emerald,abouttab);
  specialthanks = ESPUI.addControl(Label,"Special Thanks to","Vu Quoc Khanh <br> Ha Thai Son <br> Nguyen Phuong Linh <br> Nguyen Ba Khoa <br> For your precious contribution to our team.",Emerald,abouttab);
  authorsection = ESPUI.addControl(Label,"Author", "Nguyen Minh Thai a.k.a B4iter (@b4iterdev)",Emerald,abouttab);
  creditsection = ESPUI.addControl(Label,"Credit", "Nguyen Minh Thai a.k.a B4iter (@b4iterdev) <br> Tu Dang - Makerviet <br> s00500 - ESPUI Library",Emerald,abouttab);
  ESPUI.updateVisibility(disabledWarning, false);
  //Make sliders continually report their position as they are being dragged.
  ESPUI.sliderContinuous = true;
  ESPUI.begin("Stemist Club MSE - VRC 2023 Control Panel");
}

void additionalMotor(unsigned int motor, int val) {
  switch (motor) {
    case 3:
    switch(val) {
      case 1:
      pwm.setPWM(PWM_CHANNEL5,0,addMotorSpeed);
      pwm.setPWM(PWM_CHANNEL6,0,0);
      ESPUI.updateSwitcher(PWMMotor3s1,1);
      ESPUI.updateSwitcher(PWMMotor3s2,0);
      break;
      case -1:
      pwm.setPWM(PWM_CHANNEL5,0,0);
      pwm.setPWM(PWM_CHANNEL6,0,addMotorSpeed);
      ESPUI.updateSwitcher(PWMMotor3s1,0);
      ESPUI.updateSwitcher(PWMMotor3s2,1);
      break;
      case 0:
      pwm.setPWM(PWM_CHANNEL5,0,0);
      pwm.setPWM(PWM_CHANNEL6,0,0);
      ESPUI.updateSwitcher(PWMMotor3s1,0);
      ESPUI.updateSwitcher(PWMMotor3s2,0);
      break;
    }
    break;
    case 4:
    switch(val) {
      case 1:
      pwm.setPWM(PWM_CHANNEL7,0,addMotorSpeed);
      pwm.setPWM(PWM_CHANNEL8,0,0);
      ESPUI.updateSwitcher(PWMMotor4s1,1);
      ESPUI.updateSwitcher(PWMMotor4s2,0);
      break;
      case -1:
      pwm.setPWM(PWM_CHANNEL7,0,0);
      pwm.setPWM(PWM_CHANNEL8,0,addMotorSpeed);
      ESPUI.updateSwitcher(PWMMotor4s1,0);
      ESPUI.updateSwitcher(PWMMotor4s2,1);
      break;
      case 0:
      pwm.setPWM(PWM_CHANNEL7,0,0);
      pwm.setPWM(PWM_CHANNEL8,0,0);
      ESPUI.updateSwitcher(PWMMotor4s1,0);
      ESPUI.updateSwitcher(PWMMotor4s2,0);
      break;
    }
    break;
  }
}

void additionalMotorInput() {
  if (!ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_SQUARE)) {
    additionalMotor(3,1);
    return;
  } else if (!ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_TRIANGLE)) {
    additionalMotor(4,1);
    return;
  } else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_SQUARE)) {
    additionalMotor(3,-1);
    return;
  } else if(ps2x.Button(PSB_SELECT) && ps2x.Button(PSB_TRIANGLE)) {
    additionalMotor(4,-1);
    return;
  } else if(ps2x.Button(PSB_CROSS)) {
    additionalMotor(4,0);
    return;
  } else if(ps2x.Button(PSB_CIRCLE)) {
    additionalMotor(3,0);
    return;
  }
}

void updateRequest() {
  dnsServer.processNextRequest();
  ESPUI.updateSlider(PWMMotor1s1,getMotorOutput(1).toInt());
  ESPUI.updateSlider(PWMMotor1s2,getMotorOutput(2).toInt());
  ESPUI.updateSlider(PWMMotor2s1,getMotorOutput(3).toInt());
  ESPUI.updateSlider(PWMMotor2s2,getMotorOutput(4).toInt());
  ESPUI.updateSlider(sliMaxMotor,getMotorOutput(5).toInt());
}

void servoControl(){
if (ps2x.ButtonReleased(PSB_PAD_LEFT) || ps2x.ButtonReleased(PSB_PAD_RIGHT)) {
    servo1_pos = 90;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
    ESPUI.updateSlider(servo1pos,servo1_pos);
  } else if (ps2x.Button(PSB_PAD_RIGHT)) {
    servo1_pos = 30;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
    ESPUI.updateSlider(servo1pos,servo1_pos);
  } else if (ps2x.Button(PSB_PAD_LEFT)) {
    servo1_pos = 150;
    long servo1 = map(servo1_pos, 0, 180, MIN_SERVO, MAX_SERVO);
    pwm.setPWM(2,0,servo1);
    ESPUI.updateSlider(servo1pos,servo1_pos);
  }
} 

void configtabDisable() {
  ESPUI.setEnabled(addMotorControl, false);
  const String disabledstyle = "background-color: #bbb; border-bottom: #999 3px solid;";
  ESPUI.setPanelStyle(addMotorControl, disabledstyle);
  ESPUI.updateVisibility(disabledWarning, true);
  addMotorSpeed = 4095;
  ESPUI.updateSlider(addMotorControl,addMotorSpeed);
}

void setup()
{
  Serial.begin(115200);
  initMotors();
  setupPS2controller();
  initPanel();
  Serial.println("Done setup!");
}

void loop()
{
  ps2x.read_gamepad(0, 0);
  PS2control();
  if(ps2x.ButtonPressed(PSB_START)) {
    configtabDisable();
  }
  additionalMotorInput();
  servoControl();
  updateRequest();
  delay(50);
}