#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11()
{}
void HAL_DIN11::init(const e_EC_ADDR_t ADDR)
{ 
    switch(ADDR)
    {
        case EC_ADDR_1:
            this->deviceAddress = I2C_ADDRESS_DIN11__ADDR_1;
            break;
        case EC_ADDR_2:
            this->deviceAddress = I2C_ADDRESS_DIN11__ADDR_2;
            break;
        case EC_ADDR_3:
            this->deviceAddress = I2C_ADDRESS_DIN11__ADDR_3;
            break;
        case EC_ADDR_4:
            this->deviceAddress = I2C_ADDRESS_DIN11__ADDR_4;
            break;
            
        default:
            this->setError(AIN11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
            break;
    }

    for(uint8_t CH =0; CH < DIN11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       

    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();      
        this->printLog("DIN11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);      
    }    
    else
    {
        this->printLog("DIN11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);
    }
}
void HAL_DIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(DIN11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);       
    }
    else
    {
        switch (P_IO_OBJECT->getIoType())
        {                    
            case IO_TYPE__DIGITAL_INPUT:
            case IO_TYPE__RPM_SENS:
            case IO_TYPE__DIGITAL_COUNTER:
                this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
                break;

            case IO_TYPE__ROTARY_ENCODER:
            case IO_TYPE__POSITION_ENCODER:
                //3 pinPorts belegen mit gleichen objekt
                this->channels.p_ioObject[OBJECT_INSTANCE]      = P_IO_OBJECT;  //A
                this->channels.p_ioObject[OBJECT_INSTANCE + 1]  = P_IO_OBJECT;  //B
                this->channels.p_ioObject[OBJECT_INSTANCE + 2]  = P_IO_OBJECT;  //Z
                break;

            default:
            case IO_TYPE__NOT_DEFINED:
                this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                break;               
        }
    }
}
void HAL_DIN11::tick()
{   
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {         
        for(uint8_t READ = 0; READ < 2; READ++)//2x lesen um Flankenauswertung zu ermöglichen
        {
            this->PCF.read8();
            
            for(uint8_t CH = 0; CH < DIN11_CHANNEL_COUNT; CH++)
            {      
                if(this->channels.p_ioObject[CH] != nullptr)
                {
                    u_HAL_DATA_t tempBuffer;                

                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {                    
                        case IO_TYPE__DIGITAL_INPUT:
                        case IO_TYPE__RPM_SENS:
                        case IO_TYPE__DIGITAL_COUNTER:
                            tempBuffer.digitalIoData.state = !PCF.read(this->channels.PIN[CH]);
                            this->channels.p_ioObject[CH]->halCallback(&tempBuffer); 
                        break;

                        case IO_TYPE__ROTARY_ENCODER:
                        case IO_TYPE__POSITION_ENCODER:
                            tempBuffer.encoderData.stateA  = !PCF.read(this->channels.PIN[CH]);
                            tempBuffer.encoderData.stateB  = !PCF.read(this->channels.PIN[CH + 1]);
                            tempBuffer.encoderData.stateZ  = !PCF.read(this->channels.PIN[CH + 2]);
                            this->channels.p_ioObject[CH]->halCallback(&tempBuffer);
                            CH +=3;//Sonst wird encoder 3x gelesen...
                        break;

                        default:
                        case IO_TYPE__NOT_DEFINED:
                            this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                        break;               
                    }
                }                                              
            }    
        }         
    }
}
e_BPLC_ERROR_t HAL_DIN11::getErrorCode()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    return this->getError();
}