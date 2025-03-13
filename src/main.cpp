#include <Arduino.h>
#include <Serial.h>

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("initialized serial");

  // Setup the output pins
  pinMode(25, OUTPUT);
  pinMode(27, OUTPUT);
  pinMode(29, OUTPUT);
  pinMode(31, OUTPUT);
  pinMode(33, OUTPUT);
  pinMode(35, OUTPUT);
  pinMode(37, OUTPUT);
  pinMode(38, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

  // Set gpio pins with register
  GPIOJ->ODR = (0b00000011);
}