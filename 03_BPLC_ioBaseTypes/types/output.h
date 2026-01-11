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
	OUTPUTMODE__INIT,
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
		this->f_newDataAvailable	= true;
		this->value					= 0;

		this->mode = OUTPUTMODE__INIT;
	}  
	void begin(const e_IO_TYPE_t IO_TYPE = IO_TYPE__OUTPUT_PUSH, const e_outputSetting_t OUTPUT_TYPE = OUTPUT_SETTING__NORMALY_OPEN, const uint8_t ON_VALUE = 255)	
	{
		this->ioType				= IO_TYPE;
		this->setting.outputType	= OUTPUT_TYPE;
		this->setting.onValue 		= ON_VALUE;		
		this->f_newDataAvailable	= true;
		this->value					= 0;

		this->mode = OUTPUTMODE__INIT;
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
		this->mode = OUTPUTMODE__ON;	
	}
	//Setzt den Ausgang zurück
	void reset()		           
	{
		this->mode = OUTPUTMODE__OFF;		
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
				this->f_newDataAvailable = setOutValue(false);
				this->resetBlink();
			break;

			case OUTPUTMODE__ON:
				this->f_newDataAvailable = setOutValue(true);
				this->resetBlink();
			break;

			case OUTPUTMODE__VALUE:
				//keine Änderung, einfach gesetzten Wert halten 
				this->resetBlink();
			break;

			case OUTPUTMODE__BLINK_ONCE:	 
				if(this->getBlinkCycleCount() == 0)   
				{	
					const bool BLINK_STATE 		= this->tickBlink();				
					this->f_newDataAvailable 	= setOutValue(BLINK_STATE);		
				}
				else
				{
					this->mode = OUTPUTMODE__OFF;
				}			
			break;

			case OUTPUTMODE__BLINK_CONTINIOUS:
			{
				const bool BLINK_STATE 		= this->tickBlink();			
				this->f_newDataAvailable 	= setOutValue(BLINK_STATE);			
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
			case OUTPUTMODE__INIT:
			default:				
				this->f_newDataAvailable 	= true;
				this->mode 					= OUTPUTMODE__OFF;
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
		this->f_newDataAvailable 	= true;
	}

	private:     
	bool setOutValue(const bool STATE)
	{
		bool stateChanged = false;
		//Ausgangstatus hat sich geändert->übernehmen
		const bool LOGIC_STATE = this->getLogicState();

		if(STATE != LOGIC_STATE)
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

				//Schließer
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
			stateChanged = true;	
		}	
		return stateChanged;
	}
	bool getLogicState()
	{
		bool logicState = false;
		switch(this->setting.outputType)
		{
			//Invertiertes Verhalten, öffner
			case OUTPUT_SETTING__NORMALY_CLOSED:
				if(this->value == 0)
				{
					logicState = true;
				}				
			break;

			//Analog Out
			default:
			case OUTPUT_SETTING__NORMALY_OPEN:				
				if(this->value == this->setting.onValue)
				{
					logicState = true;
				}
			break;			
		}	
		return logicState;
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