#include "BPLC_IOM.h"

AnalogInput::AnalogInput(const float MAX_VOLTAGE)
{
	this->maxVoltage = MAX_VOLTAGE;
}
   
uint16_t AnalogInput::getValue()
{
	return this->inputValue.value;
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

  	float valueInVolt = this->inputValue.value * (fsRange / (32768 >> 0));
	
	//Spannungsteiler beachten, falls in verwendung
	if(this->maxVoltage > 5.00)
	{
		valueInVolt = SPANNUNGSTEILER * valueInVolt;
	}

	return valueInVolt;
}

void AnalogInput::setAlarm(const uint16_t ALARM_VALUE)
{
	this->alarmValue = ALARM_VALUE;
}

bool AnalogInput::isAlarmValueReached()
{
	return (bool)(this->inputValue.value >= this->alarmValue);
}

//--------------------------------------------------------------------
//HAL Setter funktionen
void AnalogInput::halCallback(const uint16_t VALUE)
{
	this->inputValue.previousValue 	= this->inputValue.value;
	this->inputValue.value 			= VALUE;
}

void AnalogInput::setADCGain(const adsGain_t ADC_GAIN)
{
	this->adcGain = ADC_GAIN;
}