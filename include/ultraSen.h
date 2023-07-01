#include <Arduino.h>
#define DEBUG
// define Sensor 1 pinOut
#define trigPin1 0
#define echoPin1 0

long temp;
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
  distance = ((duration *331.3 + 0.606 * temp) / 2 );
  #ifdef DEBUG
  Serial.print("Distance: ");
  Serial.println(distance);
  #endif
}

int opTemp(int action,long target) {
    switch(action) {
        case 0:
            temp = target;
            return 0;
            break;
        case 1:
            return temp;
            break;
    }
}

long getDistance() {
    return distance;
}
