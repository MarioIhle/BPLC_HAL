#include "BPLC_ioBaseTypes.h"

//--------------------------------------------------------------------
//ROTARY ENCODER
rotaryEncoder::rotaryEncoder()
{
    this->f_invertedDirection   = false;
    this->ioType                = IO_TYPE__ROTARY_ENCODER; 
}
void rotaryEncoder::invertTurningDirection()
{
    this->f_invertedDirection = true;
}   
e_MOVEMENT_t rotaryEncoder::getTurningDirection()
{
    e_MOVEMENT_t direction = MOVEMENT__IDLE;    

    if(this->A.islow() && this->B.fallingEdge())
    {
        if(this->f_invertedDirection == false)
        {
            direction = MOVEMENT__RIGHT;
        }
        else
        {
            direction = MOVEMENT__LEFT;
        }        
    }
    else if(this->B.islow() && this->A.fallingEdge())
    {
        if(this->f_invertedDirection == false)
        {
            direction = MOVEMENT__LEFT;
        }
        else
        {
            direction = MOVEMENT__RIGHT;
        }   
    }

    return direction;
}
bool rotaryEncoder::isButtonPressed()
{
    return this->PB.risingEdge();
}
u_HAL_CALLBACK_DATA_t rotaryEncoder::halCallback(u_HAL_CALLBACK_DATA_t* P_DATA)
{
    u_HAL_CALLBACK_DATA_t BUFFER;
    BUFFER.digitalIoData.state = P_DATA->rotaryEncoderData.stateA;
    this->A.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->rotaryEncoderData.stateB;
    this->B.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->rotaryEncoderData.statePushButton;
    this->PB.halCallback(&BUFFER);
    return BUFFER;
}