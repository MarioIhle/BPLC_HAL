#ifndef tempSens_h
#define tempSens_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "SpecialFunctions.h"


class TempSens: public IO_Interface
{
    public:
                        TempSens            (const e_IO_TYPE_t SENS_TYPE, const unsigned long SAMPLE_TIME){this->ioType = SENS_TYPE; this->to_sampleTime.setInterval(SAMPLE_TIME);}
    float               getTemperature      (){return this->temperature;}

    //Hal handling                 
    e_IO_TYPE_t         getIoType           (){return this->ioType;} 
    bool                newDataAvailable    (){return this->to_sampleTime.checkAndReset();}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* DATA = nullptr)
    {
        this->temperature = DATA->tempSensData.temperatur;
        return *DATA;
    }


    private:
    e_IO_TYPE_t         ioType;
    float               temperature;
    Timeout             to_sampleTime;
};
#endif