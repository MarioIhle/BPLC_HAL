#ifndef BPLC_controlInterface_h
#define BPLC_controlInterface_h

#include "Arduino.h"
#include "SpecialFunctions.h"
#include "BPLC_logPrint.h"

typedef union 
{
    struct 
    {
        uint8_t key;
        uint8_t payload;
    }command;
    
    uint8_t data[sizeof(command)];    

}u_BPLC_PLI_COMMAND_t;

class BPLC_controlInterface: BPLC_logPrint
{
    public:
                            BPLC_controlInterface   ();
    void                    begin                   (const uint8_t PREFIX, const uint8_t SUFFIX);
    bool                    commandAvailable        ();
    u_BPLC_PLI_COMMAND_t    getCommand              ();
    bool                    available               (){return (!this->to_keepAlive.check());}


    private:
    fifoBuffer          mailbox;  
    Timeout             to_keepAlive;    
    uint8_t             prefix;
    uint8_t             suffix;  
};
#endif