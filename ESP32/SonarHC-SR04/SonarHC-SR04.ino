#include "Arduino.h"

#define ECHO_PIN 2 
#define TRIG_PIN 0 

int32_t duration; 
int32_t distance; 

void setup(){
  Serial.begin (9600);
  Serial.println("Usando HC-SR04");
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop(){
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  duration = pulseIn(ECHO_PIN, HIGH);
  distance = duration / 58.2;
  Serial.println(distance);
  delay(50);
}
