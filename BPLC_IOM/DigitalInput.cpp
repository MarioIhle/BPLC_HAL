#include "BPLC_IOM.h"

DigitalInput::DigitalInput    ()
{}

bool DigitalInput::ishigh()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.value == true && this->inputState.previousValue == true);

#ifdef DEBUG_IOM
if(PIN_IS_HIGH)
{
	Serial.println("PIN_IS_HIGH");	
}
#endif

    return PIN_IS_HIGH;
}

bool DigitalInput::islow()
{
    const bool PIN_IS_LOW = (bool)(this->inputState.value == false && this->inputState.previousValue == false);

#ifdef DEBUG_IOM
if(PIN_IS_LOW)
{
	Serial.println("PIN_IS_LOW");	
}
#endif

    return PIN_IS_LOW;
}

bool DigitalInput::posEdge()
{
    const bool POSITIVE_FLANK_OCCURED = (bool)(this->inputState.value == true && this->inputState.previousValue == false);

#ifdef DEBUG_IOM
if(POSITIVE_FLANK_OCCURED)
{
	Serial.println("POSITIVE_FLANK_OCCURED");	
}
#endif

    return POSITIVE_FLANK_OCCURED;
}

bool DigitalInput::negEdge()
{
    const bool NEGATIVE_FLANK_OCCURED = (bool)(this->inputState.value == false && this->inputState.previousValue == true);

#ifdef DEBUG_IOM
if(NEGATIVE_FLANK_OCCURED)
{
	Serial.println("NEGATIVE_FLANK_OCCURED");	
}
#endif

    return NEGATIVE_FLANK_OCCURED;
}

void DigitalInput::setPortState(const bool STATE)
{
	this->inputState.previousValue 	= this->inputState.value;
	this->inputState.value 			= STATE;
}