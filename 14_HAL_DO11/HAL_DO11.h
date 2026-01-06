#ifndef HAL_DO11_h
#define HAL_DO11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCA9685.h"
//-------------------------------------------------------------
//Card definition
#define DO11_ADDRESS_COUNT 4
#define DO11_CHANNEL_COUNT 8
const uint8_t DO11_I2C_ADDRESSES[DO11_ADDRESS_COUNT] = {0x43, 0x42, 0x41, 0x40};

//-------------------------------------------------------------
#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1

//-------------------------------------------------------------
class HAL_DO11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
                    HAL_DO11                ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    void            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    (const bool READ_INPUTS);  
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);         
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    
    private:
    //Settings          
    PCA9685     PCA;    
    struct
    {
        IO_Interface* p_ioObject  [DO11_CHANNEL_COUNT];         
        const uint8_t PIN         [DO11_CHANNEL_COUNT][2] = {{15, 4}, {14, 5}, {13, 6}, {12, 7}, {8, 0}, {9, 1}, {10, 2}, {11, 3}};     //{lowside, highside}
    }channels; 
};
#endif