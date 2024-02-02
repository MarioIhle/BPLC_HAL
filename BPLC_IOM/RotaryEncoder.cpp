#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//ROTARY ENCODER
rotaryEncoder::rotaryEncoder()
{
    this->f_invertedDirection = false;
}

void rotaryEncoder::invertTurningDirection()
{
    this->f_invertedDirection = true;
}   

e_movement_t rotaryEncoder::getTurningDirection()
{
    e_movement_t direction = movement_idle;    

    if(this->A.islow() && this->B.fallingEdge())
    {
        if(this->f_invertedDirection == false)
        {
            direction = movement_right;
        }
        else
        {
            direction = movement_left;
        }        
    }
    else if(this->B.islow() && this->A.fallingEdge())
    {
        if(this->f_invertedDirection == false)
        {
            direction = movement_left;
        }
        else
        {
            direction = movement_right;
        }   
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