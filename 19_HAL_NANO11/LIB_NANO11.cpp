#include "HAL_NANO11.h"

LIB_NANO11::LIB_NANO11()
{
    initInternals();
}
void LIB_NANO11::initInternals()
{
    this->error         = 0;
    this->intOutput.begin(IO_TYPE__OUTPUT_PUSH, OUTPUT_SETTING__NORMALY_CLOSED, 1);

    for(uint8_t channel = 0; channel < NANO11_CHANNEL_COUNT; channel++)
    {
        this->channels[channel].pin                 = 0;
        this->channels[channel].ioType              = IO_TYPE__NOT_DEFINED;
        this->channels[channel].f_newDataAvailable  = false;
        this->channels[channel].p_externalData      = nullptr;
        memset(&this->channels[channel].data, 0, sizeof(u_HAL_DATA_t));
    } 
} 
void LIB_NANO11::begin(const e_EC_ADDR_t ADDR)
{    
    pinMode(this->pins.INT, OUTPUT); 
    pinMode(this->pins.OEN, INPUT); 
    digitalWrite(this->pins.INT, HIGH);

    if(ADDR < NANO11_ADDRESS_COUNT)
    {
        this->bplcNode.begin(NANO11_I2C_ADDRESSES[ADDR]);             
    }
    else
    {
        this->error = true;
    }   
}
void LIB_NANO11::mapPinToChannel(const uint8_t PIN, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE)
{
    const uint8_t CHANNEL_INSTANCE = ((uint8_t)CHANNEL - 1);
    
    if((CHANNEL < EC_CHANNEL_1)
    || (CHANNEL > NANO11_CHANNEL_COUNT))
    {
        //Channel out of range
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].ioType != IO_TYPE__NOT_DEFINED)
    {
        //Channel schon belegt
        this->error = true;
    }
    else
    {
        this->channels[CHANNEL_INSTANCE].ioType = IO_TYPE;

        switch (IO_TYPE)
        {
            case IO_TYPE__ANALOG_INPUT:  
                this->channels[CHANNEL_INSTANCE].pin = PIN;
                pinMode(PIN, INPUT);
                break;

            case IO_TYPE__DIGITAL_INPUT:
                this->channels[CHANNEL_INSTANCE].pin = PIN;
                pinMode(PIN, INPUT);
                break;

            case IO_TYPE__OUTPUT_PUSH:                    
            case IO_TYPE__OUTPUT_PULL:
            case IO_TYPE__OUTPUT_PUSH_PULL:
            case IO_TYPE__OUTPUT_PUSH_PULL_INVERT:      
                this->channels[CHANNEL_INSTANCE].pin    = PIN;                
                pinMode(PIN, OUTPUT);
                //Serial.println("Channel: " + String(CHANNEL) +  ", Output mapped to pin: " + String(PIN));
                break;

            default:
                Serial.println("IO_TYPE nicht kompatibel!");
                break;
        }
    }
}
void LIB_NANO11::mapHalDataToChannel(u_HAL_DATA_t* P_DATA, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE)
{
    const uint8_t CHANNEL_INSTANCE = ((uint8_t)CHANNEL - 1);
    
    if((CHANNEL < EC_CHANNEL_1)
    || (CHANNEL > NANO11_CHANNEL_COUNT))
    {
        //Channel out of range
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].ioType != IO_TYPE__NOT_DEFINED)
    {
        //Channel schon belegt
        this->error = true;
    }
    else
    {
        this->channels[CHANNEL_INSTANCE].ioType         = IO_TYPE;
        this->channels[CHANNEL_INSTANCE].p_externalData = P_DATA; 
    }
}
void LIB_NANO11::tick()
{
    //Commando empfangen
    const bool NEW_COMMAND_FROM_MASTER = this->bplcNode.newCommandAvailable();

    if(NEW_COMMAND_FROM_MASTER)
    {
        s_NANO11_COMMAND_t command;
        memset(&command, 0, sizeof(s_NANO11_COMMAND_t));
        this->bplcNode.getCommand(command.data);
      
        switch (command.extract.key)
        {   
            //Output setzen
            case NANO11_COMMAND_KEY__WRITE_CHANNEL:             
                //Daten an loop übergeben 
                if(this->channels[command.extract.channel].p_externalData != nullptr)
                {                
                    memcpy(this->channels[command.extract.channel].p_externalData->data, command.extract.payload, sizeof(u_HAL_DATA_t)); 
                }            
                //Daten sichern
                memcpy(this->channels[command.extract.channel].data.data, command.extract.payload, sizeof(u_HAL_DATA_t));                    
                this->channels[command.extract.channel].f_newDataAvailable = true;                                                          
                break;
            
            case NANO11_COMMAND_KEY__SET_CHANNEL_TO_READ:  
                //Daten für Masterabfrage bereitstellen
                this->bplcNode.setSlaveData(this->channels[command.extract.channel].data.data, sizeof(u_HAL_DATA_t));
                break;
            default:
                break;
        }
    }

    //Channels aktualisieren
    bool newDataAvailable = false;   

    for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
    {                              
        //Externe Daten verarbeiten aus loop           
        if(this->channels[CH].p_externalData != nullptr)
        {
            newDataAvailable = memcmp(this->channels[CH].p_externalData->data, this->channels[CH].data.data, sizeof(u_HAL_DATA_t));
            memcpy(this->channels[CH].data.data, this->channels[CH].p_externalData->data, sizeof(u_HAL_DATA_t)); 
        }
        //pins direkt Lesen/schreiben
        else
        {
            switch (this->channels[CH].ioType)
            {       
                case IO_TYPE__ANALOG_INPUT:
                    //Input direkt von Pin lesen
                    if(this->channels[CH].pin != 0)
                    {
                        this->channels[CH].data.analogIoData.value = analogRead(this->channels[CH].pin);
                    }                                      
                    break;

                case IO_TYPE__DIGITAL_INPUT:
                    //Input direkt von Pin lesen
                    if(this->channels[CH].pin != 0)
                    {
                        const bool NEW_STATE = digitalRead(this->channels[CH].pin);
                        if(this->channels[CH].data.digitalIoData.state != NEW_STATE)
                        {
                            newDataAvailable = true;
                        }
                        this->channels[CH].data.digitalIoData.state = NEW_STATE;                        
                    }                   
                    break;

                case IO_TYPE__OUTPUT_PUSH:   
                    if(this->channels[CH].pin != 0)
                    {        
                        const bool OEN_DISABLED =   (!this->getOEN()) ;   
                        if(OEN_DISABLED)
                        {
                            digitalWrite(this->channels[CH].pin, LOW);  
                        }
                        else if(this->channels[CH].f_newDataAvailable)
                        {            
                            if(this->channels[CH].data.analogIoData.value == 0)                           
                            {
                                digitalWrite(this->channels[CH].pin, LOW);                            
                            }
                            else if(this->channels[CH].data.analogIoData.value == 255)
                            {
                                digitalWrite(this->channels[CH].pin, HIGH);
                            }          
                            else
                            {
                                analogWrite(this->channels[CH].pin, this->channels[CH].data.analogIoData.value);
                            }              
                        }     
                    }          
                    break;

                default:
                case IO_TYPE__PT1000:
                case IO_TYPE__PT100:
                case IO_TYPE__DIGITAL_COUNTER:
                case IO_TYPE__POSITION_ENCODER:         
                case IO_TYPE__PTC:
                case IO_TYPE__HMI_ENCODER:
                case IO_TYPE__RPM_SENS:                    
                    break; 
            }
                
        }       
        this->channels[CH].f_newDataAvailable = false;     
    }

    //Daten für Masterabfrage aufbereiten
    if(newDataAvailable)
    {   
        //Master über Int neue Daten melden
        this->intOutput.blinkOnce(1, 1);
        Serial.println("INT");
    }      
    
    //Int schreiben       
    if(this->intOutput.newDataAvailable())
    {         
        const bool INT_STATE = (1 == this->intOutput.getHalData().analogIoData.value);  
        digitalWrite(this->pins.INT, INT_STATE);      
    }
}