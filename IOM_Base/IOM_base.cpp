#include "IOM_base.h"

//--------------------------------------------------------------------
//DIGITALINPUT
DigitalInput::DigitalInput()
{

}

DigitalInput::DigitalInput(const unint8_t PIN)
{
    this->pin = PIN;
    pinMode(PIN, INPUT);
}

void DigitalInput::begin(const unint8_t PIN)
{
    this->pin = PIN;
    pinMode(PIN, INPUT);
}

bool DigitalInput::ishigh()
{
    bool pinIsHigh = false;

    if(digitalRead(this->pin) == true)
    {
        pinIsHigh = true;
    }

    return pinIsHigh;
}

bool DigitalInput::islow()
{
    bool pinIsLow = false;

    if(digitalRead(this->pin) == false)
    {
        pinIsLow = true;
    }

    return pinIsLow;
}

bool DigitalInput::posFlank()
{
    bool positiveFlankOccured = false;

    const bool STATE == digitalRead(this->pin);

    if(STATE == true && lasteState == false)
    {
        positiveFlankOccured = true;
        lasteState = STATE;
    }

    return positiveFlankOccured;
}

bool DigitalInput::negFlank()
{
    bool negativeFlankOccured = false;

    const bool STATE == digitalRead(this->pin);

    if(STATE == false && lasteState == true)
    {
        negativeFlankOccured = true;    
        lasteState = STATE;
    }

return negativeFlankOccured;
}


//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder(const unit8_t A, const unit8_t B, const unit8_t Z)
{
    //Klasse Flankenauswertung nutzen um DigitalInput zu defienieren
    this->A.begin(A);
    this->B.begin(B);
    this->Z.begin(Z);
}

void RotaryEncoder::begin   (const unit8_t A, const unit8_t B, const unit8_t Z)
{
    this->A.begin(A);
    this->B.begin(B);
    this->Z.begin(Z);
}

e_direction_t RotaryEncoder::getTurningDirection()
{
    e_direction_t direction = idle;    

    if(this->A.negFlank() && this->B.high())
    {
        direction = right;
    }
    else if(this->B.negFlank() && this->A.high())
    {
        direction = left;
    }

    return direction;
}

bool RotaryEncoder::buttonPressed()
{
    return this->Z.posFlank();
}

