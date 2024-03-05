#ifndef HAL_FUSE12_h
#define HAL_FUSE12_h
//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"

#include "BPLC_ioBaseTypes.h"
#include "HAL_interface.h"
#include "BPLC_ERRORS.h"
#include "BPLC_PLI.h"
#include "I2C_check.h"

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    FUSE12_CARD_1_ADDRESS = 0xC0,
    FUSE12_CARD_2_ADDRESS = 0xC1,
    FUSE12_CARD_3_ADDRESS = 0xC2,
    FUSE12_CARD_4_ADDRESS = 0xC3,
    
    FUSE12_CARD_ADDRESS_COUNT = 4,
}e_FUSE12_ADDRESS_t;

//-------------------------------------------------------------
//HAL_AIN11 KLASSE
//-------------------------------------------------------------
class HAL_FUSE12:BPLC_LOG, I2C_check, public halInterface, BPLC_errorHandler
{
    public:
                    HAL_FUSE12          (const e_FUSE12_ADDRESS_t I2C_ADDRESS);
    void            init                ();
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    e_BPLC_ERROR_t  getErrorCode        ();

  
    private:          
    //Settings  
    e_FUSE12_ADDRESS_t   deviceAddress;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif