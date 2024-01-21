#include "BPLC_IOM.h"


//--------------------------------------------------------------------
//ANALOGINPUT
AnalogInput::AnalogInput()
{}
   
void AnalogInput::setMaxVoltage(const float VOLTAGE)
{
	this->maxVoltage = VOLTAGE;
}

uint16_t AnalogInput::getValue()
{
	return this->inputValue.value;
}

float AnalogInput::getValueInVolt()
{
	float VALUE_IN_VOLT = this->rawPortVoltage;
	
	//Spannungsteiler beachten
	if(this->maxVoltage > 5.00)
	{
		VALUE_IN_VOLT = SPANNUNGSTEILER * VALUE_IN_VOLT;
	}

	return VALUE_IN_VOLT;
}

void AnalogInput::setAlarm(const uint16_t ALARM_VALUE)
{
	this->alarmValue = ALARM_VALUE;
}

bool AnalogInput::isAlarmValueReached()
{
	return (bool)(this->inputValue.value >= this->alarmValue);
}

void AnalogInput::setPortValue(const uint16_t VALUE)
{
	this->inputValue.previousValue 	= this->inputValue.value;
	this->inputValue.value 			= VALUE;
}

void AnalogInput::setRawPortVoltage(const float PORT_VOLTAGE)
{
	this->rawPortVoltage = PORT_VOLTAGE;
}