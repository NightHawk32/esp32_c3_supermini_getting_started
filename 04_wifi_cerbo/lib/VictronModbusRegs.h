#ifndef VICTRONMODBUSREGS_H_
#define VICTRONMODBUSREGS_H_

#include <string>
#include <Arduino.h>

enum ServiceName{
    System,
    Vebus,
    Battery,
    Solarcharger,
    PvInverter
};


class RegisterList{
private:
    RegisterList() = default; // Make constructor private
  public:
    static RegisterList &getInstance(); // Accessor for singleton instance

    RegisterList(const RegisterList &) = delete; // no copying
    RegisterList &operator=(const RegisterList &) = delete;
};

RegisterList &RegisterList::getInstance(){
  static RegisterList instance;
  return instance;
}

class VictronModbusRegister{
public:
   VictronModbusRegister(String name, uint16_t address, ServiceName serviceName, double scaleFactor, String unit);
   double GetCurrentValue();
   String GetCurrentValueString();

private:
    String name;
    uint16_t address;
    ServiceName serviceName;
    double scaleFactor;
    String unit;
};

VictronModbusRegister BatteryVoltage = VictronModbusRegister("Battery SOC", 843, ServiceName::System, 1, "V");
VictronModbusRegister BatteryCurrent = VictronModbusRegister("Battery Current", 841, ServiceName::System, 0.1, "A");
VictronModbusRegister BatteryCurrent = VictronModbusRegister("Battery Power", 842, ServiceName::System, 1, "W");
VictronModbusRegister BatteryCurrent = VictronModbusRegister("DC - PV Power", 850, ServiceName::System, 1, "W");
VictronModbusRegister BatteryCurrent = VictronModbusRegister("Battery State", 844, ServiceName::System, 1, "W");

VictronModbusRegister BatteryVoltage = VictronModbusRegister("Grid Power L1", 820, ServiceName::System, 1, "W");
VictronModbusRegister BatteryVoltage = VictronModbusRegister("Grid Power L2", 821, ServiceName::System, 1, "W");
VictronModbusRegister BatteryVoltage = VictronModbusRegister("Grid Power L3", 822, ServiceName::System, 1, "W");

VictronModbusRegister BatteryVoltage = VictronModbusRegister("AC Output Power L1", 817, ServiceName::System, 1, "W");
VictronModbusRegister BatteryVoltage = VictronModbusRegister("AC Output Power L2", 818, ServiceName::System, 1, "W");
VictronModbusRegister BatteryVoltage = VictronModbusRegister("AC Output Power L3", 819, ServiceName::System, 1, "W");

#endif
