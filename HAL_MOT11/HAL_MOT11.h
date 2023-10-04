#ifndef HAL_MOT11_h
#define HAL_MOT11_h
#include "Arduino.h"
#include "IOM_base.h"

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
    MOT11_CARD_1 = 0x10,
    MOT11_CARD_2 = 0x11,
    MOT11_CARD_3 = 0x12,
    MOT11_CARD_4 = 0x13,
    
    MOT11_CARD_COUNT = 4,

}e_MOT11_ADDRESS_t;

typedef enum
{
  //setter 
  mot11_i2c_key__driveCommand,
  mot11_i2c_key__heartbeat,

  mot11_i2c_key__count,
}e_mot11_i2c_key_t;


typedef union 
{ 
    struct 
    {
      e_mot11_i2c_key_t key;
      e_direction_t     direction;
      uint8_t           speed;
      e_motError_t      error;
    }extract; 

    uint8_t data[sizeof(extract)];

}u_mot11_i2c_payload_t;


class HAL_MOT11
{
    public:
    HAL_MOT11   ();
    HAL_MOT11   (e_MOT11_ADDRESS_t ADDRESS, Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R);
    void begin  ();
    void begin  (e_MOT11_ADDRESS_t ADDRESS, Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R);
    
    void tick();

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
    void sendDriveCommand   (const u_mot11_i2c_payload_t COMMAND);
    void sendHeartbeat      ();
    void sendFrame          ();
  
    //Motor Parameter
    e_direction_t       actualDirection;
    uint8_t             actualSpeed;      
    e_direction_t       lastDirection;
    uint8_t             lastSpeed;   
    float               actualCurrent;
    e_motError_t        error;

    //MOT11_CARD Parameter
    e_MOT11_ADDRESS_t   i2c_address;  

    //Output Pointer
    Output* p_EN_L;
    Output* p_EN_R;
    Output* p_PWM_L;
    Output* p_PWM_R;

    Timeout to_Heartbeat;
};

#endif