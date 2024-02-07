#include "BPLC_IOM.h"


//--------------------------------------------------------------------
//OUTPUT
Output::Output() 
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= 255;
    this->mode 					= OUTPUTMODE__OFF;
}

Output::Output(const e_outputType_t OUTPUT_TYPE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= 255;
    this->mode 					= OUTPUTMODE__OFF;
}

Output::Output(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

Output::Output(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::begin(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::begin(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::blink(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL)
{
	this->blinkParameter.blinks_requested = BLINKS;
	this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
	this->blinkParameter.to_blink.now();
	this->blinkParameter.count = 0;		
	
	this->mode = OUTPUTMODE__BLINK;	
}

void Output::blinkWithBreak(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME)
{
	const bool ALREADY_IN_BLINK__WITH_BRAKE_MODE 	= (bool)(this->mode == OUTPUTMODE__BLINK_WITH_BREAK);
	const bool PARAMETERS_DID_NOT_CHANGED 			= (bool)((this->blinkParameter.blinks_requested == BLINKS) 
															&&(this->blinkParameter.to_blink.getInterval() == BLINK_INTERVAL)
															&&(this->blinkParameter.to_break.getInterval() == BREAK_TIME));

	if(ALREADY_IN_BLINK__WITH_BRAKE_MODE && PARAMETERS_DID_NOT_CHANGED)
	{
		//nichts ändern, sonst wird Blinktakt immer neu gestartet
	}
	else
	{
		this->blinkParameter.blinks_requested = BLINKS;
		this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
		this->blinkParameter.to_blink.now();
		this->blinkParameter.to_break.setInterval(BREAK_TIME);
		this->blinkParameter.to_break.now();
		this->blinkParameter.count = 0; 
	}
	
    this->mode = OUTPUTMODE__BLINK_WITH_BREAK;
}

void Output::set()
{
	this->mode = OUTPUTMODE__ON;
}

void Output::reset()
{
	this->mode = OUTPUTMODE__OFF;
}

void Output::setState(const bool STATE)
{
	if(STATE == true)
	{
		this->mode = OUTPUTMODE__ON;
	}
	else
	{
		this->mode = OUTPUTMODE__OFF;
	}	
}

void Output::setValue(const uint8_t VALUE)
{
	this->actualValue.value = VALUE;
	this->mode 				= OUTPUTMODE__VALUE;
}

void Output::setOnValue(const uint8_t VALUE)
{
    this->setting.onValue = VALUE;
}

u_IO_DATA_BASE_t Output::halCallback()
{
	//Portdaten wurden abgeholt und können jetzt abgeglichen werden
	this->actualValue.previousValue = this->actualValue.value;
	return this->actualValue;	
}

e_outputType_t Output::getOutputType()
{
	return this->setting.outputType;
}

bool Output::isThereANewPortValue()
{
	//Interne Statusmaschine ticken, um Blinken usw. zu ermöglichen
	this->tick();
	//Wenn aus der Internen Statusmaschine neue Portdaten hervor gehen wird dies hier erkannt
	const bool THERE_IS_A_NEW_PORT_VALUE_TO_WRITE = (bool)(this->actualValue.value != this->actualValue.previousValue);
	return THERE_IS_A_NEW_PORT_VALUE_TO_WRITE;
}

void Output::tick()
{
	switch(this->mode)
	{
		case OUTPUTMODE__OFF:
			this->actualValue.value = false;			
		break;

		case OUTPUTMODE__ON:
			this->actualValue.value = this->setting.onValue;
		break;

		case OUTPUTMODE__VALUE:
			//keine Änderung, einfach gesetzten Wert halten 
		break;

		case OUTPUTMODE__BLINK:
			if(this->blinkParameter.to_blink.check())
			{   
				if(this->blinkParameter.count < this->blinkParameter.blinks_requested)   
				{
					if(this->actualValue.value == 0)
					{
						this->actualValue.value = this->setting.onValue;
						this->blinkParameter.count++;
					}
					else
					{
						this->actualValue.value = 0;
					}

					this->blinkParameter.to_blink.reset();
				}
				else
				{
					this->mode = OUTPUTMODE__OFF;
				}
			}
		break;

        case OUTPUTMODE__BLINK_WITH_BREAK:
            //Blinken
            if(this->blinkParameter.to_break.check())
			{      
				if(this->blinkParameter.to_blink.check())
				{
					if(this->blinkParameter.count < this->blinkParameter.blinks_requested)   
					{
						if(this->actualValue.value == 0)
						{
							this->actualValue.value = this->setting.onValue;
							this->blinkParameter.count++;
						}
						else
						{
							this->actualValue.value = false;
						}
					}
					else
					{
						//Blinken abgeschlossen, Break Timeout starten	
						this->actualValue.value = false;		
                		this->blinkParameter.to_break.reset();                          
						this->blinkParameter.count  = 0;						
					}		
					this->blinkParameter.to_blink.reset();
				}		
			}
		break;

		default:
			this->mode = OUTPUTMODE__OFF;
		break;
	}
}