#ifndef output_h
#define output_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "SpecialFunctions.h"

//Art des Output, hat nichts mit der Ansteurung PUSH/PULL zu tun
typedef enum
{
	OUTPUT_SETTING__NORMALY_OPEN,
	OUTPUT_SETTING__NORMALY_CLOSED
}e_outputSetting_t;


// Modus der Ausgeführt wird
typedef enum
{
	OUTPUTMODE__OFF,
	OUTPUTMODE__ON, 
    OUTPUTMODE__VALUE,
	OUTPUTMODE__BLINK_ONCE,
    OUTPUTMODE__BLINK_CONTINIOUS,
	OUTPUTMODE__FADE,

	OUTPUTMODE__SIZE
}e_outputMode_t;
//Statusmaschine Fade
typedef enum
{
	FADE_MODE__IN,
	FADE_MODE__OUT
}e_FADE_MODE_t;

class output: public IO_Interface, private blink
 {
	public:
    //Setup
    output(const e_IO_TYPE_t IO_TYPE = IO_TYPE__OUTPUT_PUSH, const e_outputSetting_t OUTPUT_TYPE = OUTPUT_SETTING__NORMALY_OPEN, const uint8_t ON_VALUE = 255)
	{
		this->ioType				= IO_TYPE;
		this->setting.outputType	= OUTPUT_TYPE;
		this->setting.onValue 		= ON_VALUE;		
		this->f_newDataAvailable	= false;
		this->value					= 0;

		this->mode = OUTPUTMODE__OFF;
	}  
	void begin(const e_IO_TYPE_t IO_TYPE = IO_TYPE__OUTPUT_PUSH, const e_outputSetting_t OUTPUT_TYPE = OUTPUT_SETTING__NORMALY_OPEN, const uint8_t ON_VALUE = 255)	
	{
		this->ioType				= IO_TYPE;
		this->setting.outputType	= OUTPUT_TYPE;
		this->setting.onValue 		= ON_VALUE;		
		this->f_newDataAvailable	= false;
		this->value					= 0;

		this->mode = OUTPUTMODE__OFF;
	}  
	//Setzt den maximalen Wert des Ausgangs
    void setOnValue(const uint8_t VALUE){this->setting.onValue = VALUE;}
    //Applikation Handling
	//Blinkt für angeforderte Anzahl und Interval
	void blinkOnce(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL) 
    {
		this->setupBlink(BLINKS, BLINK_INTERVAL);		
		this->mode = OUTPUTMODE__BLINK_ONCE;	
	}
	//Blinkt dauerhaft mit optinaler Pause
	void blinkContinious(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME)  
	{
		if(this->mode != OUTPUTMODE__BLINK_ONCE)//Blinken zuerst fertig ausführen
		{
			this->setupBlink(BLINKS, BLINK_INTERVAL, BREAK_TIME);	
			this->mode = OUTPUTMODE__BLINK_CONTINIOUS;
		}
	}
	//Lässt den Ausgang endlos faden
	void fade(const unsigned long FADE_IN_TIME, const unsigned long FADE_OUT_TIME)
	{		
		if((this->mode != OUTPUTMODE__FADE) 		//Nicht neustarten, wenn Fade schon läuft
		&& (this->mode != OUTPUTMODE__BLINK_ONCE))	//Blinken zuerst fertig ausführen
		{
			this->fadeSettings.inTime 	= (FADE_IN_TIME	/ this->setting.onValue);
			this->fadeSettings.outTime 	= (FADE_OUT_TIME / this->setting.onValue);
			
			this->fadeSettings.mode 	= FADE_MODE__IN;
			this->fadeSettings.to_fade.setInterval(this->fadeSettings.inTime, false);

			this->mode = OUTPUTMODE__FADE;
		}	
	}
	//Setzt den Ausgang 
	void set()		              
	{
		if(this->mode != OUTPUTMODE__ON)
		{
			this->mode = OUTPUTMODE__ON;
			this->f_newDataAvailable = true;
		}	
	}
	//Setzt den Ausgang zurück
	void reset()		           
	{
		if(this->mode != OUTPUTMODE__OFF)
		{
			this->mode = OUTPUTMODE__OFF;
			this->f_newDataAvailable = true;
		}	
	}
	//Setzten/Rücksetzten über Parameter
    void setState(const bool STATE)      
	{
		if(STATE)
		{
			this->set();
		}
		else
		{
			this->reset();
		}	
	}
	//Setzt einen spezifischen Wert 0-255 
    void setValue(const uint8_t VALUE)   
	{
		if(this->value != VALUE)
		{
			this->value 				= VALUE;
			this->mode 					= OUTPUTMODE__VALUE;
			this->f_newDataAvailable 	= true;
		}	
	}
	//Gibt den Aktuelllen State zurück
	bool getState(){return (OUTPUTMODE__ON == this->mode);}
	//Gibt den Aktuellen Wert zurück
	uint8_t getValue(){return this->value;}
   
    //Hal handling 
	//HAL der Outputkarte prüft auf neue Daten zur ausgabe
    bool 			newDataAvailable	()
	{
		switch(this->mode)
		{
			case OUTPUTMODE__OFF:
				setOutValue(false);
				this->resetBlink();
			break;

			case OUTPUTMODE__ON:
				setOutValue(true);
				this->resetBlink();
			break;

			case OUTPUTMODE__VALUE:
				//keine Änderung, einfach gesetzten Wert halten 
				this->resetBlink();
			break;

			case OUTPUTMODE__BLINK_ONCE:	 
				if(this->getBlinkCycleCount() == 0)   
				{
					if(this->tickBlink() 
					&&(this->value != this->setting.onValue))
					{
						setOutValue(true);		
						this->f_newDataAvailable = true;
					}	
					else if((!this->tickBlink())
						&& (this->value != 0))
					{
						setOutValue(false);		
						this->f_newDataAvailable = true;	
					}		
				}
				else
				{
					this->mode = OUTPUTMODE__OFF;
				}			
			break;

			case OUTPUTMODE__BLINK_CONTINIOUS:
				if(this->tickBlink() 
				&&(this->value != this->setting.onValue))
				{
					setOutValue(true);			
					this->f_newDataAvailable = true;	
				}	
				else if((!this->tickBlink())
					&& (this->value != 0))
				{
					setOutValue(false);		
					this->f_newDataAvailable = true;			
				}		
			break;

			case OUTPUTMODE__FADE:
				//fadeIn	
				if(this->fadeSettings.to_fade.checkAndReset())
				{			
					if(this->fadeSettings.mode == FADE_MODE__IN)
					{						
						this->value++;
											
						if(this->value == this->setting.onValue)
						{
							this->fadeSettings.mode = FADE_MODE__OUT;
							this->fadeSettings.to_fade.setInterval(this->fadeSettings.outTime);
						}
					}					
					//fadeOut
					else if(FADE_MODE__OUT)
					{						
						this->value--;
						
						if(this->value == 0)
						{
							this->fadeSettings.mode = FADE_MODE__IN;
							this->fadeSettings.to_fade.setInterval(this->fadeSettings.inTime);
						}
					}					
					else
					{
						//do nothing
					}
					this->f_newDataAvailable = true;
				}
			break;

			default:
				this->mode = OUTPUTMODE__OFF;
			break;
		}
		return this->f_newDataAvailable;
	}
	u_HAL_DATA_t 	getHalData			(void)
	{
		//Daten an HAL übergeben
		u_HAL_DATA_t DATA;
		memset(&DATA, 0, sizeof(DATA));
		DATA.analogIoData.value 	= this->value;	
		this->f_newDataAvailable 	= false;
		return DATA;
	}
	void 			setHalData			(u_HAL_DATA_t* DATA)
	{
		this->value 				= DATA->analogIoData.value;
		Serial.println("OutValue: " + String(DATA->analogIoData.value));
		this->f_newDataAvailable 	= true;
	}

	private:     
	void setOutValue(const bool STATE)
	{
		switch(this->setting.outputType)
		{
			//Invertiertes Verhalten, öffner
			case OUTPUT_SETTING__NORMALY_CLOSED:
				if(STATE)
				{
					this->value = 0;
				}
				else
				{
					this->value = this->setting.onValue;	
				}
			break;

			//Analog Out
			default:
			case OUTPUT_SETTING__NORMALY_OPEN:				
				if(STATE)
				{
					this->value = this->setting.onValue;	
				}
				else
				{
					this->value = 0;
				}
			break;			
		}	
	}

    e_outputMode_t	    mode;           //Aktueller Modus
    uint8_t             value;  	    //Aktueller Wert

    struct 
    {
        uint8_t 			onValue;			
		e_outputSetting_t	outputType;
    }setting;    

	struct 
	{
		unsigned long 	inTime;
		unsigned long 	outTime;
		Timeout 		to_fade;
		e_FADE_MODE_t	mode;
	}fadeSettings;	
};

#endif