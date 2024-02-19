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
u_HAL_DATA_t rotaryEncoder::halCallback(u_HAL_DATA_t* P_DATA)
{
    u_HAL_DATA_t BUFFER;
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateA;
    this->A.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateB;
    this->B.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateZ;
    this->PB.halCallback(&BUFFER);
    return BUFFER;
}
//--------------------------------------------------------------------
//POSITION ENCODER
u_HAL_DATA_t positionEncoder::halCallback(u_HAL_DATA_t* P_DATA)
{
    u_HAL_DATA_t BUFFER;
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateA;
    this->A.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateB;
    this->B.halCallback(&BUFFER);
    BUFFER.digitalIoData.state = P_DATA->encoderData.stateZ;
    this->Z.halCallback(&BUFFER);
    //"links"                                                
    if((this->A.fallingEdge()   && this->B.ishigh())     
    || (this->A.islow()         && this->B.fallingEdge())    
    || (this->A.risingEdge()    && this->B.islow())    
    || (this->A.ishigh()        && this->B.risingEdge()))   
    {
        this->poitionIncrement.increment();
    }
    //"rechts"                                            
    if((this->A.ishigh()        && this->B.fallingEdge())    
    || (this->A.fallingEdge()   && this->B.islow())    
    || (this->A.islow()         && this->B.risingEdge())    
    || (this->A.risingEdge()    && this->B.ishigh()))   
    {
        this->poitionIncrement.decrement();
    }
    //Z-index
    if(this->Z.risingEdge())
    {
        this->poitionIncrement.resetCount();
    }
    return BUFFER;
}