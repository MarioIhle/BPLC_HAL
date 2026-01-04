#include "HAL_NANO11.h"

LIB_NANO11::LIB_NANO11()
{
    initInternals();
}
void LIB_NANO11::initInternals()
{
    this->deviceAddress = 0;
    this->error         = 0;
    this->intOutput.begin(IO_TYPE__OUTPUT_PUSH, OUTPUT_SETTING__NORMALY_CLOSED, 1);

    for(uint8_t channel = 0; channel < NANO11_CHANNEL_COUNT; channel++)
    {
        this->channels[channel].pin                 = 0;
        this->channels[channel].ioType              = IO_TYPE__NOT_DEFINED;
        this->channels[channel].f_newDataAvailable  = false;
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
        this->deviceAddress = NANO11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->error = true;
    } 
        
    //Applikationsparameter initialisieren
    if(!this->error)
    {   
        this->bplcNode.begin(this->deviceAddress);   
    }    
}
void LIB_NANO11::mapPinToChannel(const uint8_t PIN, const e_EC_CHANNEL_t CHANNEL, const e_IO_TYPE_t IO_TYPE)
{
    const uint8_t CHANNEL_INSTANCE = ((uint8_t)CHANNEL - 1);
    
    if((CHANNEL < EC_CHANNEL_1)
    || (CHANNEL > NANO11_CHANNEL_COUNT))
    {
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
                this->channels[CHANNEL_INSTANCE].pin = PIN;
                pinMode(PIN, OUTPUT);
                Serial.println("Channel: " + String(CHANNEL) +  ", Output mapped to pin: " + String(PIN));
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
      
        //Debug output
        Serial.println("BYTES RECEIVED: "+ String(MESSAGE_SIZE));        
        for(int i = 0; i< MESSAGE_SIZE; i++)
        {
            Serial.print(command.data[i]);
        }
        Serial.println("");
        Serial.println("Key: "+ String(command.extract.key));
        Serial.println("Channel: "+ String(command.extract.channel));

        switch (command.extract.key)
        {   
            //Output setzen
            case NANO11_COMMAND_KEY__WRITE:
                {                   
                    u_HAL_DATA_t halData;
                    memcpy(halData.data, command.extract.payload, 12);                          
                    Serial.println("extracted: "+ String(halData.analogIoData.value));                   
                    this->channels[command.extract.channel].data = halData;      
                    this->channels[command.extract.channel].f_newDataAvailable = true;                                                          
                }
                break;
            
            default:
                break;
        }
    }

    //Channels aktualisieren
    bool newDataAvailable = false;   

    for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
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
                    newDataAvailable     = (this->channels[CH].data.digitalIoData.state != NEW_STATE);
                    this->channels[CH].data.digitalIoData.state = NEW_STATE;                        
                }                   
                break;

            case IO_TYPE__DIGITAL_COUNTER:
            case IO_TYPE__POSITION_ENCODER:
            case IO_TYPE__PT1000:
            case IO_TYPE__PT100:
            case IO_TYPE__PTC:
            case IO_TYPE__ROTARY_ENCODER:
            case IO_TYPE__RPM_SENS:      
                break;
                

            case IO_TYPE__OUTPUT_PUSH:    
                if(this->channels[CH].f_newDataAvailable)
                {
                    if(this->channels[CH].pin != 0)
                    {                             
                        if(this->channels[CH].data.analogIoData.value == 255)
                        {
                            digitalWrite(this->channels[CH].pin, HIGH);
                            Serial.println("Write Output: high");
                        }
                        else if (this->channels[CH].data.analogIoData.value == 0)
                        {
                            digitalWrite(this->channels[CH].pin, LOW);
                            Serial.println("Write Output: low");
                        }          
                        else
                        {
                            analogWrite(this->channels[CH].pin, this->channels[CH].data.analogIoData.value);
                        }              
                    }     
                }          
                break;

            default:
                break; 
            
        }       
        this->channels[CH].f_newDataAvailable = false;     
    }

    //Daten für Masterabfrage aufbereiten
    if(newDataAvailable)
    {   
        //Daten für abfrage bereit stellen
        u_HAL_DATA_t dataBuffer[NANO11_CHANNEL_COUNT];
        memset(&dataBuffer, 0, sizeof(dataBuffer));
      
        for(uint8_t CH = 0; CH < NANO11_CHANNEL_COUNT; CH++)
        {    
            dataBuffer[CH] = this->channels[CH].data;
        }       
        this->bplcNode.setSlaveData(&dataBuffer[0].data[0], sizeof(dataBuffer));

        //Master über Int neue Daten melden
        this->intOutput.blinkOnce(1, 10);
    }      
    
    //Int schreiben       
    if(this->intOutput.newDataAvailable())
    {         
        const bool INT_STATE = this->intOutput.getHalData().digitalIoData.state;  
        digitalWrite(this->pins.INT, INT_STATE);               
    }
}