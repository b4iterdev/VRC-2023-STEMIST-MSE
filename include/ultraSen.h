#include <Arduino.h>
#define DEBUG
// define Sensor 1 pinOut
#define trigPin1 32
#define echoPin1 36

long duration, distance;

void initSensor() {
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  distance = 0;
}

void trackSen() {
  digitalWrite(trigPin1, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin1, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin1, LOW);
  // Read the signal from the sensor: a HIGH pulse whose 
  // duration is the time (in microseconds) from the sending 
  // of the ping to the reception of its echo off of an object. 
  duration = pulseIn(echoPin1, HIGH);
  distance = (duration*.0343)/2;  
  #ifdef DEBUG
  Serial.print("Distance: ");
  Serial.println(distance);
  #endif
}

long getDistance() {
    return distance;
}
