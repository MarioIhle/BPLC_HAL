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
        this->ioType                = IO_TYPE__RPM_SENS;       
    }

    void begin(const uint64_t PULSES_PER_REV = 1, const uint64_t SAMPLE_TIME = 500)
    {
        this->pulsesPerRevolution 	= PULSES_PER_REV;
        this->startTime 			= millis();
        this->rpm                   = 0;
        this->p_sampleCounter       = new counter;

        this->p_sampleCounter->resetCount();
        this->to_rpmCalculation.setInterval(SAMPLE_TIME);        
    }

    void begin(counter* P_SAMPLE_COUNTER, const uint64_t PULSES_PER_REV = 1, const uint64_t SAMPLE_TIME = 500)
    {
        this->p_sampleCounter       = P_SAMPLE_COUNTER;
        this->pulsesPerRevolution 	= (PULSES_PER_REV * 2);   //wenn externer Counter verwendet wird, ist das wahrscheinlich direkt die INT_ISR, diese wird 2x pro periode getriggert, daher PULSES_PER_REV*2
        this->startTime 			= millis();
        this->rpm                   = 0;
        
        this->p_sampleCounter->resetCount();
        this->to_rpmCalculation.setInterval(SAMPLE_TIME);        
    }

    uint64_t getRPM()
    {
        if(this->p_sampleCounter != nullptr)
        {
            if(to_rpmCalculation.checkAndReset())
            {
                //Daten vorladen
                const double OS_TIME                    = (double)millis();
                const double SAMPLES			        = (double)this->p_sampleCounter->getCount();                 
                const double START_TIME                 = (double)this->startTime;
                const double TIME_DELTA 		        = (OS_TIME - START_TIME);
                const double PULSES_PER_REVOLUTION      = (double)this->pulsesPerRevolution;                                           
                //Drehzahl berechnen
                const double REVOLUTIONS	            = (SAMPLES / PULSES_PER_REVOLUTION);
                const double TIME_PER_REV               = (REVOLUTIONS/TIME_DELTA);
                const double CALCULATED_RPM             = (TIME_PER_REV * 60000.00);
                //Drehzahl glätten
                const double RPM_OLD                    = (double)this->rpm;
                this->rpm                               = (uint64_t)CALCULATED_RPM; //(CALCULATED_RPM * 0.25) + (RPM_OLD * 0.75);
        
                //Alle Werte zurück setzten, neue Messung starten 
                this->startTime = millis();	
                this->p_sampleCounter->resetCount();    	
            }
        }
        else
        {
            BPLC_logPrint log;
            log.printResetReason("RPM_SENS_NOT_INITILIZED", __FILENAME__, __LINE__);
            ESP.restart();
        }
	    
        return this->rpm;
    }
    
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return false;}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA = nullptr)
    {
        if(this->p_sampleCounter != nullptr)
        {
            this->p_sampleCounter->halCallback(P_DATA);
        }
        else
        {
            BPLC_logPrint log;
            log.printResetReason("RPM_SENS_NOT_INITILIZED", __FILENAME__, __LINE__);
            ESP.restart();
        }
        
        return *P_DATA;
    }


    private:     
    e_IO_TYPE_t ioType;
    counter*    p_sampleCounter;
    uint64_t    startTime;

    uint64_t    rpm;
    uint64_t    pulsesPerRevolution;   

    Timeout     to_rpmCalculation;
};

#endif