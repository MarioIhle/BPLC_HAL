#include "BPLC_ioBaseTypes.h"

AnalogInput::AnalogInput(const float MAX_VOLTAGE)
{
	this->maxVoltage 	= MAX_VOLTAGE;	
	this->ioType	 	= IO_TYPE__ANALOG_INPUT;
    this->value			= 0;   
    this->alarmValue	= 0;
    this->adcGain		= GAIN_TWOTHIRDS;   
	this->to_sampleTime.setInterval(1000);         
}   
float AnalogInput::getValueInVolt()
{
	// see data sheet Table 3
	float fsRange;
	switch(this->adcGain) 
	{
		case GAIN_TWOTHIRDS:
			fsRange = 6.144f;
			break;

		case GAIN_ONE:
			fsRange = 4.096f;
			break;

		case GAIN_TWO:
			fsRange = 2.048f;
			break;

		case GAIN_FOUR:
			fsRange = 1.024f;
			break;

		case GAIN_EIGHT:
			fsRange = 0.512f;
			break;

		case GAIN_SIXTEEN:
			fsRange = 0.256f;
			break;

		default:
			fsRange = 0.0f;
	}

  	float valueInVolt = this->value * (fsRange / (32768 >> 0));
	
	//Spannungsteiler beachten, falls in verwendung
	if(this->maxVoltage > 5.00)
	{
		valueInVolt = SPANNUNGSTEILER * valueInVolt;
	}

	return valueInVolt;
}