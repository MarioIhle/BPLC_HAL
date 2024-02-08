#include "BPLC_ioBaseTypes.h"
//--------------------------------------------------------------------
//RPM Sensor
rpmSensor::rpmSensor()
{}

void rpmSensor::begin(const uint16_t PULSES_PER_REV, const uint16_t SAMPLE_TIME)
{
	this->pulsesPerRevolution 	= 1;
	this->startTime 			= millis();
	this->samples				= 0;
	this->to_rpmCalculation.setInterval(SAMPLE_TIME);
}

uint16_t rpmSensor::getRPM()
{
	if(this->samples >= MAX_SAMPLES_UNTIL_CALCULATION || to_rpmCalculation.checkAndReset())
	{
		const double 	TIME_DELTA 		= millis()-this->startTime;
		const double 	REVOLUTIONS		= this->samples / this->pulsesPerRevolution;
		const double	RPM 	 		= (REVOLUTIONS/TIME_DELTA) * 60000.00;
		//Alle Werte zurück setzten, neue Messung starten
		this->startTime = millis();
		this->samples	= 0;
		this->rpm 		= (uint16_t)RPM;
	}
	return this->rpm;
}

void rpmSensor::halCallback(const bool STATE)
{
	this->dataObject.halCallback(STATE);

	if(this->dataObject.risingEdge())
	{
		this->samples++;
	}
}