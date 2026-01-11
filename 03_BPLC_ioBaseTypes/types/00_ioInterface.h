#ifndef ioInterface_h
#define ioInterface_h

#include "Arduino.h"

//####################################################################
//Encoder oder Motoren richtung
typedef enum
{
    MOVEMENT__IDLE,
    MOVEMENT__LEFT,
    MOVEMENT__RIGHT,
    MOVEMENT__BREAK,

    MOVEMENT__COUNT,
}e_MOVEMENT_t;
//--------------------------------------------------------------------
//CALLBACK DATA STRUCT
typedef union
{        
    struct         
    {
        bool state;
    }digitalIoData;

    struct         
    {
        uint32_t value;
    }analogIoData;

    struct         
    {
        uint8_t         speed;
        e_MOVEMENT_t    direction;
        float           current;
    }dcDriveData;    

    struct         
    {
        float temperatur;
    }tempSensData;    

    struct         
    {
        bool stateA;
        bool stateB;
        bool stateZ;
    }encoderData;
    
    uint8_t data[12];
    
}u_HAL_DATA_t;
//--------------------------------------------------------------------
//IO TYPE
typedef enum
{
    IO_TYPE__NOT_DEFINED,

    IO_TYPE__DIGITAL_INPUT,   
    IO_TYPE__DIGITAL_COUNTER, 
    IO_TYPE__RPM_SENS,
    IO_TYPE__HMI_ENCODER,
    IO_TYPE__POSITION_ENCODER,
    IO_TYPE__ANALOG_INPUT,
    IO_TYPE__OUTPUT_PULL,               //0= float, 1= GND
	IO_TYPE__OUTPUT_PUSH,               //0= float, 1=VCC    
	IO_TYPE__OUTPUT_PUSH_PULL,          //0= GND,   1=VCC  
    IO_TYPE__OUTPUT_PUSH_PULL_INVERT,   //0= VCC,   1=GND inverteriter Ausgang, benötigt für H-Brücke mit DO11
    IO_TYPE__SERVO,
    IO_TYPE__DC_DRIVE,
    IO_TYPE__PT100,
    IO_TYPE__PT1000,
    IO_TYPE__PTC,

    IO_TYPE__COUNT,
}e_IO_TYPE_t;
//####################################################################
//IO INTERFACE
class IO_Interface
{
    public:
    e_IO_TYPE_t                 getIoType           (){return this->ioType;}
    virtual bool                newDataAvailable    () = 0;
    virtual void                setHalData          (u_HAL_DATA_t* DATA) = 0;
    virtual u_HAL_DATA_t        getHalData          () = 0;

    //Private
    e_IO_TYPE_t ioType;	  
    bool f_newDataAvailable;
};

#endif