#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder()
{}

e_movement_t RotaryEncoder::getTurningDirection()
{
    e_movement_t direction = movement_idle;    

    if(this->A.fallingEdge() && this->B.ishigh())
    {
        direction = movement_right;
    }
    else if(this->B.fallingEdge() && this->A.ishigh())
    {
        direction = movement_left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->pushButton.risingEdge();
}