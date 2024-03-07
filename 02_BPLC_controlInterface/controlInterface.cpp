#include "BPLC_controlInterface.h"

BPLC_controlInterface::BPLC_controlInterface()
{}
bool BPLC_controlInterface::commandAvailable()
{
    while(Serial.available())
    {      
        //Host heartbeat
        this->to_keepAlive.reset();

        u_BPLC_PLI_COMMAND_t inCommand;
        memset(&inCommand, 0, sizeof(u_BPLC_PLI_COMMAND_t));

        uint8_t startFrame              = Serial.read();
        inCommand.command.key           = Serial.read();
        inCommand.command.paramValue    = Serial.read();
        uint8_t endFrame                = Serial.read();    

        if (startFrame == hostStartFrame && endFrame == hostEndFrame)
        {
            const e_BPLC_PLI_KEY_t KEY = (e_BPLC_PLI_KEY_t)inCommand.command.key;

            if(KEY < BPLC_PLI_KEY__COUNT)
            {
                this->mailbox.createNewElement(&inCommand.data[0], sizeof(u_BPLC_PLI_COMMAND_t));              
            }
            else
            {
                this->printLog("RECEIVED COMMAND KEY INVALID!", __FILENAME__, __LINE__);
            }   
        }
    }
    return this->mailbox.isThereSomethingInTheBuffer();
}
u_BPLC_PLI_COMMAND_t BPLC_controlInterface::getCommand()
{
    u_BPLC_PLI_COMMAND_t command;
    this->mailbox.getNextElement(&command.data[0], sizeof(u_BPLC_PLI_COMMAND_t));
    this->mailbox.flushLastElementAndMoveToNext();
    return command;
}
