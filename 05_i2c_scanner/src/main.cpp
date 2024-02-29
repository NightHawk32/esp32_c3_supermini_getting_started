#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "../99_include/board.h"


void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  pinMode(PIN_TOUCH_RESET, OUTPUT);
  digitalWrite(PIN_TOUCH_RESET, true);

  Serial.begin(115200);
  Serial.println("Starting ....");
  Wire.begin (PIN_SDA, PIN_SCL);
  Serial.println("Setup done ....");

}

void Scanner ()
{
  Serial.println ();
  Serial.println ("I2C scanner. Scanning ...");
  byte count = 0;

  Wire.begin();
  for (byte i = 8; i < 120; i++)
  {
    Wire.beginTransmission (i);          // Begin I2C transmission Address (i)
    if (Wire.endTransmission () == 0)  // Receive 0 = success (ACK response) 
    {
      Serial.print ("Found address: ");
      Serial.print (i, DEC);
      Serial.print (" (0x");
      Serial.print (i, HEX);     // PCF8574 7 bit address
      Serial.println (")");
      count++;
    }
  }
  Serial.print ("Found ");      
  Serial.print (count, DEC);        // numbers of devices
  Serial.println (" device(s).");
}

void loop()
{
  Scanner ();
  delay (2000);
}