#ifndef rpmSens_h
#define rpmSens_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "SpecialFunctions.h"
#include "BPLC_logPrint.h"

class rpmSensor: public IO_Interface
{
    public:
    rpmSensor()
    {
        this->ioType = IO_TYPE__RPM_SENS;       
    }

    void begin(const uint64_t PULSES_PER_REV = 1, const uint64_t SAMPLE_TIME = 500)
    {
        this->pulsesPerRevolution 	= PULSES_PER_REV;
        this->startTime 			= millis();
        this->rpm                   = 0;

        this->sampleCounter.resetCount();
        this->to_rpmCalculation.setInterval(SAMPLE_TIME);        
    }

    uint64_t getRPM()
    {

        if(to_rpmCalculation.checkAndReset())
        {                
            //Daten vorladen                
            const uint64_t  OS_TIME                 = micros();
            const uint64_t  SAMPLES			        = this->sampleCounter.getCount();                                 
            const uint64_t  START_TIME              = this->startTime;
            const uint64_t  TIME_DELTA 		        = (OS_TIME - START_TIME);
            const uint64_t  PULSES_PER_REVOLUTION   = this->pulsesPerRevolution;                                           
            //Drehzahl berechnen
            const double REVOLUTIONS	            = (double)(SAMPLES / PULSES_PER_REVOLUTION);
            const double TIME_PER_REV               = (REVOLUTIONS/TIME_DELTA);
            const double CALCULATED_RPM             = (TIME_PER_REV * 60000000.00);
            //Drehzahl glätten
            //const double RPM_OLD                    = (double)this->rpm;
            this->rpm                               = (uint64_t)CALCULATED_RPM; //(CALCULATED_RPM * 0.25) + (RPM_OLD * 0.75);
    
            //Alle Werte zurück setzten, neue Messung starten      
            this->startTime = micros();           	
            this->sampleCounter.resetCount();              	                
        }        
	    
        return this->rpm;
    }
    
    //Hal handling
    bool            newDataAvailable(){return false;}
    void            setHalData      (u_HAL_DATA_t* P_DATA)
    {        
        if(P_DATA != nullptr)
        { 
            this->sampleCounter.setHalData(P_DATA);      
        }
    }
    u_HAL_DATA_t    getHalData      ()
    {
        u_HAL_DATA_t DATA; 
        memset(&DATA, 0, sizeof(u_HAL_DATA_t));

        return DATA;
    }


    private:     
    counter     sampleCounter;
    uint64_t    startTime;

    uint64_t    rpm;
    uint64_t    pulsesPerRevolution;   

    Timeout     to_rpmCalculation;
};

#endif