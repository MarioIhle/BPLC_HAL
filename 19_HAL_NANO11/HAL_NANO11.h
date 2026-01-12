#ifndef HAL_NANO11_h
#define HAL_NANO11_h
//-------------------------------------------------------------
//INCLUDES
#include "HAL_interface.h"
#include "BPLC_I2C_Nodes.h"
#include "SpecialFunctions.h"
//-------------------------------------------------------------
//Card definition
#define NANO11_ADDRESS_COUNT 10
#define NANO11_CHANNEL_COUNT 8      //Natürlich erweiterbar, der Databuffer von Slavedata muss aber erweitert werden!
const uint8_t NANO11_I2C_ADDRESSES[NANO11_ADDRESS_COUNT] = {0xB0, 0xB1, 0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xB7, 0xB8, 0xB9};

typedef enum
{
    NANO11_COMMAND_KEY__WRITE_CHANNEL,          //Channel schreiben
    NANO11_COMMAND_KEY__SET_CHANNEL_TO_READ,   //Channel lesen

    NANO11_COMMAND_KEY__COUNT
}e_NANO11_COMMAND_KEY_t;

#pragma pack (push, 1)
typedef union 
{
    struct 
    {
        uint8_t key;
        uint8_t channel;
        uint8_t payload[12];
    }extract;
    
    uint8_t data[14];
    
}s_NANO11_COMMAND_t;
#pragma pack (pop)
//-------------------------------------------------------------
//Normale HAL für BPLC Master um mit NANO11 zu kommunizieren
class HAL_NANO11: public halInterface, private BPLC_moduleErrorHandler, private BPLC_logPrint, private I2C_check
{
    public:
    //Hal constructor
                    HAL_NANO11              ();
    //Hal interface 
    bool            init                    (const e_EC_ADDR_t ADDR);
    bool            mapObjectToChannel      (IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL);        
    void            tick                    (const bool READ_INPUTS);        
    void            controlCommand          (const e_EC_COMMAND_t COMMAND);   
    //Modul Error Interface   
    uint8_t         getModuleErrorCount                 ()                                                  {return this->getErrorCount();}
    e_BPLC_ERROR_t  getModuleErrorCode                  (uint8_t ERROR_NUMBER)                              {return this->getError(ERROR_NUMBER)->errorCode;}
    void            resetAllModuleErrors                (String FILE, const uint16_t LINE)                  {this->resetAllErrors(FILE, LINE);}
    void            setSuperiorErrorHandlerForModule    (BPLC_moduleErrorHandler* P_SUPERIOR_ERROR_MANAGER) {}//this->setSuperiorErrorHandler(P_SUPERIOR_ERROR_MANAGER);}
    

    private:          
    //Settings  
    I2C_BPLC_Master     bplcNode;
  
    //Object handling
    struct
    {
        uint8_t         channelCount;
        IO_Interface*   p_ioObject  [NANO11_CHANNEL_COUNT];    
    }channels;        
 };

//-------------------------------------------------------------
//Lib auf Atmega um mit BPLC Master zu kommunizieren
class LIB_NANO11
{
    public:
    //Hal constructor
                    LIB_NANO11              ();
    //Hal interface 
    void            begin                   (const e_EC_ADDR_t ADDR);
    void            mapPinToChannel         (const uint8_t PIN, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE);   //pins direkt auf channel mappen, geht nur mit einfachen io objekten 
    void            mapHalDataToChannel     (u_HAL_DATA_t* P_DATA, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE);
    void            tick                    ();        
    bool            getOEN                  (){return digitalRead(this->pins.OEN);}


    private:    
    void            initInternals           ();      
   
    //Settings  
    I2C_BPLC_Slave      bplcNode;
    bool                error;

    //INT
    output intOutput;

    //Object handling
    struct
    {
        e_IO_TYPE_t     ioType;
        uint8_t         pin;
        u_HAL_DATA_t    data;    
        u_HAL_DATA_t*   p_externalData;  
        bool            f_newDataAvailable;
    }channels[NANO11_CHANNEL_COUNT];     
    
    struct 
    {
        const uint8_t OEN = 8;
        const uint8_t INT = 9;       
    }pins;    
 };
#endif