#ifndef I2C_check_h
#define I2C_check_h

#include "Arduino.h"
#include "Wire.h"
#include "SpecialFunctions.h"

//#define DEBUG_I2C_Check

class I2C_check
{
  public:
  I2C_check();
  bool begin(const uint8_t ADDRESS);

  bool requestHeartbeat();
  bool checkI2CConnection();

  private:
  //Applikation
  Timeout to_heartbeat;
  uint8_t errorCount;
  uint8_t errorCountLimit;
  bool    f_error;

  //Settings
  uint8_t deviceAddress;
};
#endif