#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_AIN11::init()
{    
    for(uint8_t CH =0; CH < AIN11_CHANNEL_COUNT; CH++)
    {
        this->channels.p_ioObject[CH] = nullptr;
    }       
    
    //I2C Verbindung prüfen
    if(!I2C_check::begin(this->deviceAddress))
    {
       this->setError(AIN11_ERROR__I2C_CONNECTION_FAILED);        
    }
    //Applikationsparameter initialisieren         
    if(this->getError() == BPLC_ERROR__NO_ERROR)
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
        this->printLog("AIN11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        this->printLog("AIN11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");    
    }
}
void HAL_AIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    const uint8_t OBJECT_INSTANCE = CHANNEL - 1;

    if(CHANNEL < 1 || CHANNEL > AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr && CHANNEL == AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[OBJECT_INSTANCE] != nullptr)
    {
        this->setError(AIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        this->channels.p_ioObject[OBJECT_INSTANCE] = P_IO_OBJECT;
    }
}
void HAL_AIN11::tick()
{   
    if(this->getError() == BPLC_ERROR__NO_ERROR)
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
                            this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE);
                        break;               
                    }
                }       
            }
        }  
    }
}
e_BPLC_ERROR_t HAL_AIN11::getErrorCode()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(AIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}