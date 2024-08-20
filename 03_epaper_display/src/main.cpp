#include <Arduino.h>
#include <GxEPD2_BW.h>
#include <WiFi.h>
#include <time.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeMonoBold18pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <U8g2_for_Adafruit_GFX.h>
#include "../../99_include/board.h"
#include "GxEPD2_display_selection_new_style.h"

U8G2_FOR_ADAFRUIT_GFX u8g2Fonts;

bool ledState = LED_OFF;
const char HelloWorld[] = "Hello World!";

void setup() {
	pinMode(PIN_LED, OUTPUT);
	digitalWrite(PIN_LED, LED_OFF);

	pinMode(PIN_TOUCH_RESET, OUTPUT);
	pinMode(PIN_TOUCH_INT, INPUT);
  SPI.begin(PIN_EPD_CLK, D9, PIN_EPD_MOSI, PIN_EPD_CS);
  Serial.begin(115200);
	Serial.println("Starting ....");
	display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
  display.setRotation(1);
  u8g2Fonts.begin(display);
	if (display.pages() > 1)
	{
		delay(100);
		Serial.print("pages = "); Serial.print(display.pages()); Serial.print(" page height = "); Serial.println(display.pageHeight());
		delay(1000);
	}
  volatile uint16_t bg = GxEPD_WHITE;
  volatile uint16_t fg = GxEPD_BLACK;
  u8g2Fonts.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8g2Fonts.setFontDirection(0);            // left to right (this is default)
  u8g2Fonts.setForegroundColor(fg);         // apply Adafruit GFX color
  u8g2Fonts.setBackgroundColor(bg);         // apply Adafruit GFX color
  u8g2Fonts.setFont(u8g2_font_logisoso92_tn);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
  volatile int16_t tw = u8g2Fonts.getUTF8Width(HelloWorld); // text box width
  volatile int16_t ta = u8g2Fonts.getFontAscent(); // positive
  volatile int16_t td = u8g2Fonts.getFontDescent(); // negative; in mathematicians view
  volatile int16_t th = ta - td; // text box height
  //Serial.print("ascent, descent ("); Serial.print(u8g2Fonts.getFontAscent()); Serial.print(", "); Serial.print(u8g2Fonts.getFontDescent()); Serial.println(")");
  // center bounding box by transposition of origin:
  // y is base line for u8g2Fonts, like for Adafruit_GFX True Type fonts
  uint16_t x = (display.width() - tw) / 2;
  uint16_t y = (display.height() - th) / 2 + ta;
  //Serial.print("bounding box    ("); Serial.print(x); Serial.print(", "); Serial.print(y); Serial.print(", "); Serial.print(tw); Serial.print(", "); Serial.print(th); Serial.println(")");
  display.firstPage();
  do
  {
    display.fillScreen(bg);
    u8g2Fonts.setCursor(-2, 110); // start writing at this position
    u8g2Fonts.print("00");
    u8g2Fonts.setCursor(105, 110); // start writing at this position
    u8g2Fonts.print(":");
    u8g2Fonts.setCursor(124, 110); // start writing at this position
    u8g2Fonts.print("00");
    display.drawRect(display.width()-50, 0, 50, display.height()/2, GxEPD_BLACK);
    display.drawRect(display.width()-50, display.height()/2, 50, display.height()/2, GxEPD_BLACK);
  }
  while (display.nextPage());
  //Serial.println("helloWorld done");

  display.hibernate();
	Serial.println("Setup done ....");


}


void loop()
{	
  delay(1000);
  Serial.println("Looping ....");
}