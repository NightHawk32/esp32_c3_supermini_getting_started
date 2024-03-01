#include <Arduino.h>
#include <Wire.h>
#include <time.h>
#include "../99_include/board.h"

#define ICNT_MAX_TOUCH 5
#define ICNT_ADDR 0x48

typedef struct{
	uint8_t Touch;
	uint8_t TouchCount;
	uint8_t TouchGestureid;
	
	uint8_t TouchEvenid[ICNT_MAX_TOUCH];
	uint8_t X[ICNT_MAX_TOUCH];
	uint8_t Y[ICNT_MAX_TOUCH];
	uint8_t P[ICNT_MAX_TOUCH];
}ICNT86_Dev;

ICNT86_Dev ICNT86_Dev_Now, ICNT86_Dev_Old;

void ICNT_Reset(void)
{
	digitalWrite(PIN_TOUCH_RESET, true);
	delay(100);
	digitalWrite(PIN_TOUCH_RESET, false);
	delay(100);
	digitalWrite(PIN_TOUCH_RESET, true);
	delay(100);
}

void ICNT_Write(uint16_t Reg, char *Data, uint8_t len)
{
  Wire.beginTransmission(ICNT_ADDR);
	Wire.write(Reg);
  for(int i=0; i<len; i++){
    Wire.write(Data[i]);
  }  
  Wire.end();
}

void ICNT_Read(uint16_t Reg, char *Data, uint8_t len)
{
  Wire.beginTransmission(ICNT_ADDR);
	Wire.write(Reg);
  for(int i=0; i<len; i++){
    Data[i] = Wire.read();
  }  
  Wire.end();
}

void ICNT_ReadVersion(void)
{
	char buf[4];
	ICNT_Read(0x000a, buf, 4);
	Serial.print("IC Version is");
  Serial.print(buf[0], HEX);
  Serial.println(buf[1], HEX);
  Serial.print("FW Version is");
  Serial.print(buf[2], HEX);
  Serial.println(buf[3], HEX);
}

void ICNT_Init(void)
{
	ICNT_Reset();
	ICNT_ReadVersion();
}

uint8_t ICNT_Scan(void)
{
	char buf[100];
	char mask[1] = {0x00};
	if (ICNT86_Dev_Now.Touch == 1) {
		ICNT86_Dev_Now.Touch = 0;
		ICNT_Read(0x1001, buf, 1);
		if (buf[0] == 0x00) {		//No new touch
			ICNT_Write(0x1001, mask, 1);
			delay(1);
			// printf("buffers status is 0 \r\n");
			return 1;
		}
		else {
			ICNT86_Dev_Now.TouchCount = buf[0];
			if (ICNT86_Dev_Now.TouchCount > 5 || ICNT86_Dev_Now.TouchCount < 1) {
				ICNT_Write(0x1001, mask, 1);
				ICNT86_Dev_Now.TouchCount = 0;
				// printf("TouchCount number is wrong \r\n");
				return 1;
			}
			ICNT_Read(0x1002, buf, ICNT86_Dev_Now.TouchCount*7);
			ICNT_Write(0x1001, mask, 1);
			
			ICNT86_Dev_Old.X[0] = ICNT86_Dev_Now.X[0];
			ICNT86_Dev_Old.Y[0] = ICNT86_Dev_Now.Y[0];
			ICNT86_Dev_Old.P[0] = ICNT86_Dev_Now.P[0];
			
			for(uint8_t i=0; i<ICNT86_Dev_Now.TouchCount; i++) {
				ICNT86_Dev_Now.X[i] = ((uint16_t)buf[2+7*i] << 8) + buf[1+7*i];
				ICNT86_Dev_Now.Y[i] = ((uint16_t)buf[4+7*i] << 8) + buf[3+7*i];
				ICNT86_Dev_Now.P[i] = buf[5+7*i];
				ICNT86_Dev_Now.TouchEvenid[i] = buf[6 + 7*i];
			}
			
			for(uint8_t i=0; i<ICNT86_Dev_Now.TouchCount; i++)
				Serial.print("Point");
        Serial.print(i+1,DEC);
        Serial.print(": X is"),;
        Serial.print(ICNT86_Dev_Now.X[i],DEC);
        Serial.print(" ,Y is");
        Serial.print( ICNT86_Dev_Now.Y[i],DEC);
        Serial.print(" ,Pressure is");
        Serial.println(ICNT86_Dev_Now.P[i],DEC);
			return 0;
		}
	}
	return 1;
}


void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LED_ON);

  pinMode(PIN_TOUCH_RESET, OUTPUT);

  Serial.begin(115200);
  Serial.println("Starting ....");
  Wire.begin (PIN_SDA, PIN_SCL);
  ICNT_Reset();
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