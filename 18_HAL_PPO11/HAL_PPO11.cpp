#include "HAL_PPO11.h"

HAL_PPO11::HAL_PPO11()
{}
void HAL_PPO11::init(const e_EC_ADDR_t ADDR)
{
    if(ADDR < PPO11_ADDRESS_COUNT)
    {
        this->deviceAddress = PPO11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(PPO11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < PPO11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
   
    //I2C verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->setError(PPO11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);     
    }
    
    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {        
        PCA.setI2CAddress(this->deviceAddress);
        PCA.init();
        PCA.setPWMFrequency(200);   
        PCA.setAllChannelsPWM(0);
        this->printLog("PPO11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);      
    }    
    else
    {
        this->printLog("PPO11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);    
    }
}
void HAL_PPO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > PPO11_CHANNEL_COUNT)
    {
        this->setError(PPO11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        if(CHANNEL == PPO11_CHANNEL_COUNT)
        {
            this->setError(PPO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {   
            this->setError(PPO11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);     
        }
        
    }
    else
    {        
        switch (P_IO_OBJECT->getIoType())
        {                    
            case IO_TYPE__OUTPUT_PULL:        
            case IO_TYPE__OUTPUT_PUSH:      
            case IO_TYPE__OUTPUT_PUSH_PULL:                         
            case IO_TYPE__OUTPUT_PUSH_PULL_INVERT: 
                this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
            break;
            default:
                this->setError(PPO11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
            break;
        }
    }
}
void HAL_PPO11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->tickHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken
    if(this->noErrorSet())
    {  
        for(uint8_t CH = 0; CH < PPO11_CHANNEL_COUNT; CH++)
        {       
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    //PWM von 0-255 laden und umrechnen
                    uint16_t TARGET_PWM_VALUE = map(this->channels.p_ioObject[CH]->halCallback().analogIoData.value, 0, 255, 0, 4095);
                
                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {                    
                        case IO_TYPE__OUTPUT_PULL:
                            PCA.setChannelPWM(this->channels.PIN[CH][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                            PCA.setChannelOff(this->channels.PIN[CH][HS_MOSFET]);                             //highside 
                        break;
                        
                        case IO_TYPE__OUTPUT_PUSH:
                            PCA.setChannelOff(this->channels.PIN[CH][LS_MOSFET]);                             //lowSide
                            PCA.setChannelPWM(this->channels.PIN[CH][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                            break;

                        case IO_TYPE__OUTPUT_PUSH_PULL:                            
                            //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                            PCA.setChannelOff(this->channels.PIN[CH][HS_MOSFET]); //Spannungsführend zuerst aus
                            PCA.setChannelOff(this->channels.PIN[CH][LS_MOSFET]);                                       
                            delay(1);       
                                                    
                            //FULL ON
                            if(TARGET_PWM_VALUE < DEAD_TIME)
                            {
                                PCA.setChannelOn(this->channels.PIN[CH][LS_MOSFET]);
                                //PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]);    
                            }
                            //FULL OFF
                            else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                            {
                                //PCA.setChannelOff(this->channels.PIN[PORT][LS_MOSFET]);
                                PCA.setChannelOn(this->channels.PIN[CH][HS_MOSFET]);    
                            }
                            //PWM
                            else
                            {                        
                                PCA.setChannelPWM(this->channels.PIN[CH][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                                PCA.setChannelPWM(this->channels.PIN[CH][HS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                            }
                            break;

                        case IO_TYPE__OUTPUT_PUSH_PULL_INVERT:                            
                            //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                            PCA.setChannelOff(this->channels.PIN[CH][HS_MOSFET]); //Spannungsführend zuerst aus
                            PCA.setChannelOff(this->channels.PIN[CH][LS_MOSFET]);                                       
                            delay(1);       
                                                    
                            //FULL ON
                            if(TARGET_PWM_VALUE < DEAD_TIME)
                            {
                                PCA.setChannelOn(this->channels.PIN[CH][HS_MOSFET]);
                                //PCA.setChannelOff(this->channels.PIN[PORT][HS_MOSFET]);    
                            }
                            //FULL OFF
                            else if(TARGET_PWM_VALUE > 4096 - DEAD_TIME)
                            {
                                //PCA.setChannelOff(this->channels.PIN[PORT][LS_MOSFET]);
                                PCA.setChannelOn(this->channels.PIN[CH][LS_MOSFET]);    
                            }
                            //PWM
                            else
                            {                        
                                PCA.setChannelPWM(this->channels.PIN[CH][HS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                                PCA.setChannelPWM(this->channels.PIN[CH][LS_MOSFET],  DEAD_TIME,               TARGET_PWM_VALUE);  
                            }
                            break;
                        
                        default:
                        case IO_TYPE__NOT_DEFINED:
                            this->setError(PPO11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                            break;               
                    }
                }
            }   
        }    
    }
}
void HAL_PPO11::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("WRONG COMMAND FOR THIS EXTENSION CARD", __FILENAME__, __LINE__);
            break;
    }
}