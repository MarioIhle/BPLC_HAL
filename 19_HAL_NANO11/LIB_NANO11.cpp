#include "HAL_NANO11.h"

LIB_NANO11::LIB_NANO11()
{
    initInternals();
}
void LIB_NANO11::initInternals()
{
    this->deviceAddress = 0;
    this->error         = 0;
    for(uint8_t channel = 0; channel < NANO11_CHANNEL_COUNT; channel++)
    {
        this->channels[channel].pin        = 0;
        this->channels[channel].p_ioObject = nullptr;
    } 
} 
void LIB_NANO11::begin(const e_EC_ADDR_t ADDR)
{    
    pinMode(this->pins.INT, OUTPUT); 
    pinMode(this->pins.OEN, INPUT); 
    digitalWrite(this->pins.INT, HIGH);
    this->to_INTout.setInterval(10);

    this->error = false;
    if(ADDR < NANO11_ADDRESS_COUNT)
    {
        this->deviceAddress = NANO11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->error = true;
    } 

    for(uint8_t CH =0; CH < NANO11_CHANNEL_COUNT; CH++)
    {
        this->channels[CH].p_ioObject = nullptr;
    }   
        
    //Applikationsparameter initialisieren
    if(!this->error)
    {   
        this->bplcNode.begin(this->deviceAddress);   
    }    
}
void LIB_NANO11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t CHANNEL_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > NANO11_CHANNEL_COUNT)
    {
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].p_ioObject != nullptr && CHANNEL == NANO11_CHANNEL_COUNT)
    {
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].p_ioObject != nullptr)
    {
        this->error = true;     
    }
    else
    {
        this->channels[CHANNEL_INSTANCE].p_ioObject= P_IO_OBJECT;   
    }
}
void LIB_NANO11::mapPinToChannel(const uint8_t PIN, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE)
{
    const uint8_t CHANNEL_INSTANCE = ((uint8_t)CHANNEL - 1);
    
    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > NANO11_CHANNEL_COUNT)
    {
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].p_ioObject != nullptr && CHANNEL == NANO11_CHANNEL_COUNT)
    {
        this->error = true;
    }
    else if(this->channels[CHANNEL_INSTANCE].p_ioObject != nullptr)
    {
        this->error = true;     
    }
    else
    {
        switch (IO_TYPE)
        {
            case IO_TYPE__ANALOG_INPUT:  
                this->channels[CHANNEL].p_ioObject  = new analogInput();
                this->channels[CHANNEL].pin         = PIN;
                pinMode(PIN, INPUT);
                break;

            case IO_TYPE__DIGITAL_INPUT:
                this->channels[CHANNEL].p_ioObject  = new digitalInput();
                this->channels[CHANNEL].pin         = PIN;
                pinMode(PIN, INPUT);
                break;

            case IO_TYPE__OUTPUT_PUSH:                    
            case IO_TYPE__OUTPUT_PULL:
            case IO_TYPE__OUTPUT_PUSH_PULL:
            case IO_TYPE__OUTPUT_PUSH_PULL_INVERT:      
                this->channels[CHANNEL].p_ioObject  = new output(IO_TYPE);
                this->channels[CHANNEL].pin         = PIN;
                pinMode(PIN, OUTPUT);
                break;

            default:
                Serial.println("IO_TYPE nicht kompatibel!");
                break;
        }
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
        const uint8_t MESSAGE_SIZE = this->bplcNode.getCommand(command.data);
        Serial.println("BYTES RECEIVED: "+ String(MESSAGE_SIZE));

        for(int i = 0; i< MESSAGE_SIZE; i++)
        {
            Serial.print(command.data[i]);
        }
    

        switch (command.extract.key)
        {   
            //Output setzen
            case NANO11_COMMAND_KEY__WRITE:
                {                   
                    u_HAL_DATA_t halData;
                    memcpy(halData.data, command.extract.payload, 12);
                    this->channels[command.extract.channel].p_ioObject->setHalData(&halData);        
                }
                break;
            
            default:
                break;
        }
    }
/*
    //Channels aktualisieren
    bool newDataAvailable = false;
    u_HAL_DATA_t dataBuffer[NANO11_CHANNEL_COUNT];
    memset(&dataBuffer[0], 0, sizeof(dataBuffer));

    for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
    {           
        if(this->channels[CH].p_ioObject != nullptr)
        {    
            switch (this->channels[CH].p_ioObject->getIoType())
            {
                case IO_TYPE__ANALOG_INPUT:
                    //Input direkt von Pin lesen
                    if(this->channels[CH].pin != 0)
                    {
                        dataBuffer[CH].analogIoData.value = analogRead(this->channels[CH].pin);
                    }
                    //Input über objekt aus der App lesen
                    else
                    {                  
                        if(this->channels[CH].p_ioObject->newDataAvailable())
                        {
                            dataBuffer[CH]      = this->channels[CH].p_ioObject->halCallback();
                            newDataAvailable    = true;    
                        }                                
                    }                    
                    break;

                case IO_TYPE__DIGITAL_INPUT:
                    //Input direkt von Pin lesen
                    if(this->channels[CH].pin != 0)
                    {
                        dataBuffer[CH].digitalIoData.state  = digitalRead(this->channels[CH].pin);
                    }
                    //Input über objekt aus der App lesen
                    else
                    {
                        if(this->channels[CH].p_ioObject->newDataAvailable())
                        {
                            dataBuffer[CH]      = this->channels[CH].p_ioObject->halCallback();
                            newDataAvailable    = true;    
                        }                           
                    } 
                    break;

                case IO_TYPE__DIGITAL_COUNTER:
                case IO_TYPE__POSITION_ENCODER:
                case IO_TYPE__PT1000:
                case IO_TYPE__PT100:
                case IO_TYPE__PTC:
                case IO_TYPE__ROTARY_ENCODER:
                case IO_TYPE__RPM_SENS:       
                    if(this->channels[CH].p_ioObject->newDataAvailable())
                    {
                        dataBuffer[CH]      = this->channels[CH].p_ioObject->halCallback();
                        newDataAvailable    = true;                                
                    }
                    break;

                case IO_TYPE__OUTPUT_PUSH:    
                    if(this->channels[CH].pin != 0)
                    {                 
                        if(this->channels[CH].p_ioObject->halCallback().analogIoData.value == 255)
                        {
                            digitalWrite(this->channels[CH].pin, HIGH);
                            //Serial.println("Write Output: high");
                        }
                        else if (this->channels[CH].p_ioObject->halCallback().analogIoData.value == 0)
                        {
                            digitalWrite(this->channels[CH].pin, LOW);
                           // Serial.println("Write Output: low");
                        }          
                        else
                        {
                            analogWrite(this->channels[CH].pin, this->channels[CH].p_ioObject->halCallback().analogIoData.value);
                        }              
                    }               
                    break;

                default:
                    break; 
            }       
        }
    }

    //INT impuls an Master
    if(newDataAvailable)
    {
        this->bplcNode.setSlaveData(&dataBuffer[0].data[0], sizeof(dataBuffer));
        digitalWrite(this->pins.INT, LOW);
        this->to_INTout.reset();
    } 
    else if(this->to_INTout.check())
    {
        digitalWrite(this->pins.INT, HIGH);
    }        
    else
    {
        //Warten auf timeout oder neue Daten zum versenden
    }
        */
}