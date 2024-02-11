#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ModbusClientTCPasync.h>
#include "../99_include/board.h"
#include "../99_include/myWifi.h"

bool ledState = LED_OFF;
IPAddress ip = {192, 168, 0, 191};          // IP address of modbus server
uint16_t port = 33007;                      // port of modbus server

// Create a ModbusTCP client instance
ModbusClientTCPasync MB(ip, port);

void setup_wifi() {
  delay(10);
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    if (++counter > 100) ESP.restart();
    Serial.print(".");
  }
  Serial.print("WiFi connected: ");
  Serial.println(MY_WIFI_SSID);
  IPAddress wIP = WiFi.localIP();
  Serial.printf("WIFi IP address: %u.%u.%u.%u\n", wIP[0], wIP[1], wIP[2], wIP[3]);
}

void handleData(ModbusMessage response, uint32_t token) 
{
  Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
  for (auto& byte : response) {
    Serial.printf("%02X ", byte);
  }
  Serial.println("");
  uint16_t word1;
  response.get(3, word1);
  Serial.println(word1);
}
 
// Define an onError handler function to receive error responses
// Arguments are the error code returned and a user-supplied token to identify the causing request
void handleError(Error error, uint32_t token) 
{
  // ModbusError wraps the error code and provides a readable error message for it
  ModbusError me(error);
  Serial.printf("Error response: %02X - %s token: %d\n", (int)me, (const char *)me, token);
}

void setup() {
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, ledState);

  Serial.begin(115200);
  Serial.println("Starting ....");

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  Serial.println("Setup done ....");

  setup_wifi();

  MB.onDataHandler(&handleData);
// - provide onError handler function
  MB.onErrorHandler(&handleError);
// Set message timeout to 2000ms and interval between requests to the same host to 200ms
  MB.setTimeout(10000);
// Start ModbusTCP background task
  MB.setIdleTimeout(60000);
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();


    Serial.printf("sending request with token %d\n", (uint32_t)lastMillis);
    Error err;
    err = MB.addRequest((uint32_t)lastMillis, 100, READ_HOLD_REGISTER, 843, 1);
    if (err != SUCCESS) {
      ModbusError e(err);
      Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
    }

  }
  if(WiFi.status() != WL_CONNECTED){
    setup_wifi();
    digitalWrite(PIN_LED, LED_OFF);
  }else{
    digitalWrite(PIN_LED, LED_ON);
  }
}