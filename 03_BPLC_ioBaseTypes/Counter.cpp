#include "BPLC_ioBaseTypes.h"
//--------------------------------------------------------------------
//RPM Sensor
rpmSensor::rpmSensor()
{}
void rpmSensor::begin(const uint16_t PULSES_PER_REV, const uint16_t SAMPLE_TIME)
{
	this->pulsesPerRevolution 	= 1;
	this->startTime 			= millis();
	this->sampleCounter.resetCount();
	this->to_rpmCalculation.setInterval(SAMPLE_TIME);
}
uint16_t rpmSensor::getRPM()
{
	if(to_rpmCalculation.check())
	{
		const double 	TIME_DELTA 		= millis()-this->startTime;
		const double 	SAMPLES			= (double)this->sampleCounter.getCount();
		const double 	REVOLUTIONS		= SAMPLES / this->pulsesPerRevolution;
		const double	RPM 	 		= (REVOLUTIONS/TIME_DELTA) * 60000.00;
		//Alle Werte zurück setzten, neue Messung starten
		this->startTime = millis();		
		this->rpm 		= (uint16_t)RPM;
		this->sampleCounter.resetCount();
		this->to_rpmCalculation.reset();//nach berechnung resetten, zwecks genauigkeit		
	}
	return this->rpm;
}
u_HAL_CALLBACK_DATA_t rpmSensor::halCallback(u_HAL_CALLBACK_DATA_t* P_DATA)
{
	this->sampleCounter.halCallback(P_DATA);
	return *P_DATA;
}