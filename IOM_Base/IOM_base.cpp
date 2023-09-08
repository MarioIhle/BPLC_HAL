#include "IOM_base.h"

//--------------------------------------------------------------------
//DIGITALINPUT
DigitalInput::DigitalInput    ()
{}

bool DigitalInput::ishigh()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.value == true);

    return PIN_IS_HIGH;
}

bool DigitalInput::islow()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.value == false);

    return PIN_IS_HIGH;
}

bool DigitalInput::posFlank()
{
    const bool POSITIVE_FLANK_OCCURED = (bool)(this->inputState.value == true && this->inputState.previousValue == false);

    return POSITIVE_FLANK_OCCURED;
}

bool DigitalInput::negFlank()
{
    const bool NEGATIVE_FLANK_OCCURED = (bool)(this->inputState.value == false && this->inputState.previousValue == true);

    return NEGATIVE_FLANK_OCCURED;
}

void DigitalInput::setPortState(const bool STATE)
{
	this->inputState.previousValue 	= this->inputState.value;
	this->inputState.value 			= STATE;
}

//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder()
{}

RotaryEncoder::RotaryEncoder(DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON)
{   
    this->p_A = P_PORT_A;
    this->p_B = P_PORT_B;
    this->p_pushButton = P_PORT_PUSHBUTTON;   
}

void RotaryEncoder::begin   (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON)
{
    this->p_A = P_PORT_A;
    this->p_B = P_PORT_B;
    this->p_pushButton = P_PORT_PUSHBUTTON;    
}

e_direction_t RotaryEncoder::getTurningDirection()
{
    e_direction_t direction = idle;    

    if(this->p_A->negFlank() && this->p_B->ishigh())
    {
        direction = right;
    }
    else if(this->p_B->negFlank() && this->p_A->ishigh())
    {
        direction = left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->p_pushButton->posFlank();
}

//--------------------------------------------------------------------
//OUTPUT
Output::Output() 
{
	this->setting.outputType 	= OUTPUTTYPE__OPEN_SOURCE;
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
	this->setting.outputType 	= OUTPUTTYPE__OPEN_SOURCE;
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
	this->setting.outputType 	= OUTPUTTYPE__OPEN_SOURCE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::begin(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::tick()
{
	switch(this->mode)
	{
		case OUTPUTMODE__OFF:
			this->actualValue.value = 0;
			break;

		case OUTPUTMODE__ON:
			this->actualValue.value = this->setting.onValue;
			break;

		case OUTPUTMODE__BLINK:
			if(this->blinkParameter.count < this->blinkParameter.blinks_requested && this->blinkParameter.to_blink.check())
			{      
				if(this->actualValue == 0)
				{
					this->actualValue.value = this->setting.onValue;
					this->blinkParameter.count++;
				}
				else
				{
					this->actualValue = 0;
				}

				this->blinkParameter.to_blink.reset();
			}
			else if(this->blinkParameter.count == this->blinkParameter.blinks_requested)
			{
				this->mode = OUTPUTMODE__OFF;
			}
			break;

        case OUTPUTMODE__BLINK_WITH_BREAK:
            //Blinken
            if(this->blinkParameter.count < this->blinkParameter.blinks_requested && this->blinkParameter.to_blink.check() && this->blinkParameter.to_break.check())
			{      
				if(this->actualValue == 0)
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
            //Blinken abgeschlossen, Break Timeout starten
			else if(this->blinkParameter.count == this->blinkParameter.blinks_requested && this->blinkParameter.to_break.check())
			{
                this->blinkParameter.to_break.reset();                                
			}
            //Break Timeout abwarten
            else
            {
                this->actualValue.value   = 0;
                this->blinkParameter.count   = 0;
            }
            break;

		default:
			this->mode = OUTPUTMODE__OFF;
			break;
	}
	this->actualValue.previousValue = this->actualValue.value;
}

void Output::blink(const uint8_t BLINKS, const int BLINK_INTERVAL)
{
	this->blinkParameter.blinks_requested = BLINKS;
    this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
    this->blinkParameter.count = 0; 

    this->mode = OUTPUTMODE__BLINK;
}

void Output::blinkWithBreak(const uint8_t BLINKS, const int BLINK_INTERVAL, const int BREAK_TIME)
{
	this->blinkParameter.blinks_requested = BLINKS;
    this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
    this->blinkParameter.to_break.setInterval(BREAK_TIME);
    this->blinkParameter.count = 0; 

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

void Output::setOnValue(const uint8_t VALUE)
{
    this->setting.onValue = VALUE;
}

//Achtung kein BOOL! TRUE = max.255
s_portValue_t Output::getValue()
{
	return this->actualValue;	
}

e_outputType_t  Output::getOutputType()
{
	return this->setting.outputType;
}