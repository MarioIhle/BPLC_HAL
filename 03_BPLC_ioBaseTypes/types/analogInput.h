#ifndef analogInput_h
#define analogInput_h

#include "Arduino.h"
#include <Adafruit_ADS1X15.h>
#include "00_ioInterface.h"
#include "SpecialFunctions.h"

#define SPANNUNGSTEILER (5900/1200)

class analogInput: public IO_Interface
{
    public:
    analogInput(const float MAX_VOLTAGE = 5.00, const uint64_t SAMPLE_TIME = 1000)
    {
        this->maxVoltage 	= MAX_VOLTAGE;	
        this->ioType	 	= IO_TYPE__ANALOG_INPUT;
        this->value			= 0;   
        this->alarmValue	= 0;
        this->adcGain		= GAIN_TWOTHIRDS;   
        this->to_sampleTime.setInterval(SAMPLE_TIME);         
    }  
    //Getter für Applikation
    uint16_t            getValue            (){return this->value;}
    float               getValueInVolt      ()
    {
        // see data sheet Table 3
        float fsRange;
        switch(this->adcGain) 
        {
            case GAIN_TWOTHIRDS:
                fsRange = 6.144f;
                break;

            case GAIN_ONE:
                fsRange = 4.096f;
                break;

            case GAIN_TWO:
                fsRange = 2.048f;
                break;

            case GAIN_FOUR:
                fsRange = 1.024f;
                break;

            case GAIN_EIGHT:
                fsRange = 0.512f;
                break;

            case GAIN_SIXTEEN:
                fsRange = 0.256f;
                break;

            default:
                fsRange = 0.0f;
        }

        float valueInVolt = this->value * (fsRange / (32768 >> 0));
        
        //Spannungsteiler beachten, falls in verwendung
        if(this->maxVoltage > 5.00)
        {
            valueInVolt = SPANNUNGSTEILER * valueInVolt;
        }

        return valueInVolt;
    }
    void                setAdcGain          (const adsGain_t GAIN){this->adcGain = GAIN;}

    void                setAlarm            (const uint16_t ALARM_VALUE){this->alarmValue = ALARM_VALUE;}
    bool                isAlarmValueReached (){return (this->value >= this->alarmValue);}
    void                setSampleTime       (const uint64_t SAMPLE_TIME){this->to_sampleTime.setInterval(SAMPLE_TIME);}
    
    //Hal handling
    bool            newDataAvailable    ()
    {
        return this->to_sampleTime.checkAndReset();
    }
    void            setHalData          (u_HAL_DATA_t* P_DATA)
    {
        if(P_DATA != nullptr)
        {
            this->value = P_DATA->analogIoData.value;
        }
    }
    u_HAL_DATA_t    getHalData          ()
    {
        u_HAL_DATA_t DATA; 
        memset(&DATA, 0, sizeof(u_HAL_DATA_t));
        DATA.analogIoData.value = this->value;
        return DATA;
    }


    private:
    uint16_t            value;   
    uint16_t            alarmValue;
    float               maxVoltage;
    Timeout             to_sampleTime;
    adsGain_t           adcGain;            //Verstärker einstellung des ADC(Auflösung für Spannugsberechnung)
};

#endif