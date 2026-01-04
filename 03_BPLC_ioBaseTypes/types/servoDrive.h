#ifndef servoDrive_h
#define servoDrive_h

#include "Arduino.h"
#include "00_ioInterface.h"


class servoMotor: public IO_Interface
{
    public:
    servoMotor()
	{
		this->pwmValue 					= 0;
		this->f_newPositionAvailable 	= true;
		this->ioType					= IO_TYPE__SERVO;
		this->minAngle 					= 0;
		this->maxAngle					= 180;
	}
    void begin(const uint16_t MIN = 0, const uint16_t MAX = 180)
	{
		this->minAngle = MIN;
		this->maxAngle = MAX;
	}
    void setServoPosition (const uint16_t POSITION)
	{	
		this->positionInDegree 			= POSITION;
		uint16_t 	NEW_PWM_VALUE 		= map(POSITION, 180, 0, 136, 363);			
		const bool 	PWM_VALUE_CHANGED 	= (NEW_PWM_VALUE != this->pwmValue);

		if(PWM_VALUE_CHANGED)
		{
			this->pwmValue 					= NEW_PWM_VALUE;
			this->f_newPositionAvailable 	= true;
		}
	}
	uint16_t getPosition()
	{
		return this->positionInDegree;
	}
    //Hal handling
    bool 			newDataAvailable(){return this->f_newPositionAvailable;}
    u_HAL_DATA_t 	getHalData		()
	{
		u_HAL_DATA_t DATA;
		DATA.analogIoData.value 		= this->pwmValue;
		this->f_newPositionAvailable 	= false;
		return DATA;
	}
	void    		setHalData      (u_HAL_DATA_t* P_DATA)
    {
        if(P_DATA != nullptr)
		{			
		}
    }

    private:    
	uint16_t			positionInDegree;
    uint16_t            minAngle;
    uint16_t            maxAngle;
    uint16_t            pwmValue;
    bool                f_newPositionAvailable;
};
#endif