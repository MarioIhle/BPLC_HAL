#include "BPLC_IOM.h"

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

e_movement_t RotaryEncoder::getTurningDirection()
{
    e_movement_t direction = movement_idle;    

    if(this->p_A->fallingEdge() && this->p_B->ishigh())
    {
        direction = movement_right;
    }
    else if(this->p_B->fallingEdge() && this->p_A->ishigh())
    {
        direction = movement_left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->p_pushButton->risingEdge();
}