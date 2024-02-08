#include "HAL_DO11.h"

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_DO11::setup()
{
    this->setError(BPLC_ERROR__NO_ERROR);
   
    //I2C verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
        this->setError(DO11_ERROR__I2C_CONNECTION_FAILED);     
    }
    
    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {        
        PCA.setI2CAddress(this->deviceAddress);
        PCA.init();
        PCA.setPWMFrequency(200);   //Falls Servos verwendet werden, wird automatisch PWM freuenz auf 25Hz gesenkt!
        PCA.setAllChannelsPWM(0);
        this->printLog("DO11revA CARD (" + String(I2C_ADDRESS) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("DO11revA CARD (" + String(I2C_ADDRESS) + ") INIT FAILED");    
    }
}
void HAL_DO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    //Wenn Channel 1 übergeben wird, ist p_ioObject[0] gemeint 
    CHANNEL--;
    if(CHANNEL < 0 || CHANNEL > DO11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr && CHANNEL == DO11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr)
    {
        this->setError(DIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        this->channels.p_ioObject[CHANNEL] = P_IO_OBJECT;
        //Bei Servos, die PWM Frequenz senken
        if(P_IO_OBJECT->getIoType() == IO_TYPE__SERVO)
        {
            this->PCA.setPWMFrequency(25);
        }
    }
}
void HAL_DO11::tick()
{
    for(uint8_t CH = 0; CH < DO11_CHANNEL_COUNT; CH++)
    {       
        if(this->channels.p_ioObject[CH] != nullptr)
        {
            if(this->channels.p_ioObject[CH].newDataAvailable())
            {
                //PWM von 0-255 laden und umrechnen
                const uint16_t TARGET_PWM_VALUE = map(this->channels.p_ioObject[CH]->halCallback().analogIoData.value, 0, 255, 0, 4095);
            
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

                    case IO_TYPE__SERVO:
                        uint16_t TARGET_PWM_VALUE = this->channels.p_servoInstance[CH]->halCallback().value;
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

                    default:
                    case IO_TYPE__NOT_DEFINED:
                        this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE);
                        break;               
                }
            }
        }   
    }    
}
e_BPLC_ERROR_t HAL_DO11::getError()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}