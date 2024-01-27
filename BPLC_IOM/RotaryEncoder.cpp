#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//ROTARY ENCODER
rotaryEncoder::rotaryEncoder()
{}

e_movement_t rotaryEncoder::getTurningDirection()
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

bool rotaryEncoder::isButtonPressed()
{
    return this->Z.risingEdge();
}

void rotaryEncoder::halCallback(const bool STATE_A, const bool STATE_B, const bool STATE_Z)
{
    this->A.halCallback(STATE_A);
    this->B.halCallback(STATE_B);
    this->Z.halCallback(STATE_Z);
}