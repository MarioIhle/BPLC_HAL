#include "HAL_DO11.h"

HAL_DO11::HAL_DO11()
{
    memset(&this->channels, 0, sizeof(this->channels));
}

void HAL_DO11::begin(const e_DO11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup DO11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case DO11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case DO11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case DO11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case DO11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        this->errorCode = DO11_ERROR__I2C_CONNECTION_FAILED;     
    }

    //Applikationsparameter initialisieren
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {        
        PCA.setI2CAddress(this->deviceAddress);
        PCA.init();
        PCA.setPWMFrequency(200);   //Falls Servos verwendet werden, wird automatisch PWM freuenz auf 50Hz gesenkt!
        PCA.setAllChannelsPWM(0);
    }
}

void HAL_DO11::setPWMFrequency(const uint8_t FREQUENCY)
{
    this->PCA.setPWMFrequency(FREQUENCY);
}

e_BPLC_ERROR_t HAL_DO11::mapObjectToChannel(Output* P_OBJECT, const e_DO11_CHANNEL_t CHANNEL)
{
    if(this->channels.used[CHANNEL] == CHANNEL_STATE__NOT_IN_USE)
    {
        this->channels.p_object[CHANNEL] = P_OBJECT;
        this->channels.used[CHANNEL]     = CHANNEL_STATE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->errorCode = DO11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
    return this->errorCode;
}

e_BPLC_ERROR_t HAL_DO11::mapObjectToChannel(servoMotor* P_OBJECT, const e_DO11_CHANNEL_t CHANNEL)
{
    if(this->channels.used[CHANNEL] == CHANNEL_STATE__NOT_IN_USE)
    {
        this->channels.p_object[CHANNEL] = &P_OBJECT->CHANNEL;
        this->channels.used[CHANNEL]     = CHANNEL_STATE__MAPPED_TO_OBJECT;
        this->PCA.setPWMFreqServo();
    }
    else 
    {
        this->errorCode = DO11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
    return this->errorCode;
}

void HAL_DO11::tick()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = DO11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t CH = 0; CH < DO11_CHANNEL__COUNT; CH++)
    {            
        if(this->channels.used[CH] == CHANNEL_STATE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->channels.used[CH] == CHANNEL_STATE__NOT_IN_USE && CH == (DO11_CHANNEL__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = DO11_ERROR__NO_CHANNEL_IN_USE;
        }
    }

    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {
        for(uint8_t CH; CH < DO11_CHANNEL__COUNT; CH++)
        {       
            if(this->channels.used[CH] == CHANNEL_STATE__MAPPED_TO_OBJECT)
            {                      
                if(this->channels.p_object[CH]->isThereANewPortValue())    //Nur Wert abrufen und schreiben, falls dier sich geändert hat
                {     
                    //PWM von 0-255 laden und umrechnen
                    const uint16_t TARGET_PWM_VALUE = map(this->channels.p_object[CH]->halCallback().value, 0, 255, 0, 4095);

                    switch(this->channels.p_object[CH]->getOutputType())
                    {
                        case OUTPUTTYPE__PULL:
                            PCA.setChannelPWM(this->channels.PIN[CH][LS_MOSFET], 0, TARGET_PWM_VALUE);        //lowSide
                            PCA.setChannelOff(this->channels.PIN[CH][HS_MOSFET]);                             //highside
                        break;

                        case OUTPUTTYPE__PUSH:
                            PCA.setChannelOff(this->channels.PIN[CH][LS_MOSFET]);                             //lowSide
                            PCA.setChannelPWM(this->channels.PIN[CH][HS_MOSFET], 0, TARGET_PWM_VALUE);        //highside
                        break;

                        case OUTPUTTYPE__PUSH_PULL:                            
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
                        case OUTPUTTYPE__PUSH_PULL_INVERT:                            
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
                    }
                }  
            }        
        }   
    }
}

e_BPLC_ERROR_t HAL_DO11::getError()
{
    return this->errorCode;
}