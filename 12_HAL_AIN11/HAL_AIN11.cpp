#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11()
{}
void HAL_AIN11::init(const e_EC_ADDR_t ADDR)
{    
    if(ADDR < AIN11_ADDRESS_COUNT)
    {
        this->deviceAddress = AIN11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(AIN11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }   

    for(uint8_t CH =0; CH < AIN11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
       this->setError(AIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }
    //Applikationsparameter initialisieren         
    if(this->noErrorSet())
    {   
        // The ADC input range (or gain) can be changed via the following
        // functions, but be careful never to exceed VDD +0.3V max, or to
        // exceed the upper and lower limits if you adjust the input range!
        // Setting these values incorrectly may destroy your ADC!
        //                                                                       ADS1015  ADS1115
        //                                                                       -------  -------
        this->adcGain = GAIN_TWOTHIRDS;         // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
        //this->adcGain = GAIN_ONE;             // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
        //this->adcGain = GAIN_TWO;             // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
        //this->adcGain = GAIN_FOUR;            // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
        //this->adcGain = GAIN_EIGHT;           // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
        //this->adcGain = GAIN_SIXTEEN;         // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV
        
        this->ADC.setGain(this->adcGain);
        this->ADC.begin(this->deviceAddress);
        this->printLog("AIN11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);      
    }    
    else
    {
        this->printLog("AIN11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);    
    }
}
void HAL_AIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = (uint8_t)CHANNEL - 1;

    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(AIN11_ERROR__CHANNEL_ALREADY_IN_USE, __FILENAME__, __LINE__);       
    }
    else
    {
        this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
    }
}
void HAL_AIN11::tick()
{   
    //I2C Verbindung zyklisch prüfen
    if(!this->tickHeartbeat())
    {
        this->setError(AIN11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);
    }
    //Hal ticken
    if(this->noErrorSet())
    {          
        for(uint8_t CH = 0; CH < AIN11_CHANNEL_COUNT; CH++)
        {            
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                if(this->channels.p_ioObject[CH]->newDataAvailable())
                {
                    u_HAL_DATA_t tempBuffer;
                    int16_t      readSingleEnded;

                    switch (this->channels.p_ioObject[CH]->getIoType())
                    {                    
                        case IO_TYPE__ANALOG_INPUT:    
                            readSingleEnded = this->ADC.readADC_SingleEnded(this->channels.PIN[CH]);
                            
                            if(readSingleEnded >= 0)
                            {
                                tempBuffer.analogIoData.value = readSingleEnded;
                                this->channels.p_ioObject[CH]->halCallback(&tempBuffer);                        
                            }     
                            else
                            {
                                tempBuffer.analogIoData.value = 0;
                                this->channels.p_ioObject[CH]->halCallback(&tempBuffer);
                            }         
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
}
void HAL_AIN11::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("WRONG COMMAND FOR THIS EXTENSION CARD", __FILENAME__, __LINE__);
            break;
    }
}