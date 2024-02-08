#include "BPLC_ioBaseTypes.h"

//--------------------------------------------------------------------
//OUTPUT
Output::Output(const e_outputType_t OUTPUT_TYPE = OUTPUTTYPE__PUSH, const uint8_t ON_VALUE = 255) 
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}
void Output::setOnValue(const uint8_t VALUE)
{
    this->setting.onValue = VALUE;
}
//Digital Setter
void Output::set()
{
	if(this->mode != OUTPUTMODE__ON)
	{
		this->mode = OUTPUTMODE__ON;
		this->f_newDataAvailable = true;
	}	
}
void Output::reset()
{
	if(this->mode != OUTPUTMODE__OFF)
	{
		this->mode = OUTPUTMODE__OFF;
		this->f_newDataAvailable = true;
	}	
}
void Output::setState(const bool STATE)
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
//Analog Setter
void Output::setValue(const uint8_t VALUE)
{
	if(this->value != VALUE)
	{
		this->actualValue.value 	= VALUE;
		this->mode 					= OUTPUTMODE__VALUE;
		this->f_newDataAvailable 	= true;
	}	
}
//Blinken
void Output::blinkOnce(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL)
{
	this->setupBlink(BLINKS, BLINK_INTERVAL);		
	this->mode = OUTPUTMODE__BLINK_ONCE;	
}
void Output::blinkContinious(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME)
{
	this->setupBlink(BLINKS, BLINK_INTERVAL, BREAK_TIME);	
    this->mode = OUTPUTMODE__BLINK_CONTINIOUS;
}
//Hal Handling
bool Output::newDataAvailable()
{
	switch(this->mode)
	{
		case OUTPUTMODE__OFF:
			this->value = false;	
		break;

		case OUTPUTMODE__ON:
			this->value = this->setting.onValue;
		break;

		case OUTPUTMODE__VALUE:
			//keine Änderung, einfach gesetzten Wert halten 
		break;

		case OUTPUTMODE__BLINK_ONCE:	 
			if(this->getBlinkCycleCount() < 0)   
			{
				if(this->tickBlink() != this->value)
				{
					this->value = this->tickBlink();
					this->f_newDataAvailable = true;
				}
			}
			else
			{
				this->mode = OUTPUTMODE__OFF;
			}			
		break;

        case OUTPUTMODE__BLINK_CONTINIOUS:
			if(this->tickBlink() != this->value)
			{
				this->value = this->tickBlink();
				this->f_newDataAvailable = true;
			}			
		break;

		default:
			this->mode = OUTPUTMODE__OFF;
		break;
	}
	return this->f_newDataAvailable;
}