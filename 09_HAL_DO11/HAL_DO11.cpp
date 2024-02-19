#include "HAL_DO11.h"

HAL_DO11::HAL_DO11(const e_DO11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_DO11::init()
{
    for(uint8_t CH =0; CH < DO11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
   
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
        this->printLog("DO11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("DO11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");    
    }
}
void HAL_DO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > DO11_CHANNEL_COUNT)
    {
        this->setError(DO11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == DO11_CHANNEL_COUNT)
    {
        this->setError(DO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(DO11_ERROR__CHANNEL_ALREADY_IN_USE);       
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

            case IO_TYPE__SERVO:
                this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
                this->PCA.setPWMFrequency(25);
            break;

            default:
                this->setError(DO11_ERROR__IO_OBJECT_NOT_SUITABLE);
            break;
        }
    }
}
void HAL_DO11::tick()
{
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {  
        for(uint8_t CH = 0; CH < DO11_CHANNEL_COUNT; CH++)
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

                        case IO_TYPE__SERVO:
                            //Direkt 16bit wert von Servo Klasse berechent
                            TARGET_PWM_VALUE = this->channels.p_ioObject[CH]->halCallback().analogIoData.value;
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
                            this->setError(DO11_ERROR__IO_OBJECT_NOT_SUITABLE);
                        break;               
                    }
                }
            }   
        }    
    }
}
e_BPLC_ERROR_t HAL_DO11::getErrorCode()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(DO11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}