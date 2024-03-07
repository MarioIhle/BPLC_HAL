#ifndef HAL_AIN11_h
#define HAL_AIN11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include <Adafruit_ADS1X15.h>

//-------------------------------------------------------------
//I2C ADDRESSEN
typedef enum
{
    I2C_ADDRESS_AIN11__ADDR_1 = 0x48,
    I2C_ADDRESS_AIN11__ADDR_2 = 0x49,
    I2C_ADDRESS_AIN11__ADDR_3 = 0x4A,
    I2C_ADDRESS_AIN11__ADDR_4 = 0x4B,
    
    I2C_ADDRESS_AIN11__COUNT = 4,
}e_I2C_ADDRESS_AIN11_t;

//-------------------------------------------------------------
#define AIN11_CHANNEL_COUNT 4

//-------------------------------------------------------------
class HAL_AIN11: public halInterface,  private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_AIN11           ();
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
    e_I2C_ADDRESS_AIN11_t   deviceAddress;
    adsGain_t               adcGain;
  
    //Object handling
    struct
    {
        IO_Interface* p_ioObject  [AIN11_CHANNEL_COUNT];   
        const uint8_t PIN         [AIN11_CHANNEL_COUNT] = {2, 3, 1, 0};   
    }channels;         
 };
#endif