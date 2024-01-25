#include "BPLC_IOM.h"

//--------------------------------------------------------------------
//MOTOR
MOTOR::MOTOR()
{}

void MOTOR::stop()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 0;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::stopAndBreak()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 255;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::start()
{
	//Bei Start letzte gemerkte Parameter laden 
    this->motParams.direction     = this->motParams.old.direction; 
    this->motParams.speed     	  = this->motParams.old.speed;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::setSpeed(const uint8_t SPEED)
{   
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::setDirection(const e_movement_t DIRECTION)
{
    this->motParams.direction     = DIRECTION; 
	f_thereAreNewDriveParametersAvailable = true;    
}

void MOTOR::setDirectionAndSpeed(const e_movement_t DIRECTION, const uint8_t SPEED)
{
    this->motParams.direction     = DIRECTION; 
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}

float MOTOR::getCurrent()
{
    return this->motParams.current;
}

e_movement_t MOTOR::getDirection()
{
    return this->motParams.direction;
}

uint8_t MOTOR::getSpeed()
{
    return this->motParams.speed;
}

e_DRIVE_STATE_t MOTOR::getDriveState()
{
	return this->driveState;
}

void MOTOR::setCurrent(const float CURRENT)
{
	this->motParams.current = CURRENT;
}

bool MOTOR::newDriveParameterAvailable()
{
	const bool newDriveParameterAvailable 		= this->f_thereAreNewDriveParametersAvailable;
	this->f_thereAreNewDriveParametersAvailable = false;
	return newDriveParameterAvailable;
}


//--------------------------------------------------------------------
//Software H-Bridge
Software_H_Bridge::Software_H_Bridge()
{}

Software_H_Bridge::Software_H_Bridge(Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::begin()
{
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::begin(Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;
	
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::setDirection(const e_movement_t DIRECTION)
{
	switch(DIRECTION)
	{
		case movement_idle:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
		break;

		case movement_left:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
			this->p_R_PWM->setValue(this->driveSpeed);
			this->p_L_PWM->setValue(this->driveSpeed);
		break;

		case movement_right:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->setValue(this->driveSpeed);
			this->p_L_PWM->setValue(this->driveSpeed);
		break;

		default:
			this->driveDirection = movement_idle;
		break;
	}
}

void Software_H_Bridge::setSpeed(const uint8_t SPEED)
{
	this->driveSpeed = SPEED;

	this->p_R_PWM->setValue(SPEED);
	this->p_L_PWM->setValue(SPEED);
}


//--------------------------------------------------------------------
//Servo Motor
servoMotor::servoMotor()
{}

void servoMotor::setMinMaxAngle(const uint16_t MIN, const uint16_t MAX)
{
    this->minAngle = MIN;
    this->maxAngle = MAX;
    //pwmValueCalculator.PCA9685_ServoEval(MIN, MAX);
}

void servoMotor::setServoPosition(const uint16_t POSITION)
{
    uint16_t SERVO_PWM = this->pwmValueCalculator.pwmForAngle(POSITION);
    this->CHANNEL.setValue(SERVO_PWM);
}