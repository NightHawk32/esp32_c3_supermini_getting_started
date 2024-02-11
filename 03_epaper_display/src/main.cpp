#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include "../99_include/board.h"
#include "../99_include/myWifi.h"

bool ledState = LED_OFF;

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, ledState);

  Serial.begin(115200);
  Serial.println("Starting ....");

  Serial.println("Setup done ....");
}

void loop() {
  delay(2000);
}