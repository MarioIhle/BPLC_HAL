#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11   ()
{}

HAL_MOT11::HAL_MOT11   (e_MOT11_ADDRESS_t ADDRESS, Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R)
{
    this->i2c_address = ADDRESS;

    this->p_EN_L    = P_EN_L;
    this->p_EN_R    = P_EN_R;
    this->p_PWM_L   = P_PWM_L;
    this->p_PWM_R   = P_PWM_R;
}

void HAL_MOT11::begin()
{
    this->to_Heartbeat.setInterval(5000);
}

void HAL_MOT11::begin (e_MOT11_ADDRESS_t ADDRESS, Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R)
{
    this->i2c_address = ADDRESS;

    this->p_EN_L    = P_EN_L;
    this->p_EN_R    = P_EN_R;
    this->p_PWM_L   = P_PWM_L;
    this->p_PWM_R   = P_PWM_R;

    this->to_Heartbeat.setInterval(5000);
}

void HAL_MOT11::tick()
{
    //Über Heartbeat wird zyklisch Errors und Stromaufnahme abgefragt
    if(this->to_Heartbeat.check())
    {
        this->sendHeartbeat(); 
        this->to_Heartbeat.reset();
    }    
}

//Nur Stop senden, danach aber wierder letzte Parameter laden, bei Start diese senden
void HAL_MOT11::stop()
{
    this->lastSpeed      = this->actualSpeed;
    this->lastDirection  = this->actualDirection;

    this->actualSpeed       = 0;
    this->actualDirection   = idle;

    this->sendDriveCommand();
}
//Nur Temporärer Stop senden, bei start wieder mit letzten Parameter anlaufen
void HAL_MOT11::start()
{
    this->actualSpeed      = this->lastSpeed;
    this->actualDirection  = this->lastDirection;

    this->sendDriveCommand();
}

void HAL_MOT11::setSpeed(const uint8_t SPEED)
{   
    if(this->actualSpeed != SPEED)
    {
        this->actualSpeed = SPEED;    
        //Neue Geschwindigkeit an MOT11_CARD schicken
        this->sendDriveCommand();
    }    
}

void HAL_MOT11::setDirection(const e_direction_t DIRECTION)
{
    if(this->actualDirection != DIRECTION)
    {
        this->actualSpeed = DIRECTION;    
        //Neue Richtung an MOT11_CARD schicken
        this->sendDriveCommand();
    }   
}

void HAL_MOT11::setDirectionAndSpeed(const e_direction_t DIRECTION, const uint8_t SPEED)
{
     if(this->actualDirection != DIRECTION || this->actualSpeed != SPEED)
    {
        this->actualSpeed = DIRECTION;    
        this->actualSpeed = SPEED;
        //Neue Geschwindigkeit und Richtung an MOT11_CARD schicken
        this->sendDriveCommand();
    }  
}

e_motError_t HAL_MOT11::getError()
{
    return this->error;
}

float HAL_MOT11::getCurrent()
{
    return this->actualCurrent;
}

e_direction_t HAL_MOT11::getDirection()
{
    return this->actualDirection;
}

uint8_t HAL_MOT11::getSpeed()
{
    return this->actualSpeed;
}

void HAL_MOT11::sendDriveCommand()
{  
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));

    COMMAND.extract.key         = mot11_i2c_key__driveCommand;
    COMMAND.extract.direction   = this->actualDirection;
    COMMAND.extract.speed       = this->actualSpeed;

    this->sendFrame(COMMAND);         
}

void HAL_MOT11::sendHeartbeat()
{
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));
    COMMAND.extract.key = mot11_i2c_key__heartbeat;
    this->sendFrame(COMMAND);    

    //AUF antwort warten!!!!!!!!
}

void HAL_MOT11::sendFrame(const u_mot11_i2c_payload_t COMMAND)
{
    Wire.beginTransmission(this->i2c_address);   
    for(uint8_t bytes; bytes < sizeof(u_mot11_i2c_payload_t); bytes++)
    {
        Wire.write(COMMAND.data[bytes]);
    }                
    Wire.endTransmission();    
}