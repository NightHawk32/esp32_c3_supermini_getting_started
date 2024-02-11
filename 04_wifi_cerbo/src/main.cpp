#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ModbusClientTCPasync.h>
#include "../99_include/board.h"
#include "../99_include/myWifi.h"
#include "../99_include/cerbo.h"

bool ledState = LED_OFF;
IPAddress *ip;
ModbusClientTCPasync *MB;

void setup_wifi() {
  if ((WiFi.getMode() != WIFI_STA))
  {
    WiFi.mode(WIFI_STA);
    WiFi.disconnect(true, true);
  }
  delay(10);
  WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
  int counter = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    if(counter%10 == 0){
      WiFi.disconnect(true, true);
      WiFi.begin(MY_WIFI_SSID, MY_WIFI_PASSWORD);
    }

    if (++counter > 100) ESP.restart();
    Serial.print(".");
  }
  Serial.print("WiFi connected: ");
  Serial.println(MY_WIFI_SSID);
  IPAddress wIP = WiFi.localIP();
  Serial.printf("WIFi IP address: %u.%u.%u.%u\n", wIP[0], wIP[1], wIP[2], wIP[3]);
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println("Disconnected from WiFi access point");
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  setup_wifi();
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
  Serial.print("Bat SOC: ");
  Serial.print(word1);
  Serial.println("%");
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

  setup_wifi();
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  ip = new IPAddress();
  ip->fromString(CERBO_IP);
  MB = new ModbusClientTCPasync(*ip, (uint16_t)CERBO_PORT);

  MB->onDataHandler(&handleData);
// - provide onError handler function
  MB->onErrorHandler(&handleError);
// Set message timeout to 2000ms and interval between requests to the same host to 200ms
  MB->setTimeout(10000);
// Start ModbusTCP background task
  MB->setIdleTimeout(60000);

  Serial.println("Setup done ....");
}

void loop() {
  static unsigned long lastMillis = 0;
  if (millis() - lastMillis > 5000) {
    lastMillis = millis();


    Serial.printf("sending request with token %d\n", (uint32_t)lastMillis);
    Error err;
    err = MB->addRequest((uint32_t)lastMillis, 100, READ_HOLD_REGISTER, 843, 1);
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