#ifndef HAL_DO11_h
#define HAL_DO11_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "PCA9685.h"
#include "BPLC_ioBaseTypes.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"
#include "HAL_interface.h"
#include "BPLC_ERRORS.h"

typedef enum
{
    I2C_ADDRESS_DO11__ADDR_1 = 0x43,
    I2C_ADDRESS_DO11__ADDR_2 = 0x42,
    I2C_ADDRESS_DO11__ADDR_3 = 0x41,
    I2C_ADDRESS_DO11__ADDR_4 = 0x40,
    
    I2C_ADDRESS_DO11__COUNT = 4,

}e_I2C_ADDRESS_DO11_t;

#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1
#define DO11_CHANNEL_COUNT 8

class HAL_DO11:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_DO11            ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        (); 

    
    private:
    //Settings          
    PCA9685                 PCA;
    e_I2C_ADDRESS_DO11_t    deviceAddress;
    
    struct
    {
        IO_Interface* p_ioObject  [DO11_CHANNEL_COUNT];         
        const uint8_t PIN         [DO11_CHANNEL_COUNT][2] = {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}
    }channels; 
};
#endif