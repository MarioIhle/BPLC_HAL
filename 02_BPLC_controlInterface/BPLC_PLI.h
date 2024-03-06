#ifndef BPLC_LOG_h
#define BPLC_LOG_h
#include "Arduino.h"
#include "SpecialFunctions.h"
#include "BPLC_ERRORS.h"

typedef enum
{   
    //System
    BPLC_PLI_KEY__GET_SYSTEM_ERRORS,
    BPLC_PLI_KEY__RESET_ALL_ERRORS,
    BPLC_PLI_KEY__GET_DEVICE_SETTINGS,
    BPLC_PLI_KEY__RESET_ALL_SETTINGS,
    BPLC_PLI_KEY__SOFT_RESET,
    BPLC_PLI_KEY__INVERT_ENCODER,
    BPLC_PLI_KEY__USE_BUZZER,
    BPLC_PLI_KEY__BEEP_ON_ENCODER_INPUT,

    //Applikation
    BPLC_PLI_KEY__DEVICE_MODE_STOP = 0x10,
    BPLC_PLI_KEY__DEVICE_MODE_RUN,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_SAFETY,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_HARDWARE,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_COMMUNICATION,

    //Kommunikation
    BPLC_PLI_KEY__SET_DEVICE_ADDRESS = 0x20,

    //Hardware
    BPLC_PLI_KEY__DEFINE_MCU = 0x30,
    BPLC_PLI_KEY__ADD_EC_AIN11revA,
    BPLC_PLI_KEY__ADD_EC_DIN11revA,
    BPLC_PLI_KEY__ADD_EC_DO11revA,
    BPLC_PLI_KEY__ADD_EC_REL11revA,
    BPLC_PLI_KEY__ADD_EC_MOT111revA,
    BPLC_PLI_KEY__ADD_EC_TMP11revA,
    BPLC_PLI_KEY__ADD_EC_PPO11revA,
    BPLC_PLI_KEY__ADD_EC_NANOrevA,
    BPLC_PLI_KEY__ADD_EC_FUSE12revA,

    BPLC_PLI_KEY__COUNT,

}e_BPLC_PLI_KEY_t;

typedef union 
{
    struct 
    {
        uint8_t key;
        uint8_t paramValue;
    }command;
    
    uint8_t data[sizeof(command)];    

}u_BPLC_PLI_COMMAND_t;


class BPLC_LOG: ERROR_OUT
{
    public:
            BPLC_LOG            ();
    void    printLog            (String TEXT, String FILE, const uint16_t LINE);
    void    printError          (String TEXT, String FILE, const uint16_t LINE);
    void    printResetReason    (String TEXT, String FILE, const uint16_t LINE);


    private:
};

#define hostStartFrame  36  //ASCII $
#define hostEndFrame    37  //ASCII %

class BPLC_controlInterface: BPLC_LOG
{
    public:
                            BPLC_controlInterface  ();
    bool                    commandAvailable       ();
    u_BPLC_PLI_COMMAND_t    getCommand             ();
    bool                    available              (){return (bool)(this->to_keepAlive.check() == false);}


    private:
    fifoBuffer          mailbox;  
    Timeout             to_keepAlive;      
};
#endif