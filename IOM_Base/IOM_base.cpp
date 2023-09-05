#include "IOM_base.h"

//--------------------------------------------------------------------
//DIGITALINPUT
DigitalInput::DigitalInput    ()
{}

bool DigitalInput::ishigh()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.state == true);

    return PIN_IS_HIGH;
}

bool DigitalInput::islow()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.state == false);

    return PIN_IS_HIGH;
}

bool DigitalInput::posFlank()
{
    const bool POSITIVE_FLANK_OCCURED = (bool)(this->inputState.state == true && this->inputState.previousState == false);

    return POSITIVE_FLANK_OCCURED;
}

bool DigitalInput::negFlank()
{
    const bool NEGATIVE_FLANK_OCCURED = (bool)(this->inputState.state == false && this->inputState.previousState == true);

    return NEGATIVE_FLANK_OCCURED;
}

void DigitalInput::setPortState(const bool STATE)
{
	this->inputState.previousState = this->inputState.state;
	this->inputState.state = STATE;
}

//--------------------------------------------------------------------
//ROTARY ENCODER
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

bool RotaryEncoder::buttonPressed()
{
    return this->p_pushButton->posFlank();
}


//--------------------------------------------------------------------
//OUTPUT
Output::Output() 
{}

Output::Output(const uint8_t ON_VALUE)
{
	this->onValue = ON_VALUE;

    this->mode = OUTPUTMODE__OFF;
}

void Output::begin(const uint8_t ON_VALUE)
{
	this->onValue = ON_VALUE;

    this->mode = OUTPUTMODE__OFF;
}

void Output::tick()
{
	switch(this->mode)
	{
		case OUTPUTMODE__OFF:
			this->actualValue = 0;
			break;

		case OUTPUTMODE__ON:
			this->actualValue = this->onValue;
			break;

		case OUTPUTMODE__BLINK:
			if(this->count < this->blinks_requested && this->to_blink.check())
			{      
				if(this->actualValue == 0)
				{
					this->actualValue = this->onValue;
					this->count++;
				}
				else
				{
					this->actualValue = 0;
				}

				this->to_blink.reset();
			}
			else if(this->count == this->blinks_requested)
			{
				this->mode = OUTPUTMODE__OFF;
			}
			break;

        case OUTPUTMODE__BLINK_WITH_BREAK:
            //Blinken
            if(this->count < this->blinks_requested && this->to_blink.check() && this->to_break.check())
			{      
				if(this->actualValue == 0)
				{
					this->actualValue = this->onValue;
					this->count++;
				}
				else
				{
					this->actualValue = 0;
				}
				this->to_blink.reset();
			}
            //Blinken abgeschlossen, Break Timeout starten
			else if(this->count == this->blinks_requested && this->to_break.check())
			{
                this->to_break.reset();                                
			}
            //Break Timeout abwarten
            else
            {
                this->actualValue   = 0;
                this->count         = 0;
            }
            break;

		default:
			this->mode = OUTPUTMODE__OFF;
			break;
	}
}

void Output::blink(const uint8_t BLINKS, const int BLINK_INTERVAL)
{
	this->blinks_requested = BLINKS;
    this->to_blink.setInterval(BLINK_INTERVAL);
    this->count = 0; 

    this->mode = OUTPUTMODE__BLINK;
}

void Output::blinkWithBreak(const uint8_t BLINKS, const int BLINK_INTERVAL, const int BREAK_TIME)
{
	this->blinks_requested = BLINKS;
    this->to_blink.setInterval(BLINK_INTERVAL);
    this->to_break.setInterval(BREAK_TIME);
    this->count = 0; 

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

void Output::setValue(const uint8_t VALUE)
{
    this->actualValue = VALUE;
}

uint8_t Output::getValue()
{
	return this->actualValue;
}