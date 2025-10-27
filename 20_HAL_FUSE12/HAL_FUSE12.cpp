#include "HAL_FUSE12.h"

HAL_FUSE12::HAL_FUSE12()
{}
void HAL_FUSE12::init(const e_EC_ADDR_t ADDR)
{
    this->bplcAddress = ADDR;
    
    if(ADDR < FUSE12_ADDRESS_COUNT)
    {
        this->i2cAddress = FUSE12_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(FUSE12_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < FUSE12_CHANNEL_COUNT; CH++)
    {
        this->channels.p_inObject[CH]   = nullptr;
        this->channels.p_outObject[CH]  = nullptr;
    }   
      
    //I2C Verbindung prüfen
    if(I2C_check::begin(this->i2cAddress) == false)
    {
        this->setError(FUSE12_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {           
        this->printLog("FUSE12revA CARD (" + String(this->i2cAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {    
        this->printLog("FUSE12revA CARD (" + String(this->i2cAddress) + ") INIT FAILED", __FILENAME__, __LINE__);  
    } 
}
void HAL_FUSE12::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    if(P_IO_OBJECT->getIoType() == IO_TYPE__DIGITAL_INPUT)
    {
        if(CHANNEL < EC_CHANNEL_1 || CHANNEL > FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
        }
        else if(this->channels.p_inObject != nullptr && CHANNEL == FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {   
            this->channels.p_inObject[CHANNEL] = P_IO_OBJECT;
        }
    } 
    else if (P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
    {
        if(CHANNEL < EC_CHANNEL_1 || CHANNEL > FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
        }
        else if(this->channels.p_outObject != nullptr && CHANNEL == FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {   
            this->channels.p_outObject[CHANNEL] = P_IO_OBJECT;
        }
    } 
}
void HAL_FUSE12::tick()
{}
void HAL_FUSE12::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("COMMAND NOT AVAILABLE", __FILENAME__, __LINE__);
            break;
    }
}