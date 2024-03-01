#ifndef BPLC_LOG_h
#define BPLC_LOG_h
#include "Arduino.h"
//BPLC_ParameterLogInterface


typedef enum
{
    BPLC_PLI_KEY__DEVICE_MODE_STOP,
    BPLC_PLI_KEY__DEVICE_MODE_RUN,
    BPLC_PLI_KEY__DEVICE_MODE_RUN_WITHOUT_SAFETY,
    BPLC_PLI_KEY__GET_SYSTEM_ERRORS,

    BPLC_PLI_KEY__DEFINE_MCU11revA,
    BPLC_PLI_KEY__DEFINE_MCU11revB,
    BPLC_PLI_KEY__DEFINE_MCU11revC,

    BPLC_PLI_KEY__ADD_EC_DIN11revA_ADDR_1,
    BPLC_PLI_KEY__ADD_EC_DIN11revA_ADDR_2,
    BPLC_PLI_KEY__ADD_EC_DIN11revA_ADDR_3,
    BPLC_PLI_KEY__ADD_EC_DIN11revA_ADDR_4,

    BPLC_PLI_KEY__ADD_EC_AIN11revA_ADDR_1,
    BPLC_PLI_KEY__ADD_EC_AIN11revA_ADDR_2,
    BPLC_PLI_KEY__ADD_EC_AIN11revA_ADDR_3,
    BPLC_PLI_KEY__ADD_EC_AIN11revA_ADDR_4,
    BPLC_PLI_KEY__COUNT,

}e_BPLC_PLI_KEY_t;

typedef struct 
{
    uint16_t key;
    uint16_t paramValue;

}s_BPLC_PLI_COMMAND_t;


class BPLC_controlInterface:BPLC_LOG
{
    public:
            BPLC_controlInterface();
    void    tick();
    void    setCallbackForKey(const e_BPLC_PLI_KEY_t KEY, void (*callback)());


    private:
    void*   callbackFunction[BPLC_PLI_KEY__COUNT];
};

class BPLC_LOG
{
    public:
    BPLC_LOG();
    void printLog(String TEXT);
    void printError(String TEXT);
    void printResetReason(String MODUL, String FUNKTION, String REASON);


    private:

};
#endif