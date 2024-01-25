#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder()
{}

e_movement_t RotaryEncoder::getTurningDirection()
{
    e_movement_t direction = movement_idle;    

    if(this->A.islow() && this->B.fallingEdge())
    {
        direction = movement_right;
    }
    else if(this->B.islow() && this->A.fallingEdge())
    {
        direction = movement_left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->Z.risingEdge();
}

void RotaryEncoder::halCallback(const bool STATE_A, const bool STATE_B, const bool STATE_Z)
{
    this->A.halCallback(STATE_A);
    this->B.halCallback(STATE_B);
    this->Z.halCallback(STATE_Z);
}