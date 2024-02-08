#include "BPLC_ioBaseTypes.h"

//--------------------------------------------------------------------
//dcDrive
dcDrive::dcDrive()
{}
void dcDrive::stop()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 0;   
	f_thereAreNewDriveParametersAvailable = true;
}
void dcDrive::stopAndBreak()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 255;   
	f_thereAreNewDriveParametersAvailable = true;
}
void dcDrive::start()
{
	//Bei Start letzte gemerkte Parameter laden 
    this->motParams.direction     = this->motParams.old.direction; 
    this->motParams.speed     	  = this->motParams.old.speed;   
	f_thereAreNewDriveParametersAvailable = true;
}
void dcDrive::setSpeed(const uint8_t SPEED)
{   
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}
void dcDrive::setDirection(const e_movement_t DIRECTION)
{
    this->motParams.direction     = DIRECTION; 
	f_thereAreNewDriveParametersAvailable = true;    
}
void dcDrive::setDirectionAndSpeed(const e_movement_t DIRECTION, const uint8_t SPEED)
{
    this->motParams.direction     = DIRECTION; 
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}
float dcDrive::getCurrent()
{
    return this->motParams.current;
}
e_movement_t dcDrive::getDirection()
{
    return this->motParams.direction;
}
uint8_t dcDrive::getSpeed()
{
    return this->motParams.speed;
}
e_DRIVE_STATE_t dcDrive::getDriveState()
{
	return this->driveState;
}
void dcDrive::setCurrent(const float CURRENT)
{
	this->motParams.current = CURRENT;
}
bool dcDrive::newDriveParameterAvailable()
{
	const bool newDriveParameterAvailable 		= this->f_thereAreNewDriveParametersAvailable;
	this->f_thereAreNewDriveParametersAvailable = false;
	return newDriveParameterAvailable;
}
//--------------------------------------------------------------------
//Servo Motor
servoMotor::servoMotor()
{
	this->pwmValue 					= 0;
	this->f_newPositionAvailable 	= true;
}
void servoMotor::begin(const uint16_t MIN, const uint16_t MAX)
{
    this->minAngle = MIN;
    this->maxAngle = MAX;
}
void servoMotor::setServoPosition(const uint16_t POSITION)
{		
    this->pwmValue = map(POSITION, 180, 0, 136, 363);
	this->f_newPositionAvailable = true;
}
u_IO_DATA_BASE_t servoMotor::halCallback(u_IO_DATA_BASE_t DATA)
{
	u_IO_DATA_BASE_t BUFFER;
	BUFFER.analogIoData.value = this->pwmValue;
	this->f_newPositionAvailable = false;
		
	return BUFFER;
}