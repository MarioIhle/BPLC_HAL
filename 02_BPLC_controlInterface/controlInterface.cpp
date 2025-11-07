#include "BPLC_controlInterface.h"

BPLC_controlInterface::BPLC_controlInterface()
{

}
void BPLC_controlInterface::begin(const uint8_t PREFIX, const uint8_t SUFFIX)
{
    this->prefix    = PREFIX;
    this->suffix    = SUFFIX;
}
bool BPLC_controlInterface::commandAvailable()
{
    if(Serial.available())
    {      
        //Host heartbeat
        this->to_keepAlive.reset();

        u_BPLC_PLI_COMMAND_t inCommand;
        memset(&inCommand, 0, sizeof(u_BPLC_PLI_COMMAND_t));

        if(Serial.peek() == this->prefix)
        {
            uint8_t startFrame              = Serial.read();
            inCommand.command.key           = Serial.read();
            inCommand.command.payload       = Serial.read();
            uint8_t endFrame                = Serial.read();          

            if (startFrame == this->prefix && endFrame == this->suffix)
            {                
                this->mailbox.createNewElement(&inCommand.data[0], sizeof(u_BPLC_PLI_COMMAND_t));              
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
