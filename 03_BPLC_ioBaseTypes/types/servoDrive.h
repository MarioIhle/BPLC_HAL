#ifndef servoDrive_h
#define servoDrive_h

#include "Arduino.h"
#include "00_ioInterface.h"


class servoMotor: public IO_Interface
{
    public:
                        servoMotor          ()
	{
		this->pwmValue 					= 0;
		this->f_newPositionAvailable 	= true;
		this->ioType					= IO_TYPE__SERVO;
		this->minAngle 					= 0;
		this->maxAngle					= 180;
	}
    void                begin               (const uint16_t MIN = 0, const uint16_t MAX = 180)
	{
		this->minAngle = MIN;
		this->maxAngle = MAX;
	}
    void                setServoPosition    (const uint16_t POSITION)
	{		
		this->pwmValue = map(POSITION, 180, 0, 136, 363);
		this->f_newPositionAvailable = true;
	}
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return this->f_newPositionAvailable;}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA = nullptr)
	{
		u_HAL_DATA_t BUFFER;
		BUFFER.analogIoData.value 		= this->pwmValue;
		this->f_newPositionAvailable 	= false;
			
		return BUFFER;
	}


    private:    
    e_IO_TYPE_t         ioType;
    uint16_t            minAngle;
    uint16_t            maxAngle;
    uint16_t            pwmValue;
    bool                f_newPositionAvailable;
};
#endif