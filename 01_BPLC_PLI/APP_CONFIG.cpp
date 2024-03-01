#include "BPLC_PLI.h"

BPLC_controlInterface::BPLC_controlInterface()
{}
void BPLC_controlInterface::setCallbackForKey(const e_BPLC_PLI_KEY_t KEY, void (*callback)())
{
    this->callbackFunction[KEY] = callback;
}
void BPLC_controlInterface::tick()
{
    uint8_t                 startFrame  = 0;
    s_BPLC_PLI_COMMAND_t    inCommand;
    uint8_t                 endFrame    = 0;
        
    if(Serial.available())
    {   
        startFrame              = Serial.read();
        inCommand.key           = Serial.read();
        inCommand.paramValue    = Serial.read();
        endFrame                = Serial.read();    
    }

    if (startFrame == 55 && endFrame == 55)
    {
        const e_BPLC_PLI_KEY_t KEY = (e_BPLC_PLI_KEY_t)inCommand.key;

        if(KEY < BPLC_PLI_KEY__COUNT)
        {
            this->callbackFunction[KEY](inCommand.paramValue);
        }
        else
        {
            this->printLog("COMMAND KEY INVALID!")
        }   
    }
}