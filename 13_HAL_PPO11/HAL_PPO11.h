#ifndef HAL_PPO11_h
#define HAL_PPO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "PCA9685.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"
#include "HAL_interface.h"

typedef enum
{
   I2C_ADDRESS_PPO11__ADDR_1 = 0x47,
   I2C_ADDRESS_PPO11__ADDR_2 = 0x46,
   I2C_ADDRESS_PPO11__ADDR_3 = 0x45,
   I2C_ADDRESS_PPO11__ADDR_4 = 0x44,
    
   I2C_ADDRESS_PPO11__COUNT = 4,

}e_I2C_ADDRESS_PPO11_t;

#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1
#define PPO11_CHANNEL_COUNT 8

class HAL_PPO11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_PPO11           ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (); 

    
    private:
    //Settings          
    PCA9685                 PCA;
    e_I2C_ADDRESS_PPO11_t   deviceAddress;
    
    struct
    {
        IO_Interface* p_ioObject  [PPO11_CHANNEL_COUNT];         
        const uint8_t PIN         [PPO11_CHANNEL_COUNT][2] = {{6, 7}, {4, 5}, {2, 3}, {0, 1}, {14, 15}, {12, 13}, {10, 11}, {8, 9}};     //{lowside, highside}
    }channels; 
};
#endif