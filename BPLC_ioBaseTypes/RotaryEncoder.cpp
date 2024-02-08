#include "BPLC_ioBaseTypes.h"

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
u_IO_DATA_BASE_t rotaryEncoder::halCallback(u_IO_DATA_BASE_t DATA)
{
    u_IO_DATA_BASE_t BUFFER;
    BUFFER.digitalIoData.state = DATA.rotaryEncoderData.stateA;
    this->A.halCallback(BUFFER);
    BUFFER.digitalIoData.state = DATA.rotaryEncoderData.stateB;
    this->B.halCallback(BUFFER);
    BUFFER.digitalIoData.state = DATA.rotaryEncoderData.statePushButton;
    this->Z.halCallback(BUFFER);
    return DATA;
}