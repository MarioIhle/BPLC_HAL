#include "BPLC_I2C_Nodes.h"




bool I2C_BPLC_Master::requestACK(const uint8_t SLAVE_ADDRESS)
{
    uint8_t inByte = NAK;
    
    this->comNode.sendFrame()
    Wire.requestFrom(SLAVE_ADDRESS, 1); 

    Timeout to_receiveTimeout(50);

    while(to_receiveTimeout.check() == false)
    {   
        while(Wire.available()) 
        {       
            inByte = Wire.read();    
        }
    	//Fast exit wenn Kommando empfangen 
        if(inByte == ACK)
        {
            break;
        }  
    }

    return (bool)(inByte == ACK);
}