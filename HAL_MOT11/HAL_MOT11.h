#ifndef HAL_MOT11_h
#define HAL_MOT11_h

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "IOM_base.h"
#include "Wire.h"
#include "I2C_check.h"
#include "BPLC_TYPES.h"
#include "BPLC_ERRORS.h"
//-------------------------------------------------------------
//HARDWARE DEBUGGING
//-------------------------------------------------------------
//#define DEBUG_HAL_MOT11 

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
  MOT11_CARD__1,
  MOT11_CARD__2,
  MOT11_CARD__3,
  MOT11_CARD__4,
  
  MOT11_CARD__MAX,
}e_MOT11_CARD_t;

typedef enum
{
  MOT11_CARD_1_ADDRESS = 0x10,
  MOT11_CARD_2_ADDRESS = 0x11,
  MOT11_CARD_3_ADDRESS = 0x12,
  MOT11_CARD_4_ADDRESS = 0x13,
  
  MOT11_CARD_ADDRESS_COUNT = 4,
}e_MOT11_ADDRESS_t;

typedef enum
{
  MOT11_PORT__1,

  MOT11_PORT__COUNT,
}e_MOT11_PORTS_t;

//-------------------------------------------------------------
//APPLIKATION
//-------------------------------------------------------------
#define ACK 0x06
#define NAK 0x15

//State auf MOT11
typedef enum
{
  deviceState_init,
  deviceState_running,
  deviceState_safeState,
  deviceState_autotuning,

  deviceState_count,
}e_deviceState_t;

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

//-------------------------------------------------------------
//HAL_DIN11 KLASSE
//-------------------------------------------------------------
class HAL_MOT11
{
    public:
    //Init
    HAL_MOT11   ();
    HAL_MOT11   (const e_MOT11_ADDRESS_t ADDRESS);
    void begin  ();
    void begin  (const e_MOT11_ADDRESS_t ADDRESS);
    
    //Applikation
    void            tick();
    void            mapObjectToPort(MOTOR* P_OBJECT);
    e_BPLC_ERROR_t  getError();
  
    private:
    //Applikation      
    e_deviceState_t deviceState;

    //I2C Kommunikation
    void sendDriveCommand     (const e_movement_t DIRECTION, const uint8_t SPEED);
    void requestDriveParameter();
    void sendFrame            (const u_mot11_i2c_payload_t COMMAND);
    bool waitForACK           ();
    bool waitForDriveParameter();

    Timeout to_parameterPoll; //ping timer
    Timeout to_I2C;           //max Wartezeit auf Antwort
   
    //Safety
    I2C_check   selfCheck;
    struct 
    {
      struct 
      {
        uint8_t count;      //counter bis error ausgegeben wird
        uint8_t countLimit; //Limit ab wann error ausgegeben wird
      }i2cError;    
      
      e_BPLC_ERROR_t code;       //aktueller Erororcode
    }error;

    //Settings
    MOTOR*              P_MOTOR;   //zeiger auf ausgegebenes Objekt
    uint8_t             usedPorts;
    e_MOT11_ADDRESS_t   deviceAddress;
};
#endif