#ifndef HAL_PPO11_h
#define HAL_PPO11_h

//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "PCA9685.h"

//-------------------------------------------------------------
//Card definition
#define PPO11_ADDRESS_COUNT 4
#define PPO11_CHANNEL_COUNT 8
const uint8_t PPO11_I2C_ADDRESSES[PPO11_ADDRESS_COUNT] = {0x47, 0x46, 0x45, 0x44};

//-------------------------------------------------------------
//Interne 
#define DEAD_TIME 100 //besser geht nicht, ohne kurzeitigen Kurzschluss bei PWM änderung
#define LS_MOSFET 0
#define HS_MOSFET 1

//-------------------------------------------------------------
class HAL_PPO11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal constructor
                    HAL_PPO11               ();
    //Hal interface 
    void            init                    (const e_EC_ADDR_t ADDR);
    bool            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    (const bool READ_INPUTS);      
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);     
    //Modul Error Interface   
    uint8_t         getModuleErrorCount     ()                                                      {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode      (uint8_t ERROR_NUMBER)                                  {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors    (String FILE, const uint16_t LINE)                      {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorManager (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER)     {this->p_superiorErrorManager = P_SUPERIOR_ERROR_MANAGER;}

    
    private:
    //Settings          
    PCA9685 PCA;
       
    struct
    {
        IO_Interface* p_ioObject  [PPO11_CHANNEL_COUNT];         
        const uint8_t PIN         [PPO11_CHANNEL_COUNT][2] = {{6, 7}, {4, 5}, {2, 3}, {0, 1}, {14, 15}, {12, 13}, {10, 11}, {8, 9}};     //{lowside, highside}
    }channels; 
};
#endif