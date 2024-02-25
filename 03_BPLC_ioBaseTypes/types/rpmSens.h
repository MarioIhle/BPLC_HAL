#ifndef rpmSens_h
#define rpmSens_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "SpecialFunctions.h"


#define MAX_SAMPLES_UNTIL_CALCULATION   500

class rpmSensor: public IO_Interface
{
    public:
                        rpmSensor           ();
    void                begin               (const uint16_t PULSES_PER_REV = 1, const uint16_t SAMPLE_TIME = 500)
    {
        this->pulsesPerRevolution 	= 1;
        this->startTime 			= millis();
        this->sampleCounter.resetCount();
        this->to_rpmCalculation.setInterval(SAMPLE_TIME);
    }
    uint16_t            getRPM              ()
    {
	    if(to_rpmCalculation.check())
        {
            const double 	TIME_DELTA 		= millis()-this->startTime;
            const double 	SAMPLES			= (double)this->sampleCounter.getCount();
            const double 	REVOLUTIONS		= SAMPLES / this->pulsesPerRevolution;
            const double	RPM 	 		= (REVOLUTIONS/TIME_DELTA) * 60000.00;
            //Alle Werte zurück setzten, neue Messung starten
            this->startTime = millis();		
            this->rpm 		= (uint16_t)RPM;
            this->sampleCounter.resetCount();
            this->to_rpmCalculation.reset();//nach berechnung resetten, zwecks genauigkeit		
        }
        return this->rpm;
    }
    
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return false;}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA = nullptr)
    {
        this->sampleCounter.halCallback(P_DATA);
        return *P_DATA;
    }


    private:     
    e_IO_TYPE_t         ioType;
    counter             sampleCounter;
    unsigned long       startTime;

    uint16_t            rpm;
    uint16_t            pulsesPerRevolution;

    Timeout             to_rpmCalculation;
};

#endif