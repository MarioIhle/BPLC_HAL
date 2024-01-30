#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11()
{
    for(uint8_t CH; CH < AIN11_CHANNEL__COUNT; CH++)
    {  
        this->channels.state[CH] = AIN_CHANNEL__NOT_USED;
    }   
}

void HAL_AIN11::begin(const e_AIN11_ADDRESS_t I2C_ADDRESS, const uint16_t READ_INTERVAL)
{    
    this->deviceAddress = I2C_ADDRESS;
    this->errorCode     = BPLC_ERROR__NO_ERROR;
    this->to_read.setInterval(READ_INTERVAL);
    
    
    //Instanzen Prüfen
        for(uint8_t CHANNEL = 0; CHANNEL < AIN11_CHANNEL__COUNT; CHANNEL++)
        {            
            if(this->channels.state[CHANNEL] == AIN_CHANNEL__ANALOG)
            {
                //individuelle Anpassung?
                break;
            }
            else if(this->channels.state[CHANNEL] == AIN_CHANNEL__NOT_USED && CHANNEL == (AIN11_CHANNEL__COUNT - 1))
            {//letzter Port und immernoch keiner in nutzung
                this->errorCode = AIN11_ERROR__NO_CHANNEL_IN_USE;
            }
        }

    //I2C Verbindung prüfen
    if(!this->selfCheck.begin(this->deviceAddress))
    {
        this->errorCode = AIN11_ERROR__I2C_CONNECTION_FAILED;        
    }
    //Applikationsparameter initialisieren         
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
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

e_BPLC_ERROR_t HAL_AIN11::mapObjectToChannel(AnalogInput* P_OBJECT, const e_AIN11_CHANNEL_t CHANNEL)
{
    if(this->channels.state[CHANNEL] == AIN_CHANNEL__NOT_USED)
    {
        this->channels.p_analogInputInstance[CHANNEL] = P_OBJECT;
        this->channels.state[CHANNEL]    = AIN_CHANNEL__ANALOG;
        P_OBJECT->setADCGain(this->adcGain);    //ADC gain für Spannungsberechnung übergeben
    }
    else 
    {
        this->errorCode = AIN11_ERROR__CHANNEL_ALREADY_IN_USE;
    }
    return this->errorCode;
}

void HAL_AIN11::tick()
{   
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = AIN11_ERROR__I2C_CONNECTION_FAILED;
    }
   
  
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {
        if(this->to_read.check())
        {
            for(uint8_t CHANNEL = 0; CHANNEL < AIN11_CHANNEL__COUNT; CHANNEL++)
            {            
                if(this->channels.state[CHANNEL] == AIN_CHANNEL__ANALOG)
                {
                    const int16_t   RAW_ADC_VALUE = this->ADC.readADC_SingleEnded(this->channels.PIN[CHANNEL]);
                    const float     VALUE_IN_VOLT = this->ADC.computeVolts(RAW_ADC_VALUE);

                    if(RAW_ADC_VALUE >= 0)
                    {
                        this->channels.p_analogInputInstance[CHANNEL]->halCallback(RAW_ADC_VALUE);                        
                    }     
                    else
                    {
                        this->channels.p_analogInputInstance[CHANNEL]->halCallback(0);
                    }               

                }                
            }
            this->to_read.reset(); 
        }
    }
}

e_BPLC_ERROR_t HAL_AIN11::getError()
{
    return this->errorCode;
}