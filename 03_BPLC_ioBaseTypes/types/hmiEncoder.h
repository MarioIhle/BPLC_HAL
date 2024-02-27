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
    void                invertTurningDirection  (){this->f_invertedDirection = true;}
    e_MOVEMENT_t        getTurningDirection     ()
    {     
        e_MOVEMENT_t direction = MOVEMENT__IDLE;  
        //"rechts"  
        if(     this->stateA != this->previoursStateA
            ||  this->stateB != this->previoursStateB)
        {
            if(     this->stateA == false && this->previoursStateA == true 
                &&  this->stateB == false)
            {
                if(this->f_invertedDirection == false)
                {
                    direction = MOVEMENT__RIGHT;
                }
                else
                {
                    direction = MOVEMENT__LEFT;
                }       
            }//"links"
            else if(this->stateA == false && 
                    this->stateB == false && this->previoursStateB == true)
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
            previoursStateA = stateA;
            previoursStateB = stateB;            
        }
        return direction;
    }
    bool                isButtonPressed         ()    
    {
        const bool BUTTON_WAS_PRESSED   = (bool)(statePushButton == false && this->previoursStatePushButton == true); 
        this->previoursStatePushButton  = statePushButton;
        return BUTTON_WAS_PRESSED;
    }

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return false;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
    {      
        stateA          = P_DATA->encoderData.stateA;
        stateB          = P_DATA->encoderData.stateB;
        statePushButton = P_DATA->encoderData.stateZ;
        
        return *P_DATA;
    }
    

    private: 

    e_IO_TYPE_t         ioType;
    bool                f_invertedDirection;
    bool                stateA;
    bool                stateB;
    bool                statePushButton;
    bool                previoursStateA;
    bool                previoursStateB;
    bool                previoursStatePushButton;
};

#endif