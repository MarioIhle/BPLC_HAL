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
    const bool LAST_COMMAND_NOT_PROCESSED = p_i2cNode->newSlaveCommandReceived();
    //Letztes Kommando wurde verarbeitet-> neues Kommando speichern  
    if(LAST_COMMAND_NOT_PROCESSED)
    {
        Serial.println("Kommando empfangen, kann nicht gespeichert werden, da Buffer voll");
    }
    else
    {
        p_i2cNode->handleNewFrame(&callback_inBuffer);
    }
}
void requestCallback()
{    
    uint8_t* p_packetSize = nullptr;
    uint8_t* p_dataBuffer = p_i2cNode->getSlaveDataPacket(p_packetSize);
    
    if(p_dataBuffer != nullptr)
    {       
        for(uint8_t BYTE = 0; BYTE < *p_packetSize; BYTE++)
        {
            //Serial.println(p_dataBuffer->data[BYTE]);
            Wire.write(p_dataBuffer[BYTE]); 
        } 
    }
    else
    {
        Serial.println("Slave Data Buffer is nullptr");
    }
}
BPLC_I2C_NODE::BPLC_I2C_NODE()
{}
void BPLC_I2C_NODE::begin(const uint8_t NODE_ADDRESS, uint8_t* p_slaveDataBuffer)
{
    this->nodeAddress = NODE_ADDRESS; 
    //I2C Kommunikation aufbauen
    if(p_slaveDataBuffer == nullptr)
    {
        Wire.begin();
    }
    else
    {
        //Slave Konfig
        Wire.begin(NODE_ADDRESS);
        Wire.onReceive(receiveCallback);
        Wire.onRequest(requestCallback); 
        this->request.p_dataBuffer = p_slaveDataBuffer; 
    }
    //I2C taktfrequenz auf 400kHz erhöhen
    Wire.setClock(400000);

    //Callback Pointer   
    p_i2cNode = this;
}
void BPLC_I2C_NODE::sendFrame(const e_I2C_BPLC_KEY_t KEY, const uint8_t* P_PAYLOAD, const uint8_t PAYLOAD_SIZE)
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
    
    Wire.beginTransmission(this->nodeAddress);
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

        case I2C_BPLC_KEY__SET_REQUESTED_PAYLOADSIZE:
        case I2C_BPLC_KEY__SET_REQUESTED_PACKET:
            this->request.packet        = p_newFrame->extract.payload[0];
            this->request.packetSize    = p_newFrame->extract.payload[1];
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
u_I2C_BPLC_NODE_FRAME_t BPLC_I2C_NODE::getCommand()
{
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->command.frame;
    memset(&this->command.frame, 0, sizeof(this->command.frame));
    this->command.f_newFrameReceived = false;

    return NEW_FRAME;
}
uint8_t BPLC_I2C_NODE::requestFromNode(uint8_t* p_payloadBuffer, const uint8_t PAYLOAD_SIZE)
{
    const   uint8_t BYTES_REQUESTED     = this->request.packetSize;   //PAYLOAD_SIZE + i2cBplcKey + palyoadSize    
    const   uint8_t PACKETS_TO_RECEIVE  = (BYTES_REQUESTED / MAX_FRAME_SIZE);
   
    uint8_t inByte = 0;    
    this->request.packet = 0;
    //alle Bytes erhalten 
    while ((inByte < BYTES_REQUESTED)
        //buffer überlauf vermeiden 
        || (inByte == 255))
    {    
        //Immer in 32byte Paketen von Slavenode anfragen, da i2c Buffer nicht größer
        if((inByte == 0)
        || (inByte == 32)
        || (inByte == 64)
        || (inByte == 96)
        || (inByte == 128)
        || (inByte == 160)
        || (inByte == 192)
        || (inByte == 224))
        {     
            const uint8_t PACKET            = (inByte / MAX_FRAME_SIZE);              
            const bool REQUEST_NEXT_PACKET  = (this->request.packet != PACKET);

            if(REQUEST_NEXT_PACKET)       
            {                          
                //Berechnen wie viele Bytes in diesem Packet ankommen  
                uint8_t requestedBytes = 0;
                //Insgemant weniger als 32 bytes zu empfangen
                if(BYTES_REQUESTED <= MAX_FRAME_SIZE)
                {
                    requestedBytes = BYTES_REQUESTED;
                }
                //Insgemant und in diesem Packet mehr als 32bytes
                else if(PACKET < PACKETS_TO_RECEIVE)
                {
                    requestedBytes = MAX_FRAME_SIZE;
                }
                //Insgemant mehr als 32ybtes, in diesem Packet aber weniger als 32 bytes zu empfangen
                else
                {
                    requestedBytes = (BYTES_REQUESTED % (PACKET * MAX_FRAME_SIZE)); 
                }
                const uint8_t PACKET_DEFINITION[2] = {PACKET, requestedBytes};

                this->sendFrame(I2C_BPLC_KEY__SET_REQUESTED_PACKET, &PACKET_DEFINITION[0], 2);
                Wire.requestFrom(this->nodeAddress, requestedBytes);   
                this->request.packet = PACKET;
            }
        }

        while(Wire.available())
        {      
            p_payloadBuffer[inByte] = Wire.read(); 
            //Serial.println("byte: " + String(inBytes) + ": "+ String(slaveDataBuffer.data[inBytes]));
            inByte++;
        }        
    }
    return inByte;
}