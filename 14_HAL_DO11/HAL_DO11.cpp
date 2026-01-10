#include "HAL_DO11.h"

HAL_DO11::HAL_DO11()
{
    this->bplcAddress           = EC_ADDR_NOT_DEFINED;
    this->i2cAddress            = 0;
    this->debugOutputEnabled    = false;
}
bool HAL_DO11::init(const e_EC_ADDR_t ADDR)
{
    this->bplcAddress = ADDR;

    if(ADDR < DO11_ADDRESS_COUNT)
    {
        this->i2cAddress = DO11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(DO11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    for(uint8_t CH =0; CH < DO11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
   
    //I2C verbindung prüfen
    if(!I2C_check::begin(this->i2cAddress))
    {
        this->setError(DO11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);     
    }
    
    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {        
        PCA.setI2CAddress(this->i2cAddress);
        PCA.init();
        PCA.setPWMFrequency(200);   //Falls Servos verwendet werden, wird automatisch PWM freuenz auf 25Hz gesenkt!
        PCA.setAllChannelsPWM(0);
        this->printLog("DO11revA CARD (" + String(this->bplcAddress + 1 )  + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);      
    }    
    else
    {
        this->printLog("DO11revA CARD (" + String(this->bplcAddress + 1 )  + ") INIT FAILED", __FILENAME__, __LINE__);    
    }

    return this->noErrorSet();
}
bool HAL_DO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    bool error = true;

    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > DO11_CHANNEL_COUNT)
    {
        this->setError(DO11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == DO11_CHANNEL_COUNT)
    {
        this->setError(DO11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(DO11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);       
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
                error = false;
            break;

            case IO_TYPE__SERVO:
                this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
                this->PCA.setPWMFrequency(25);
                error = false;
            break;

            default:
                this->setError(DO11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
            break;
        }
    }
    return error;
}
void HAL_DO11::tick(const bool READ_INPUTS)
{
    //I2C Verbindung zyklisch prüfen
    if(!this->tickHeartbeat())
    {
        this->setError(DO11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken    
    if(this->noErrorSet()) 
    {  
        for(uint8_t CH = 0; CH < DO11_CHANNEL_COUNT; CH++)
        {       
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    //PWM von 0-255 laden und umrechnen
                    const uint16_t APPLICATION_VALUE    = (uint16_t)this->channels.p_ioObject[CH]->getHalData().analogIoData.value;
                    const uint16_t TARGET_PWM_VALUE     = map(APPLICATION_VALUE, 0, 255, 0, 4095);                                     
                    
                    if(this->debugOutputEnabled)
                    {
                        this->printExtensionCardDebugOutput("DO11", String(this->bplcAddress), String(CH), String(APPLICATION_VALUE));
                    }

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
                            //Um überschneidung bei umschalten der PWM zu vermeiden, sonst FETS = rauch :C
                            PCA.setChannelOff(this->channels.PIN[CH][HS_MOSFET]); //Spannungsführend zuerst aus
                            PCA.setChannelOff(this->channels.PIN[CH][LS_MOSFET]);                                       
                            delay(1);       
                                                    
                            //FULL ON
                            if(APPLICATION_VALUE < DEAD_TIME)
                            {
                                PCA.setChannelOn(this->channels.PIN[CH][LS_MOSFET]);
                                //PCA.setChannelOff(this->PIN[PORT][HS_MOSFET]);    
                            }
                            //FULL OFF
                            else if(APPLICATION_VALUE > 4096 - DEAD_TIME)
                            {
                                //PCA.setChannelOff(this->channels.PIN[PORT][LS_MOSFET]);
                                PCA.setChannelOn(this->channels.PIN[CH][HS_MOSFET]);    
                            }
                            //PWM
                            else
                            {                        
                                PCA.setChannelPWM(this->channels.PIN[CH][LS_MOSFET],  TARGET_PWM_VALUE + DEAD_TIME,       4095);
                                PCA.setChannelPWM(this->channels.PIN[CH][HS_MOSFET],  DEAD_TIME,               APPLICATION_VALUE);  
                            }
                            break;

                        default:
                        case IO_TYPE__NOT_DEFINED:
                            this->setError(DO11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                            break;               
                    }
                }
            }   
        }    
    }
}
void HAL_DO11::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("COMMAND NOT AVAILABLE", __FILENAME__, __LINE__);
        break;

        case EC_COMMAND__ENABLE_DEBUG_OUTPUT: 
            this->debugOutputEnabled = true;
            this->printLog("DEBUG OUTPUT ENABLED", __FILENAME__, __LINE__);
        break;
        
        case EC_COMMAND__DISABLE_ERROR_DETECTION:
            this->printLog("ERROR DETECTION DISABLED", __FILENAME__, __LINE__);
            this->disableErrordetection(__FILENAME__, __LINE__);
        break;
    }
}