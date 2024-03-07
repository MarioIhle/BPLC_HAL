#ifndef HAL_TMP11_h
#define HAL_TMP11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_TMP11__ADDR_1 = 0x68,
    I2C_ADDRESS_TMP11__ADDR_2 = 0x6A,
    I2C_ADDRESS_TMP11__ADDR_3 = 0x6C,
    I2C_ADDRESS_TMP11__ADDR_4 = 0x6E,
    
    I2C_ADDRESS_TMP11__COUNT = 4,
}e_I2C_ADDRESS_TMP11_t;

//-------------------------------------------------------------
#define TMP11_CHANNEL_COUNT 4

//-------------------------------------------------------------
class HAL_TMP11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_TMP11           ();
    //Hal interface 
    void            init                (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel  (IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL);        
    void            tick                ();        
    //Modul Error Interface
    e_BPLC_ERROR_t  getModulError       ();
    void            resetModulError     (String FILE, const uint16_t LINE){this->resetError(FILE, LINE);}

  
    private:          
    //Settings
    Adafruit_ADS1115        ADC;
    e_I2C_ADDRESS_TMP11_t   deviceAddress;
    adsGain_t               adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [TMP11_CHANNEL_COUNT];   
        const uint8_t PIN         [TMP11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif