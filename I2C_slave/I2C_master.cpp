#include "BPLC_I2C_Nodes.h"

void I2C_BPLC_Master::begin()
{
    this->comNode.begin();
}
uint8_t I2C_BPLC_Master::getSlaveData(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{    
    Timeout to_receiveTimeout(500);

    //Key senden, damit Slave Daten sendet
    this->comNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__REQUEST_SLAVE_DATA, nullptr, 0);     
    //Auf Daten warten
   
    s_I2C_BPLC_NODE_FRAME_t NEW_FRAME;  
    uint8_t buffer[32];
    int BYTE = 0;
    memset(buffer, 0, 32);
    memset(NEW_FRAME.frame.data, 0, 32);
    
    Wire.requestFrom(SLAVE_ADDRESS, BYTE_COUNT);

    while(to_receiveTimeout.check() == false)
    {   
        
        if(Wire.available())
        {    
                 
            buffer[BYTE] = Wire.read();  
            Serial.print(buffer[BYTE]); 
            BYTE++;            
        }

        /*
        if(I2C_BPLC_KEY__SLAVE_DATA == this->comNode.newFrameReceived())
        {
            s_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->comNode.getFrame();  
            memcpy(P_DATA_BUFFER, NEW_FRAME.frame.data, NEW_FRAME.frameSize);
        
            return NEW_FRAME.frameSize;
        }  
        */
    } 
    
    memcpy(NEW_FRAME.frame.extract.payload, buffer, BYTE);
    Serial.println();
    return 0;
}
bool I2C_BPLC_Master::sendCommand(const uint8_t SLAVE_ADDRESS, uint8_t* P_DATA_BUFFER, const uint8_t BYTE_COUNT)
{
    this->comNode.sendFrame(SLAVE_ADDRESS, I2C_BPLC_KEY__SLAVE_COMMAND, P_DATA_BUFFER, BYTE_COUNT);
    
    Timeout to_waitForACK(50);

    while (to_waitForACK.check() == false)
    {        
        if (I2C_BPLC_KEY__ACK == this->comNode.newFrameReceived())
        {
            return true;
        }
        else
        {            
            //error?
        }        
    }   
    return false; 
}