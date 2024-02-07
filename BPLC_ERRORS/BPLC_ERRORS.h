#ifndef BPLC_ERRORS_h
#define BPLC_ERRORS_h
#include "Arduino.h"
//Bei änderungen immer auch MOT11 Firmware updaten, da sonst Fehler falsch interpretiert werden!!!
//case BPLC ERROR TYPE
typedef enum
{    
    BPLC_ERROR__NO_ERROR,
    //MCU Errors
    BPLC_ERROR__NO_MCU_DEFINED,
    BPLC_ERROR__BPLC_BEGIN_CALL_MISSING,
    BPLC_ERROR__OLED_COMMUNICATION_FAILED,
    BPLC_ERROR__RUNNTIME,
    BPLC_ERROR__COMMUNICATION_FAILED,
    BPLC_ERROR__ADDRESS_NOT_DEFINED,
    //DIN11 errors
    DIN11_ERROR__I2C_CONNECTION_FAILED, 
    DIN11_ERROR__NO_CHANNEL_IN_USE,
    DIN11_ERROR__CHANNEL_ALREADY_IN_USE,
    DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,   
    DIN11_ERROR__CHANNEL_OUT_OF_RANGE,
    DIN11_ERROR__IO_OBJECT_NOT_SUITABLE,
    //AIN11 errors
    AIN11_ERROR__I2C_CONNECTION_FAILED, 
    AIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    AIN11_ERROR__CARD_NOT_DEFINED,
    AIN11_ERROR__NO_CHANNEL_IN_USE,
    AIN11_ERROR__CHANNEL_ALREADY_IN_USE,
    AIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,
    //REL11 errors
    REL11_ERROR__I2C_CONNECTION_FAILED, 
    REL11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    REL11_ERROR__CARD_NOT_DEFINED,
    REL11_ERROR__NO_CHANNEL_IN_USE,
    REL11_ERROR__CHANNEL_ALREADY_IN_USE,
    REL11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,
    //DO11 errors
    DO11_ERROR__I2C_CONNECTION_FAILED, 
    DO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    DO11_ERROR__CARD_NOT_DEFINED,
    DO11_ERROR__NO_CHANNEL_IN_USE,
    DO11_ERROR__CHANNEL_ALREADY_IN_USE,
    DO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,
    //FUSE11 errors
    FUSE11_ERROR__I2C_CONNECTION_FAILED, 
    FUSE11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    FUSE11_ERROR__CARD_NOT_DEFINED,
    FUSE11_ERROR__NO_CHANNEL_IN_USE,
    FUSE11_ERROR__CHANNEL_ALREADY_IN_USE,
    FUSE11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,
    //NANO11 errors
    NANO11_ERROR__I2C_CONNECTION_FAILED, 
    NANO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    NANO11_ERROR__CARD_NOT_DEFINED,
    NANO11_ERROR__NO_CHANNEL_IN_USE,
    NANO11_ERROR__CHANNEL_ALREADY_IN_USE,
    NANO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,
    //MOT11 errors
    MOT11_ERROR__I2C_CONNECTION_FAILED,
    MOT11_ERROR__CARD_FOUND_BUT_NOT_DEFINED,
    MOT11_ERROR__CARD_NOT_DEFINED,
    MOT11_ERROR__NO_CHANNEL_IN_USE,
    MOT11_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE, 
    MOT11_ERROR__OVER_CURRENT_125_10s,
    MOT11_ERROR__OVER_CURRENT_200_2s,
    MOT11_ERROR__OVER_CURRENT_500_0s,
    MOT11_ERROR__OVER_TEMPERATURE,   
    MOT11_ERROR__CURRENT_NOT_TEACHED, 
    MOT11_ERROR__OEN_DISABLED,
    MOT11_ERROR__CHANNEL_ALREADY_IN_USE,
    MOT11_ERROR__ALL_CHANNELS_ALREADY_IN_USE,

    BPLC_ERROR__COUNT,
}e_BPLC_ERROR_t;

