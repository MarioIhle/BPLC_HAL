#include "BPLC_I2C_Nodes.h"

BPLC_I2C_NODE* p_i2cNode;

void receiveCallback(int howMany)
{
    u_I2C_BPLC_NODE_FRAME_t callback_inBuffer;
    memset(&callback_inBuffer, 0, sizeof(callback_inBuffer));

    while(Wire.available())
    {
        for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
        {    
            callback_inBuffer.data[BYTE] = Wire.read();    
            //Serial.print(callback_inBuffer.data[BYTE]);
        }
    }             
    p_i2cNode->handleNewFrame(&callback_inBuffer);
}
void requestCallback()
{  
    const s_slaveDataPaketDefinition_t  SLAVE_DATA_DEFINITION    = p_i2cNode->getSlaveDataDefinition();
    const uint8_t                       BYTES_REQUESTED          = (SLAVE_DATA_DEFINITION.payloadSize + MESSAGE_HEAD);
    const uint8_t                       PACKET                   = SLAVE_DATA_DEFINITION.index;
    u_I2C_BPLC_NODE_FRAME_t*            p_dataBuffer             = p_i2cNode->getSlaveDataBuffer(PACKET);
    
    if(p_dataBuffer != nullptr)
    {       
        for(uint8_t BYTE = 0; BYTE < BYTES_REQUESTED; BYTE++)
        {
            //Serial.println(p_dataBuffer->data[BYTE]);
            Wire.write(p_dataBuffer->data[BYTE]); 
        } 
    }
    else
    {
        Serial.println("Slave Data Buffer is nullptr");
    }
}
BPLC_I2C_NODE::BPLC_I2C_NODE()
{}
void BPLC_I2C_NODE::begin(const uint8_t ADDRESS)
{
    //I2C Kommunikation aufbauen
    if(ADDRESS != 0)
    {
        Wire.begin(ADDRESS);
    }
    //Pointer init
    for(uint8_t ptr = 0; ptr < SLAVE_DATA_PACKETS; ptr++)
    {
        this->request.p_slaveDataBuffer[ptr] = nullptr;
    }
    //I2C taktfrequenz auf 400kHz erhöhen
    Wire.setClock(400000);   

    Wire.onReceive(receiveCallback);
    Wire.onRequest(requestCallback);    

    p_i2cNode = this;
}
void BPLC_I2C_NODE::setPacketPointer(const uint8_t PACKET, u_I2C_BPLC_NODE_FRAME_t* p_dataPacket)
{
    if(this->request.p_slaveDataBuffer[PACKET] == nullptr)
    {
        this->request.p_slaveDataBuffer[PACKET] = p_dataPacket;
        //this->printLog(String(PACKET) + " Packet Pointer set", __FILENAME__, __LINE__);
    }
    else
    {
        this->printLog("Packet Pointer already set!", __FILENAME__, __LINE__);
    }
}   
void BPLC_I2C_NODE::sendFrame(const uint8_t DESTINATION_ADDRESS, const e_I2C_BPLC_KEY_t KEY, const uint8_t* P_PAYLOAD, const uint8_t PAYLOAD_SIZE)
{
    u_I2C_BPLC_NODE_FRAME_t OUT_FRAME;
    memset(&OUT_FRAME, 0, sizeof(OUT_FRAME));

    OUT_FRAME.extract.i2cBplcKey    = (uint8_t)KEY;
    OUT_FRAME.extract.payloadSize   = PAYLOAD_SIZE;

    if(P_PAYLOAD != nullptr)
    {        
        memcpy(OUT_FRAME.extract.payload, P_PAYLOAD, PAYLOAD_SIZE);
    }   
   
    /*
    Serial.println("destiantion Adress: " + String(DESTINATION_ADDRESS));
    Serial.println("key: " + String(OUT_FRAME.extract.i2cBplcKey));
    Serial.println("size: " + String(OUT_FRAME.extract.payloadSize));
    Serial.println("payload: ");
    for(int i = 0; i< (PAYLOAD_SIZE); i++)
    {
        Serial.print(OUT_FRAME.extract.payload[i]);
    }
    Serial.println("");
    Serial.print("frame: ");
    for(int i = 0; i< (PAYLOAD_SIZE + MESSAGE_HEAD); i++)
    {
        Serial.print(OUT_FRAME.data[i]);
    }
    Serial.println("");
    */
    
    Wire.beginTransmission(DESTINATION_ADDRESS);
    Wire.write(OUT_FRAME.data, (MESSAGE_HEAD + PAYLOAD_SIZE));            
    Wire.endTransmission(true);          
}
void BPLC_I2C_NODE::handleNewFrame(u_I2C_BPLC_NODE_FRAME_t* p_newFrame)
{
    switch (p_newFrame->extract.i2cBplcKey)
    {
        case I2C_BPLC_KEY__SLAVE_COMMAND:
            memcpy(&this->command.frame, p_newFrame, sizeof(u_I2C_BPLC_NODE_FRAME_t));
            this->command.f_newFrameReceived = true;
            break;                

        case I2C_BPLC_KEY__SET_REQUESTED_CHANNEL:
            this->request.requestedData.index       = p_newFrame->extract.payload[0];
            this->request.requestedData.payloadSize = p_newFrame->extract.payload[1];
            //Serial.println("Request Packet set to : "+ String(this->request.requestedData.index));
            //Serial.println("Request payloadSize set to : "+ String(this->request.requestedData.payloadSize));
            break;

        default:
        case I2C_BPLC_KEY__NO_KEY:              
        case I2C_BPLC_KEY__REQUEST_SLAVE_DATA:   
        case I2C_BPLC_KEY__SLAVE_DATA:    
            //nichts tun    
            break;
    }
}
bool BPLC_I2C_NODE::newSlaveCommandReceived()
{
    return this->command.f_newFrameReceived;
}
u_I2C_BPLC_NODE_FRAME_t BPLC_I2C_NODE::getFrame()
{
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->command.frame;
    memset(&this->command.frame, 0, sizeof(this->command.frame));
    this->command.f_newFrameReceived = false;

    return NEW_FRAME;
}
void BPLC_I2C_NODE::requestFromSlave(const uint8_t SLAVE_ADDRESS, uint8_t* p_payloadBuffer, const uint8_t PAYLOAD_SIZE)
{
    const uint8_t BYTES_REQUESTED = (PAYLOAD_SIZE + MESSAGE_HEAD);   //PAYLOAD_SIZE + i2cBplcKey + palyoadSize
    
    if(BYTES_REQUESTED > MAX_FRAME_SIZE)
    {
        this->printLog("PAYLOADSIZE TO BIG", __FILENAME__, __LINE__);
    }
    else
    {   
        uint8_t inBytes = 0;
        u_I2C_BPLC_NODE_FRAME_t slaveData;
        memset(&slaveData, 0, sizeof(slaveData));
        Wire.requestFrom(SLAVE_ADDRESS, BYTES_REQUESTED);               

        while(Wire.available())
        {      
            slaveData.data[inBytes] = Wire.read(); 
            //Serial.println("byte: " + String(inBytes) + ": "+ String(slaveData.data[inBytes]));
            inBytes++;
        }        
        memcpy(p_payloadBuffer, slaveData.extract.payload, PAYLOAD_SIZE); 
    }
}