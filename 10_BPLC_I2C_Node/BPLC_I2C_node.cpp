#include "BPLC_I2C_Nodes.h"

//receiveCallback
u_I2C_BPLC_NODE_FRAME_t callback_inBuffer;
void receiveCallback(int howMany)
{
    while(Wire.available())
    {
        for(uint8_t BYTE = 0; BYTE < howMany; BYTE++)
        {    
            callback_inBuffer.data[BYTE] = Wire.read();    
            //Serial.print(callback_inBuffer.data[BYTE]);
        }
    }             
}

//requestCallback
u_I2C_BPLC_NODE_FRAME_t* p_callback_outBuffer;  //pointer auf Daten, die bei Master Request versendet werden
void requestCallback()
{    
    Serial.println("Master request");
    const uint8_t BYTES_REQUESTED = (p_callback_outBuffer->extract.payloadSize + MESSAGE_HEAD);
    Wire.write(p_callback_outBuffer->data, BYTES_REQUESTED);  //PAYLOAD_SIZE + i2cBplcKey + paylaodSize  
}

BPLC_I2C_NODE::BPLC_I2C_NODE()
{}
void BPLC_I2C_NODE::begin(const uint8_t ADDRESS, u_I2C_BPLC_NODE_FRAME_t* p_cb_request)
{
    //I2C Kommunikation aufbauen
    if(ADDRESS != 0)
    {
        Wire.begin(ADDRESS);
    }
    //I2C taktfrequenz auf 400kHz erhöhen
    Wire.setClock(400000);
   
  
    Wire.onReceive(receiveCallback);
    Wire.onRequest(requestCallback);
    p_callback_outBuffer = p_cb_request;
}
void BPLC_I2C_NODE::sendFrame(const uint8_t DESTINATION_ADDRESS, const e_I2C_BPLC_KEY_t KEY, const uint8_t* PAYLOAD, const uint8_t BYTE_COUNT)
{
    u_I2C_BPLC_NODE_FRAME_t OUT_FRAME;
    memset(&OUT_FRAME, 0, sizeof(OUT_FRAME));

    OUT_FRAME.extract.i2cBplcKey    = (uint8_t)KEY;
    OUT_FRAME.extract.payloadSize   = BYTE_COUNT;

    if(PAYLOAD != nullptr)
    {        
        memcpy(OUT_FRAME.extract.payload, PAYLOAD, BYTE_COUNT);
    }   

    Serial.println("key: " + String(OUT_FRAME.extract.i2cBplcKey));
    Serial.println("size: " + String(OUT_FRAME.extract.payloadSize));
    Serial.println("payload: ");
    for(int i = 0; i< (BYTE_COUNT); i++)
    {
        Serial.print(OUT_FRAME.extract.payload[i]);
    }
    Serial.println("");
    Serial.print("frame: ");
    for(int i = 0; i< (BYTE_COUNT + MESSAGE_HEAD); i++)
    {
        Serial.print(OUT_FRAME.data[i]);
    }
    Serial.println("");

    Wire.beginTransmission(DESTINATION_ADDRESS);
    Wire.write(OUT_FRAME.data, (MESSAGE_HEAD+BYTE_COUNT));            
    Wire.endTransmission(true);          
}
bool BPLC_I2C_NODE::newSlaveCommandReceived()
{
    return (I2C_BPLC_KEY__SLAVE_COMMAND == callback_inBuffer.extract.i2cBplcKey);
}
u_I2C_BPLC_NODE_FRAME_t BPLC_I2C_NODE::getFrame()
{
    u_I2C_BPLC_NODE_FRAME_t NEW_FRAME = callback_inBuffer;
    memset(&callback_inBuffer, 0, sizeof(callback_inBuffer));
    callback_inBuffer.extract.i2cBplcKey = I2C_BPLC_KEY__NO_KEY;

    return NEW_FRAME;
}