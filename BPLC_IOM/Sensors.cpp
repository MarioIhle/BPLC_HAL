#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//PT10x 
PT10x::PT10x()
{}

void PT10x::begin(AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_VCC)
{
	this->p_PORT_1 	 = P_PORT_1;
	this->p_PORT_2 	 = P_PORT_2;
	this->p_PORT_VCC = P_PORT_VCC;
}
#define mvPerDegAtVCC1mV	0.00060		//mV
#define mvPerBit			0.1875		//mV

float PT10x::getTemperatur()
{
	const double VOLATGE_PORT_A_mV 	= (double)this->p_PORT_1->getValue() * mvPerBit;
	const double VOLATGE_PORT_B_mV 	= (double)this->p_PORT_2->getValue() * mvPerBit;	
	const double VOLTAGE_AB_mV 		= VOLATGE_PORT_A_mV	- VOLATGE_PORT_B_mV;
	const double VOLTAGE_VCC_mV		= (double)this->p_PORT_VCC->getValue() * mvPerBit;
	const double mVPerDeg			= mvPerDegAtVCC1mV * VOLTAGE_VCC_mV;
	const float	 TEMPERATUR			= VOLTAGE_AB_mV / mVPerDeg;		

	Serial.print("mVA: "); Serial.print(VOLATGE_PORT_A_mV);
	Serial.print(", mVB: "); Serial.print(VOLATGE_PORT_B_mV);
	Serial.print(", VOLTAGE_AB_mV: "); Serial.print(VOLTAGE_AB_mV);
	Serial.print(", VOLTAGE_VCC_mV: "); Serial.print(VOLTAGE_VCC_mV);
	Serial.print(", mVPerDeg: "); Serial.print(mVPerDeg);
	Serial.print(", TEMPERTUR: "); Serial.println(TEMPERATUR);

	return TEMPERATUR;
}


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

void rpmSensor::tick()
{
	if(this->CHANNEL.risingEdge())
	{
		this->samples++;
	}
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