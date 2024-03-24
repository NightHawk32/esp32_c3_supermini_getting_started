#ifndef VICTRONMODBUSREGS_H_
#define VICTRONMODBUSREGS_H_

#include <string>
#include <Arduino.h>
#include <ModbusClientTCPasync.h>

enum ServiceName{
    System = 100,
    Vebus,
    Battery = 225,
    Solarcharger,
    PvInverter
};

class VictronModbusRegister{
public:
   VictronModbusRegister(String name, uint16_t address, ServiceName serviceName, double scaleFactor, String unit, void (*updateRegFunction)(VictronModbusRegister * reg, uint32_t token, ServiceName serviceName), bool isSigned = false, bool isStatus = false,uint32_t numStringLookupVals = 0, String *statusMap=NULL){
        this->name = name;
        this-> isSigned = isSigned;
        this->address = address;
        this->serviceName = serviceName;
        this->scaleFactor = scaleFactor;
        this->unit = unit;
        this->isStatus = isStatus;
        this->statusMap = statusMap;
        this->updateRegFunction = updateRegFunction;
   }

   double GetCurrentValue(){
        return value;
   }

   String GetName(){
        return name;
   } 

   String GetNameWithVal(){
        return name + ": " + GetCurrentValueString();
   }

   String GetCurrentValueString(){
        if(isStatus){
            return valueString;
        }
        return String(value) + unit;
   }

   void SetToken(uint32_t token){
        this->token = token;
   }

   uint32_t GetToken(){
        return token;
   }

   void UpdateRegister(uint32_t token){
        //MB->addRequest((uint32_t)lastMillis, 100, READ_HOLD_REGISTER, 843, 1);
        this->updateRegFunction(this, token, serviceName);
   }

   uint16_t GetAddress(){
        return address;
   } 

   void HandleData(const uint8_t *data, uint8_t dataLength){
        uint16_t val = data[3] << 8 | data[4];

        if(isSigned){
            int16_t signedVal = val;
            value = signedVal * scaleFactor;
        }else{
            value = val * scaleFactor;
        }

        if(isStatus){
            valueString = statusMap[val];
        }
   }

private:
    String name;
    bool isStatus;
    uint16_t address;
    ServiceName serviceName;
    double scaleFactor;
    String unit;
    uint32_t token;
    String *statusMap;
    double value;
    String valueString;
    bool isSigned;
    void (*updateRegFunction)(VictronModbusRegister * reg, uint32_t token, ServiceName serviceName);

};

/*VictronModbusRegister BatteryVoltage = VictronModbusRegister("Battery SOC", 843, ServiceName::System, 1, "%");
VictronModbusRegister BatteryCurrent = VictronModbusRegister("Battery Current", 841, ServiceName::System, 0.1, "A");
VictronModbusRegister BatteryPower = VictronModbusRegister("Battery Power", 842, ServiceName::System, 1, "W");
VictronModbusRegister DC_PV_Power = VictronModbusRegister("DC - PV Power", 850, ServiceName::System, 1, "W");
VictronModbusRegister BatteryState = VictronModbusRegister("Battery State", 844, ServiceName::System, 1, "W", true, 3, new String[3]{"idle","charging","discharging"});

VictronModbusRegister GridPowerL1 = VictronModbusRegister("Grid Power L1", 820, ServiceName::System, 1, "W");
VictronModbusRegister GridPowerL2 = VictronModbusRegister("Grid Power L2", 821, ServiceName::System, 1, "W");
VictronModbusRegister GridPowerL3 = VictronModbusRegister("Grid Power L3", 822, ServiceName::System, 1, "W");

VictronModbusRegister AcOutPowerL1 = VictronModbusRegister("AC Output Power L1", 817, ServiceName::System, 1, "W");
VictronModbusRegister AcOutPowerL2 = VictronModbusRegister("AC Output Power L2", 818, ServiceName::System, 1, "W");
VictronModbusRegister AcOutPowerL3 = VictronModbusRegister("AC Output Power L3", 819, ServiceName::System, 1, "W");*/

class RegisterList{
private:
    RegisterList() = default; // Make constructor private
    std::vector<VictronModbusRegister *> registers;
  public:
    static RegisterList &getInstance(); // Accessor for singleton instance

    RegisterList(const RegisterList &) = delete; // no copying
    RegisterList &operator=(const RegisterList &) = delete;
    void AddRegister(VictronModbusRegister *reg){
        registers.push_back(reg);
    }
    int GetNumRegisters(){
        return registers.size();
    }

    VictronModbusRegister *GetRegister(int index){
        return registers[index];
    }

    void UpdateAllRegisters(){
      uint32_t lastMillis = millis();
        for(int i = 0;i < registers.size();i++){
            registers[i]->UpdateRegister(lastMillis+i);
        }
    }
};

RegisterList &RegisterList::getInstance(){
  static RegisterList instance;
  return instance;
}

#endif
