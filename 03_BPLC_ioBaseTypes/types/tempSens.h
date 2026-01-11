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
    bool                temeratureChanged   (){bool TEMP_CHANGED = (bool)(this->f_tempChanged == true); this->f_tempChanged = false; return TEMP_CHANGED;}

    //Hal handling                 
    bool            newDataAvailable    (){return this->to_sampleTime.checkAndReset();}
    void            setHalData          (u_HAL_DATA_t* P_DATA)
    {
        if(P_DATA != nullptr)
        {
            if(this->temperature != P_DATA->tempSensData.temperatur)
            {
                this->f_tempChanged = true;
                this->temperature = P_DATA->tempSensData.temperatur;
            }   
        }     
    }
    u_HAL_DATA_t    getHalData          ()
    {
        u_HAL_DATA_t DATA; 
        memset(&DATA, 0, sizeof(u_HAL_DATA_t));
        DATA.tempSensData.temperatur = this->temperature;
        return DATA;
    }

    private:
    float               temperature;
    Timeout             to_sampleTime;
    bool                f_tempChanged;
};
#endif