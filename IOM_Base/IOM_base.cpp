#include "IOM_base.h"

//--------------------------------------------------------------------
//DIGITALINPUT
DigitalInput::DigitalInput    ()
{}

DigitalInput::DigitalInput    (const uint8_t PORT, HAL_DI11* CARD)
{
    this->p_card    = CARD;
    this->port      = PORT;
}

void DigitalInput::begin      (const uint8_t PORT, HAL_DI11* CARD)
{
    this->p_card    = CARD;
    this->port      = PORT;
}

bool DigitalInput::ishigh()
{
    const bool PIN_IS_HIGH = (bool)(this->p_card->digitalInputState[this->port].state == true);

    return PIN_IS_HIGH;
}

bool DigitalInput::islow()
{
    const bool PIN_IS_HIGH = (bool)(this->p_card->digitalInputState[this->port].state == false);

    return PIN_IS_HIGH;
}

bool DigitalInput::posFlank()
{
    const bool POSITIVE_FLANK_OCCURED = (bool)(this->p_card->digitalInputState[this->port].state == true && this->p_card->digitalInputState[this->port].previousState == false);

    return POSITIVE_FLANK_OCCURED;
}

bool DigitalInput::negFlank()
{
    const bool NEGATIVE_FLANK_OCCURED = (bool)(this->p_card->digitalInputState[this->port].state == false && this->p_card->digitalInputState[this->port].previousState == true);

    return NEGATIVE_FLANK_OCCURED;
}


//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder(const uint8_t PORT_A, const uint8_t PORT_B, const uint8_t PORT_PUSHBUTTON, HAL_DI11* CARD)
{   
    this->A.begin(PORT_A, CARD);
    this->B.begin(PORT_B, CARD);
    this->pushButton.begin(PORT_PUSHBUTTON, CARD);   
}

void RotaryEncoder::begin   (const uint8_t PORT_A, const uint8_t PORT_B, const uint8_t PORT_PUSHBUTTON, HAL_DI11* CARD)
{
    this->A.begin(PORT_A, CARD);
    this->B.begin(PORT_B, CARD);
    this->pushButton.begin(PORT_PUSHBUTTON, CARD);  
}

e_direction_t RotaryEncoder::getTurningDirection()
{
    e_direction_t direction = idle;    

    if(this->A.negFlank() && this->B.ishigh())
    {
        direction = right;
    }
    else if(this->B.negFlank() && this->A.ishigh())
    {
        direction = left;
    }

    return direction;
}

bool RotaryEncoder::buttonPressed()
{
    return this->pushButton.posFlank();
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