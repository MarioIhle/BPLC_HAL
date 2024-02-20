#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11(const e_DIN11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_DIN11::init()
{  
    for(uint8_t CH =0; CH < DIN11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       

    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);        
    }

    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        PCF.setAddress(this->deviceAddress);   
        PCF.begin();      
        this->printLog("DIN11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("DIN11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");
    }
}
void HAL_DIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == DIN11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(DIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
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
                this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE);
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
            //PCF über I2C lesen
            this->PCF.read8();
            //Alle genutzen Channels abfragen
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
                            this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE);
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
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}