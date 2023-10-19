#ifndef HAL_MOT11_h
#define HAL_MOT11_h

#include "Arduino.h"
#include "IOM_base.h"
#include "Wire.h"
#include "I2C_check.h"
#include "BPLC_TYPES.h"

//#define DEBUG_HAL_MOT11 

//I2C Commands
#define ACK 0x06
#define NAK 0x15

//State auf MCU
typedef enum
{
  driveState_stop,
  driveState_stopAndBreak,
  driveState_waitForStart,
  driveState_start,
  driveState_runningWithParameters,
  driveState_safeState,
  driveState_autoTuningRunning,

  driveState_count,

}e_driveState_t;

//State auf Mot11
typedef enum
{
  deviceState_init,
  deviceState_running,
  deviceState_safeState,
  deviceState_autotuning,

  deviceState_count,
}e_deviceState_t;

//Error out
typedef enum
{
  motError_noError,
  motError_overcurrent,
  motError_overtemperature,
  motError_notTeached, 
  motError_OEN_disabled,
  motError_i2cConnectionFailed,

  motError_count,
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
  mot11_i2c_key__getDriveState,
  mot11_i2c_key__startCurrentAutotuning,

  mot11_i2c_key__count,
}e_mot11_i2c_key_t;


#pragma pack (push, 1)
typedef union 
{   
  struct 
  {
    uint8_t key;          //1
    uint8_t deviceState;  //1+1 = 2
    uint8_t direction;    //2+1 = 3
    uint8_t speed;        //3+1 = 4
    uint8_t error;        //4+1 = 5
    float   current;      //5+4 = 9
  }extract; 

  uint8_t data[9];

}u_mot11_i2c_payload_t;
#pragma pack (pop)


class HAL_MOT11
{
    public:
    //Init
    HAL_MOT11   ();
    HAL_MOT11   (const e_MOT11_ADDRESS_t ADDRESS);
    e_BPLC_ERROR_t begin  ();
    e_BPLC_ERROR_t begin  (const e_MOT11_ADDRESS_t ADDRESS);
    
    //Routine aufruf
    void tick();

    //Drive Commands
    void stop                 ();
    void start                ();
    void stopAndBreak         ();
    void setSpeed             (const uint8_t SPEED);
    void setDirection         (const e_movement_t DIRECTION);
    void setDirectionAndSpeed (const e_movement_t DIRECTION, const uint8_t SPEED);
    void startAutotuning();
    
    //Getter 
    e_BPLC_ERROR_t  getError();
    float           getCurrent();
    e_movement_t    getDirection();
    uint8_t         getSpeed();


    private:
    //Applikation
    e_driveState_t  driveState;       
    e_deviceState_t deviceState;

    //I2C Kommunikation
    void sendDriveCommand     (e_mot11_i2c_key_t KEY);
    void requestDriveParameter();
    void sendFrame            (const u_mot11_i2c_payload_t COMMAND);
    bool waitForACK           ();
    bool waitForDriveParameter();

    e_MOT11_ADDRESS_t   deviceAddress;  
    bool                f_thereIsANewDriveCommand;

    Timeout to_parameterPoll; //ping timer
    Timeout to_I2C;           //max Wartezeit auf Antwort

    //Motor Parameter
    struct 
    {
      e_movement_t direction;   //Aktuell angesteuerte Drehrichtung
      uint8_t      speed;       //Aktuell angesteuerte Geschwindigkeit
      float        current;     //Aktuelle Stromaufnahme
    
      struct  //Merke Struktur um nach Stop, letzte geschriebene Beweung fortzusetzten 
      {
        e_movement_t direction;  
        uint8_t      speed;
      }old;    

    }motParams;
    
    //Safety
    I2C_check   selfCheck;
    struct 
    {
      struct 
      {
        uint8_t count;      //counter bis error ausgegeben wird
        uint8_t countLimit; //Limit ab wann error ausgegeben wird
      }i2cError;    
      
      e_motError_t  code;       //aktueller Erororcode
    }error;
};
#endif