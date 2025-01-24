#include "HAL_DIN11.h"

HAL_DIN11::HAL_DIN11()
{}
void HAL_DIN11::init(const e_EC_ADDR_t ADDR)
{ 
    if(ADDR < DIN11_ADDRESS_COUNT)
    {
        this->deviceAddress = DIN11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(DIN11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < DIN11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       

    //I2C verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);     
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
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
void HAL_DIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > DIN11_CHANNEL_COUNT)
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
    //I2C Verbindung zyklisch prüfen
    if(!this->tickHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken
    if(this->noErrorSet())
    {        
        //Alle pinzustände von PCF lesen
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
                        tempBuffer.digitalIoData.state = !PCF.read(this->channels.PIN[CH]);         //pinstates bitweise aus Datenpaket filtern
                        this->channels.p_ioObject[CH]->halCallback(&tempBuffer); 
                    break;

                    case IO_TYPE__ROTARY_ENCODER:
                    case IO_TYPE__POSITION_ENCODER:
                        tempBuffer.encoderData.stateA  = !PCF.read(this->channels.PIN[CH]);          //pinstates bitweise aus Datenpaket filtern
                        tempBuffer.encoderData.stateB  = !PCF.read(this->channels.PIN[CH + 1]);      //pinstates bitweise aus Datenpaket filtern
                        tempBuffer.encoderData.stateZ  = !PCF.read(this->channels.PIN[CH + 2]);      //pinstates bitweise aus Datenpaket filtern
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