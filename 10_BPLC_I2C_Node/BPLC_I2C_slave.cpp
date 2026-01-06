#include "BPLC_I2C_Nodes.h"

I2C_BPLC_Slave::I2C_BPLC_Slave()
{}
void I2C_BPLC_Slave::begin(const uint8_t ADDRESS)
{
    this->i2cNode.begin(ADDRESS);
    
    //Slave Datenpaktet initialisieren
    for(uint8_t packet = 0; packet < SLAVE_DATA_PACKETS; packet++)
    {
        memset(this->slaveDataPackets[packet].data, 0, sizeof(u_I2C_BPLC_NODE_FRAME_t));
        this->i2cNode.setPacketPointer(packet, &this->slaveDataPackets[packet]);
    }
}
bool I2C_BPLC_Slave::newCommandAvailable()
{
    return this->i2cNode.newSlaveCommandReceived();   
}
uint8_t I2C_BPLC_Slave::getCommand(uint8_t* P_PAYLOADBUFFER)
{         
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->i2cNode.getFrame();  
    memcpy(P_PAYLOADBUFFER, NEW_FRAME.extract.payload, NEW_FRAME.extract.payloadSize);

    return NEW_FRAME.extract.payloadSize;
}
bool I2C_BPLC_Slave::setSlaveData(const uint8_t DATA_PACKET, uint8_t* BUFFER, const uint8_t SIZE)
{    
    bool error = false;

    if(SIZE > PAYLAOD_BYTES_MAX)
    {
        Serial.println("slave data too big");
        error = true;        
    } 
    else
    {
        memset(this->slaveDataPackets[DATA_PACKET].data, 0, sizeof(u_I2C_BPLC_NODE_FRAME_t));
        this->slaveDataPackets[DATA_PACKET].extract.i2cBplcKey    = I2C_BPLC_KEY__SLAVE_DATA;
        this->slaveDataPackets[DATA_PACKET].extract.payloadSize   = SIZE;  
        memcpy(&this->slaveDataPackets[DATA_PACKET].extract.payload[0], BUFFER, SIZE);

        /*
        Serial.println("Set Slave packet: " + String(DATA_PACKET));
        for(uint8_t i = 0; i < 30; i++)
        {
            Serial.print(this->slaveDataPackets[DATA_PACKET].extract.payload[i]);
        }
        Serial.println("");
        */
    }
    return error;
}