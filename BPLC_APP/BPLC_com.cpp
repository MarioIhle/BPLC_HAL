#include "BPLC_APP.h"

void BPLC_APP::mapObjectToComPort(BPLC_COM_Port* P_PORT)
{
    for(uint8_t PORT = 0; PORT<256; PORT++)
    {
        const bool POINTER_NOT_DEFINED = (bool)(this->p_ComPorts[PORT] == NULL);
        
        if(POINTER_NOT_DEFINED)
        {
            this->p_ComPorts = P_PORT;
            return;
        }
    }   
}