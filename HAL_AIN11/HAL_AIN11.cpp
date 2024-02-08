#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_AIN11::setup()
{    
    this->setError(BPLC_ERROR__NO_ERROR);
    this->to_read.setInterval(1000);
    
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
        BPLC_LOG logPrint;
        logPrint.printLog("AIN11revA CARD (" + String(I2C_ADDRESS) + ") INIT SUCCESSFUL");      
    }    
    else
    {
        BPLC_LOG logPrint;
        logPrint.printLog("AIN11revA CARD (" + String(I2C_ADDRESS) + ") INIT FAILED");    
    }
}
void HAL_AIN11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    //Wenn Channel 1 übergeben wird, ist p_ioObject[0] gemeint 
    CHANNEL--;
    if(CHANNEL < 0 || CHANNEL > AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr && CHANNEL == AIN11_CHANNEL_COUNT)
    {
        this->setError(AIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else if(this->channels.p_ioObject[CHANNEL] != nullptr)
    {
        this->setError(AIN11_ERROR__CHANNEL_ALREADY_IN_USE);       
    }
    else
    {
        this->channels.p_ioObject[CHANNEL] = P_IO_OBJECT;
    }
}
void HAL_AIN11::tick()
{   
    if(this->to_read.checkAndReset())
    {
        for(uint8_t CH = 0; CH < AIN11_CHANNEL_COUNT; CH++)
        {            
            if(this->channels.p_ioObject[CH] != nullptr)
            {
                u_IO_DATA_BASE_t tempBuffer;

                switch (this->channels.p_ioObject[CH]->getIoType())
                {                    
                    case IO_TYPE__ANALOG_INPUT:    
                        tempBuffer.analogIoData.value = this->ADC.readADC_SingleEnded(this->channels.PIN[CH]);
                        if(tempBuffer.analogIoData.value >= 0)
                        {
                            this->channels.p_analogInputInstance[CH]->halCallback(tempBuffer.analogIoData.value);                        
                        }     
                        else
                        {
                            tempBuffer.analogIoData.value = 0;
                            this->channels.p_analogInputInstance[CH]->halCallback(tempBuffer.analogIoData.value);
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
e_BPLC_ERROR_t HAL_AIN11::getError()
{
    //I2C Verbindung zyklisch prüfen
    if(!this->requestHeartbeat())
    {
        this->setError(AIN11_ERROR__I2C_CONNECTION_FAILED);
    }
    return this->getError();
}