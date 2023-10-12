#ifndef I2C_check_h
#define I2C_check_h

#include "Arduino.h"
#include "Wire.h"

//#define DEBUG_I2C_Check

class I2C_check
{
  public:
  I2C_check();
  bool checkI2CConnection(const uint8_t ADDRESS);

  private:
};
#endif