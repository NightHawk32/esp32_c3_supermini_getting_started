#include <Arduino.h>
#include "../99_include/board.h"

bool ledState = LED_OFF;

void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, ledState);

  Serial.begin(115200);
  Serial.println("Start blinky");
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("LED ");
  Serial.println(!ledState);
  digitalWrite(PIN_LED, ledState);
  ledState = !ledState;
  delay(1000);
}