#ifndef HAL_NANO11_h
#define HAL_NANO11_h
#include "Arduino.h"


#include "BPLC_IOM.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"

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
}e_movement_t;

class HAL_MOT11
{
    public:
    HAL_MOT11();
    void begin();
    
    void stop();
    void start();
    void setSpeed(const unint8_t SPEED);
    void setDirection(const e_movement_t DIRECTION);
    void setDirectionAndSpeed(const e_movement_t DIRECTION, const unint8_t SPEED);
    
    e_motError_t    getError();
    float           getCurrent();
    e_movement_t   getDirection();
    uint8_t         getSpeed();

    private:
    void observeCurrent();
    void tickSlave();
    
    e_movement_t   actualdirection;
    uint8_t         actualSpeed;        
};

#endif