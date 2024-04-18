#ifndef hmiEncoder_h
#define hmiEncoder_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "digitalInput.h"

class hmiEncoder:public IO_Interface
{
	public:
                        hmiEncoder              ()
    {
        this->f_invertedDirection   = false;
        this->ioType                = IO_TYPE__ROTARY_ENCODER; 
    }

    void                invertTurningDirection  (){this->f_invertedDirection = !this->f_invertedDirection;}

    e_MOVEMENT_t        getTurningDirection     ()
    {     
        e_MOVEMENT_t direction = MOVEMENT__IDLE;  

        if(this->A.fallingEdge() && this->B.ishigh())
        {
            if(this->f_invertedDirection == false)
            {
                direction = MOVEMENT__RIGHT;
            }
            else
            {
                direction = MOVEMENT__LEFT;
            }       
        }
        else if(this->B.fallingEdge() && this->A.ishigh())
        {
            if(this->f_invertedDirection == false)
            {
                direction = MOVEMENT__LEFT;
            }
            else
            {
                direction = MOVEMENT__RIGHT;
            }              
        }        
        
        return direction;
    }

    bool isButtonPressed(){return this->PB.fallingEdge();}

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return false;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
    {              
        u_HAL_DATA_t EXTRACTED_HAL_DATA__A;
        EXTRACTED_HAL_DATA__A.digitalIoData.state = P_DATA->encoderData.stateA;
        u_HAL_DATA_t EXTRACTED_HAL_DATA__B ;
        EXTRACTED_HAL_DATA__B.digitalIoData.state = P_DATA->encoderData.stateB;
        u_HAL_DATA_t EXTRACTED_HAL_DATA__Z;
        EXTRACTED_HAL_DATA__Z.digitalIoData.state = P_DATA->encoderData.stateZ;

        this->A.halCallback(&EXTRACTED_HAL_DATA__A);   
        this->B.halCallback(&EXTRACTED_HAL_DATA__B);   
        this->PB.halCallback(&EXTRACTED_HAL_DATA__Z);     
        return *P_DATA;
    }
    

    private: 

    e_IO_TYPE_t         ioType;
    bool                f_invertedDirection;
    digitalInput        A;
    digitalInput        B;
    digitalInput        PB;
};

#endif