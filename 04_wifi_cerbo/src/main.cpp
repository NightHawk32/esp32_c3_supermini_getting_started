#include <Arduino.h>
#include <WiFi.h>
#include <time.h>
#include <ModbusClientTCPasync.h>
#include "../99_include/board.h"
#include "../99_include/myWifi.h"
#include "../99_include/cerbo.h"
#include "../lib/VictronModbusRegs.h"

bool ledState = LED_OFF;
IPAddress *ip;
ModbusClientTCPasync *MB;

void UpdateRegister(VictronModbusRegister *reg, uint32_t token, ServiceName serviceName){
  //Serial.printf("sending request with token %d\n", (uint32_t)token);
  Error err;
  err = MB->addRequest((uint32_t)token, (int)serviceName, READ_HOLD_REGISTER, reg->GetAddress(), 1);
  if (err != SUCCESS) {
    ModbusError e(err);
    Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
  }else{
    reg->SetToken(token);
  }
}

VictronModbusRegister BatteryVoltage = VictronModbusRegister("Battery SOC", 843, ServiceName::System, 1, "%", &UpdateRegister);
VictronModbusRegister BatteryCurrent = VictronModbusRegister("Battery Current", 841, ServiceName::System, 0.1, "A", &UpdateRegister, true);
VictronModbusRegister BatteryTemp = VictronModbusRegister("Battery Temperature", 262, ServiceName::Battery, 0.1, "°C", &UpdateRegister, true);
VictronModbusRegister BatteryPower = VictronModbusRegister("Battery Power", 842, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister DC_PV_Power = VictronModbusRegister("DC - PV Power", 850, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister BatteryState = VictronModbusRegister("Battery State", 844, ServiceName::System, 1, "W", &UpdateRegister, false, true, 3, new String[3]{"idle","charging","discharging"});

VictronModbusRegister GridPowerL1 = VictronModbusRegister("Grid Power L1", 820, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister GridPowerL2 = VictronModbusRegister("Grid Power L2", 821, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister GridPowerL3 = VictronModbusRegister("Grid Power L3", 822, ServiceName::System, 1, "W", &UpdateRegister, true);

VictronModbusRegister AcOutPowerL1 = VictronModbusRegister("AC Output Power L1", 817, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister AcOutPowerL2 = VictronModbusRegister("AC Output Power L2", 818, ServiceName::System, 1, "W", &UpdateRegister, true);
VictronModbusRegister AcOutPowerL3 = VictronModbusRegister("AC Output Power L3", 819, ServiceName::System, 1, "W", &UpdateRegister, true);

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
  //Serial.printf("Response: serverID=%d, FC=%d, Token=%08X, length=%d:\n", response.getServerID(), response.getFunctionCode(), token, response.size());
  /*for (auto& byte : response) {
    Serial.printf("%02X ", byte);
  }*/
  for(int i = 0;i < RegisterList::getInstance().GetNumRegisters();i++){
    VictronModbusRegister *reg = RegisterList::getInstance().GetRegister(i);
    if(reg->GetToken() == token){
      reg->HandleData(response.data(), response.size());
      Serial.println(reg->GetNameWithVal());
    }
  }
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
  RegisterList::getInstance().AddRegister(&BatteryVoltage);
  RegisterList::getInstance().AddRegister(&BatteryCurrent);
  RegisterList::getInstance().AddRegister(&BatteryTemp);
  RegisterList::getInstance().AddRegister(&BatteryPower);
  RegisterList::getInstance().AddRegister(&DC_PV_Power);
  RegisterList::getInstance().AddRegister(&BatteryState);
  RegisterList::getInstance().AddRegister(&GridPowerL1);
  RegisterList::getInstance().AddRegister(&GridPowerL2);
  RegisterList::getInstance().AddRegister(&GridPowerL3);
  RegisterList::getInstance().AddRegister(&AcOutPowerL1);
  RegisterList::getInstance().AddRegister(&AcOutPowerL2);
  RegisterList::getInstance().AddRegister(&AcOutPowerL3);

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


   /* Serial.printf("sending request with token %d\n", (uint32_t)lastMillis);
    Error err;
    err = MB->addRequest((uint32_t)lastMillis, 100, READ_HOLD_REGISTER, 843, 1);
    if (err != SUCCESS) {
      ModbusError e(err);
      Serial.printf("Error creating request: %02X - %s\n", (int)e, (const char *)e);
    }*/

    //BatteryVoltage.UpdateRegister(); 

    RegisterList::getInstance().UpdateAllRegisters();

  }
  if(WiFi.status() != WL_CONNECTED){
    setup_wifi();
    digitalWrite(PIN_LED, LED_OFF);
  }else{
    digitalWrite(PIN_LED, LED_ON);
  }
}