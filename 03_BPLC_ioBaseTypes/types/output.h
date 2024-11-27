#ifndef output_h
#define output_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "SpecialFunctions.h"


typedef enum
{
	OUTPUTMODE__OFF,
	OUTPUTMODE__ON, 
    OUTPUTMODE__VALUE,
	OUTPUTMODE__BLINK_ONCE,
    OUTPUTMODE__BLINK_CONTINIOUS,

	OUTPUTMODE__SIZE,
}e_outputMode_t;

class output: public IO_Interface, blink
 {
	public:
    //Setup
                        output          (const e_IO_TYPE_t OUTPUT_TYPE = IO_TYPE__OUTPUT_PUSH, const uint8_t ON_VALUE = 255)
	{
		this->ioType 				= OUTPUT_TYPE;
		this->setting.onValue 		= ON_VALUE;
		this->mode 					= OUTPUTMODE__OFF;
	}  
	void 				begin			(const e_IO_TYPE_t OUTPUT_TYPE = IO_TYPE__OUTPUT_PUSH, const uint8_t ON_VALUE = 255)	
	{
		this->ioType 				= OUTPUT_TYPE;
		this->setting.onValue 		= ON_VALUE;
		this->mode 					= OUTPUTMODE__OFF;
	}  
    void                setOnValue      (const uint8_t VALUE){this->setting.onValue = VALUE;}
    //Applikation Handling
	void                blinkOnce		(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL)			                        //Blinkt für angeforderte Anzahl und Interval
    {
		this->setupBlink(BLINKS, BLINK_INTERVAL);		
		this->mode = OUTPUTMODE__BLINK_ONCE;	
	}
	void                blinkContinious (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME)  //Blinkt dauerhaft mit optinaler Pause
	{
		this->setupBlink(BLINKS, BLINK_INTERVAL, BREAK_TIME);	
		this->mode = OUTPUTMODE__BLINK_CONTINIOUS;
	}
	void                set			    ()		                //output ON
	{
		if(this->mode != OUTPUTMODE__ON)
		{
			this->mode = OUTPUTMODE__ON;
			this->f_newDataAvailable = true;
		}	
	}
	void                reset		    ()		                //output OFF
	{
		if(this->mode != OUTPUTMODE__OFF)
		{
			this->mode = OUTPUTMODE__OFF;
			this->f_newDataAvailable = true;
		}	
	}
    void                setState        (const bool STATE)      //ON/OFF
	{
		if(STATE == true)
		{
			this->set();
		}
		else
		{
			this->reset();
		}	
	}
    void                setValue        (const uint8_t VALUE)   //0-255
	{
		if(this->value != VALUE)
		{
			this->value 				= VALUE;
			this->mode 					= OUTPUTMODE__VALUE;
			this->f_newDataAvailable 	= true;
		}	
	}
	bool	 			getState		(){return (bool)this->value;}
	uint8_t 			getValue		(){return this->value;}
   
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable()
	{
		switch(this->mode)
		{
			case OUTPUTMODE__OFF:
				this->value = false;	
				this->resetBlink();
			break;

			case OUTPUTMODE__ON:
				this->value = this->setting.onValue;
				this->resetBlink();
			break;

			case OUTPUTMODE__VALUE:
				//keine Änderung, einfach gesetzten Wert halten 
				this->resetBlink();
			break;

			case OUTPUTMODE__BLINK_ONCE:	 
				if(this->getBlinkCycleCount() == 0)   
				{
					if(this->tickBlink() == true && this->value != this->setting.onValue)
					{
						this->value = this->setting.onValue;
						this->f_newDataAvailable = true;
					}	
					else if(this->tickBlink() == false && this->value != 0)
					{
						this->value = 0;
						this->f_newDataAvailable = true;
					}	
				}
				else
				{
					this->mode = OUTPUTMODE__OFF;
				}			
			break;

			case OUTPUTMODE__BLINK_CONTINIOUS:
				if(this->tickBlink() == true && this->value != this->setting.onValue)
				{
					this->value = this->setting.onValue;
					this->f_newDataAvailable = true;
				}	
				else if(this->tickBlink() == false && this->value != 0)
				{
					this->value = 0;
					this->f_newDataAvailable = true;
				}		
			break;

			default:
				this->mode = OUTPUTMODE__OFF;
			break;
		}
		return this->f_newDataAvailable;
	}
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA)
	{
		u_HAL_DATA_t tempBuffer;
		tempBuffer.analogIoData.value = this->value;
		this->f_newDataAvailable = false;
		return tempBuffer;
	}


	private: 
    e_IO_TYPE_t         ioType;
    e_outputMode_t	    mode;           //Aktueller Modus
    uint8_t             value;  	    //Aktueller Wert
    bool                f_newDataAvailable;

    struct //Hauptsächlich für verarbeitende HAL interessant
    {
        uint8_t         onValue;	    //Welcher Wert wird geschieben bei object.set():
    }setting;    
};

#endif