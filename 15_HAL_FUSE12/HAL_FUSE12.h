#ifndef HAL_FUSE12_h
#define HAL_FUSE12_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"

#include "BPLC_ioBaseTypes.h"
#include "HAL_interface.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    I2C_ADDRESS_FUSE12__ADDR_1 = 0xC0,
    I2C_ADDRESS_FUSE12__ADDR_2 = 0xC1,
    I2C_ADDRESS_FUSE12__ADDR_3 = 0xC2,
    I2C_ADDRESS_FUSE12__ADDR_4 = 0xC3,
    
    I2C_ADDRESS_FUSE12__COUNT = 4,
}e_I2C_ADDRSS_FUSE12_t;

//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_FUSE12:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_FUSE12          ();
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

  
    private:          
    //Settings  
    e_I2C_ADDRSS_FUSE12_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif