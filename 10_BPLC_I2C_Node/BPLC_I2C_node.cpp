#include "BPLC_I2C_Nodes.h"

BPLC_I2C_NODE* p_i2cNode;

void receiveCallback(int howMany)
{
    //Serial.println("recevie Callback");
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
    //Serial.println("request Callback");
    const uint8_t FIRST_BYTE      = p_i2cNode->getFirstByte();     
    const uint8_t LAST_BYTE       = p_i2cNode->getLastByte(); 
    uint8_t* p_dataBuffer         = p_i2cNode->getSlaveDataPacket();

    if(p_dataBuffer != nullptr)
    {     
        if((FIRST_BYTE == 0)
        && (LAST_BYTE == 0))
        {
            Serial.println("error: No Slave Data Set");
        }
        for(uint8_t BYTE = FIRST_BYTE; BYTE < LAST_BYTE; BYTE++)
        {
            //Serial.println(p_dataBuffer[BYTE]);
            Wire.write(p_dataBuffer[BYTE]); 
        } 
    }
    else
    {
        Serial.println("error: Slave Data Buffer is nullptr");
    }
    //Sobald alle Daten versendet, Zeiger löschen als bestätigung
    if(LAST_BYTE < MAX_FRAME_SIZE)
    {
        p_i2cNode->requestedDataSend();
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
        Serial.println("Setup I2C node as Master");
        Wire.begin();
    }
    else
    {
        Serial.println("Setup I2C node as Slave");
        //Slave Konfig
        Wire.begin(NODE_ADDRESS);
        Wire.onReceive(receiveCallback);
        Wire.onRequest(requestCallback); 
        this->request.p_dataBuffer  = p_slaveDataBuffer; 
        this->request.firstByte     = 0;
        this->request.lastByte      = 0;
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
    Serial.print("commandToProcess: ");
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
            if(this->command.f_newFrameReceived)
            {
                Serial.println("error: Kommando empfangen, kann nicht gespeichert werden, da Buffer voll");
            }
            else
            {
                memcpy(&this->command.commandToProcess, p_newFrame, sizeof(u_I2C_BPLC_NODE_FRAME_t));
                this->command.f_newFrameReceived = true;
            }            
            break;   

        case I2C_BPLC_KEY__REQUEST_SLAVE_DATA:
            this->request.firstByte = p_newFrame->extract.payload[0];
            this->request.lastByte  = p_newFrame->extract.payload[1];
            break;

            default:
        case I2C_BPLC_KEY__NO_KEY:              
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
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = this->command.commandToProcess;
    memset(&this->command.commandToProcess, 0, sizeof(this->command.commandToProcess));
    this->command.f_newFrameReceived = false;

    return NEW_FRAME;
}
uint8_t BPLC_I2C_NODE::requestFromNode(uint8_t* p_payloadBuffer, const uint8_t PAYLOAD_SIZE)
{ 
    uint8_t inByte = 0;    
    //alle Bytes erhalten 
    while (inByte < PAYLOAD_SIZE) 
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
            //Berechnen wie viele Bytes in diesem Packet ankommen  
                    uint8_t requestedBytes          = 0;
            const   uint8_t BYTES_LEFT_TO_RECEIVE   = (PAYLOAD_SIZE - inByte);
            if(BYTES_LEFT_TO_RECEIVE > MAX_FRAME_SIZE)
            {
                requestedBytes = MAX_FRAME_SIZE;                
            }
            else 
            {
                requestedBytes = BYTES_LEFT_TO_RECEIVE;
            }
            const uint8_t FIRST_BYTE    = inByte; 
            const uint8_t LAST_BYTE     = (inByte + requestedBytes);                
            const uint8_t PACKET_DEFINITION[2] = {FIRST_BYTE, LAST_BYTE};               

            this->sendFrame(I2C_BPLC_KEY__REQUEST_SLAVE_DATA, &PACKET_DEFINITION[0], 2);
            delay(1);   //Sicherheit, kann wahrscheinlich weg aber nur wenn es nötig ist 
            Wire.requestFrom(this->nodeAddress, requestedBytes);              
        }

        while(Wire.available())
        {      
            p_payloadBuffer[inByte] = Wire.read(); 
            //Serial.println("byte: " + String(inByte) + ": "+ String(p_payloadBuffer[inByte]));
            inByte++;
        }        
    }
    return inByte;
}