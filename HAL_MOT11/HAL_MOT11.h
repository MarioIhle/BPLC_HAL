#ifndef HAL_MOT11_h
#define HAL_MOT11_h
#include "Arduino.h"


//Error out
typedef enum
{
    motError_noError,
    motError_overcurrent,
    motError_notTeached, 
    motError_size,
}e_motError_t;

typedef enum 
{
    direction_idle,
    direction_left,
    direction_rigth,
}e_direction_t;

class HAL_MOT11
{
    public:
    HAL_MOT11();
    void begin();
    
    void stop();
    void start();
    void setSpeed(const unint8_t SPEED);
    void setDirection(const e_direction_t DIRECTION);
    void setDirectionAndSpeed(const e_direction_t DIRECTION, const unint8_t SPEED);
    
    e_motError_t    getError();
    float           getCurrent();
    e_direction_t   getDirection();
    uint8_t         getSpeed();

    private:
    void observeCurrent();
    void tickSlave();
    
    e_direction_t   actualdirection;
    uint8_t         actualSpeed;        
};

#endif