class BPLC_errorHandler
{
    public:
    e_MASTER_NODE_ERROR_t   getError(){return this->errorCode;}
    void                    setError(const e_BPLC_ERROR_t ERROR_CODE)
    {
        //Nur erster Error bleibt gespeichert
        if(this->errorCode == BPLC_ERROR__NO_ERROR)
        {
            this->errorCode = ERROR_CODE;
        }
    }
    void                    resetError(const e_BPLC_ERROR_t ERROR_CODE)
    {
        //Nur error zurück setzten, wenn auch dieser gesetzt war
        if(ERROR_CODE == this->errorCode)
        {
            this->errorCode = BPLC_ERROR__NO_ERROR;
        }
    }

    private:
    e_BPLC_ERROR_t errorCode
};

class ERROR_OUT
{
    public:
    ERROR_OUT(){};

    String ERROR_TEXT[BPLC_ERROR__COUNT]=
    {
        {"BPLC_ERROR__NO_ERROR"},
        //BPLC Errors
        {"BPLC_ERROR__NO_MCU_DEFINED"},
        {"BPLC_ERROR__BPLC_BEGIN_CALL_MISSING"},
        {"BPLC_ERROR__OLED_COMMUNICATION_FAILED"},
        {"BPLC_ERROR__RUNNTIME"},
        {"BPLC_ERROR__COMMUNICATION_FAILED"},
        {"BPLC_ERROR__ADDRESS_NOT_DEFINED"},
        //DIN11 errors
        {"DIN11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"DIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"DIN11_ERROR__CARD_NOT_DEFINED"},
        {"DIN11_ERROR__NO_CHANNEL_IN_USE"},
        {"DIN11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},   
        //AIN11 errors
        {"AIN11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"AIN11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"AIN11_ERROR__CARD_NOT_DEFINED"},
        {"AIN11_ERROR__NO_CHANNEL_IN_USE"},
        {"AIN11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"AIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},
        //REL11 errors
        {"REL11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"REL11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"REL11_ERROR__CARD_NOT_DEFINED"},
        {"REL11_ERROR__NO_CHANNEL_IN_USE"},
        {"REL11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"REL11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},
        //DO11 errors
        {"DO11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"DO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"DO11_ERROR__CARD_NOT_DEFINED"},
        {"DO11_ERROR__NO_CHANNEL_IN_USE"},
        {"DO11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"DO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},
        //FUSE11 errors
        {"FUSE11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"FUSE11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"FUSE11_ERROR__CARD_NOT_DEFINED"},
        {"FUSE11_ERROR__NO_CHANNEL_IN_USE"},
        {"FUSE11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"FUSE11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},
        //NANO11 errors
        {"NANO11_ERROR__I2C_CONNECTION_FAILED"}, 
        {"NANO11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"NANO11_ERROR__CARD_NOT_DEFINED"},
        {"NANO11_ERROR__NO_CHANNEL_IN_USE"},
        {"NANO11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"NANO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"},
        //MOT11 errors
        {"MOT11_ERROR__I2C_CONNECTION_FAILED"},
        {"MOT11_ERROR__CARD_FOUND_BUT_NOT_DEFINED"},
        {"MOT11_ERROR__CARD_NOT_DEFINED"},
        {"MOT11_ERROR__NO_CHANNEL_IN_USE"},
        {"MOT11_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE"}, 
        {"MOT11_ERROR__OVER_CURRENT_125_10s"},
        {"MOT11_ERROR__OVER_CURRENT_200_2s"},
        {"MOT11_ERROR__OVER_CURRENT_500_0s"},
        {"MOT11_ERROR__OVER_TEMPERATURE"},   
        {"MOT11_ERROR__CURRENT_NOT_TEACHED"}, 
        {"MOT11_ERROR__OEN_DISABLED"},
        {"MOT11_ERROR__CHANNEL_ALREADY_IN_USE"},
        {"MOT11_ERROR__ALL_CHANNELS_ALREADY_IN_USE"}
    };

    String getErrorCodeText(const e_BPLC_ERROR_t ERROR_CODE)
    {      
        if(BPLC_ERROR__COUNT != 56)
        {
            return("BPLC_ERROR__ERROR_TEXTS_NEED_UPDATE!");
        }
        else
        {
            return ERROR_TEXT[ERROR_CODE];
        }          
    }       
};
#endif