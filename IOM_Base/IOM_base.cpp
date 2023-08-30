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

