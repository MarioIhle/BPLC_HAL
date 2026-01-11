#ifndef hmiEncoder_h
#define hmiEncoder_h

#include "Arduino.h"
#include "00_ioInterface.h"
#include "digitalInput.h"

#define ENCODER_CHANNEL_A 0
#define ENCODER_CHANNEL_B 1

typedef enum
{
    HMI_ENCODER_CYCLE__IDLE,
    HMI_ENCODER_CYCLE__FIRST_EDGE,
    HMI_ENCODER_CYCLE__ACTIVE_STATE,
    HMI_ENCODER_CYCLE__SECOND_EDGE,

}e_HMI_ENCODER_CYCLE_t;

class hmiEncoder:public IO_Interface
{
	public:
                        hmiEncoder              ()
    {
        this->f_invertedDirection   = false;
        this->f_encoderLogicLevel   = false;
        this->ioType                = IO_TYPE__HMI_ENCODER; 
        this->inputChannels[ENCODER_CHANNEL_A].setDebounceTime(10,10);
        this->inputChannels[ENCODER_CHANNEL_B].setDebounceTime(10,10);
        this->pushButton.setDebounceTime(10,10);
        this->channelState[ENCODER_CHANNEL_A] = HMI_ENCODER_CYCLE__IDLE;
        this->channelState[ENCODER_CHANNEL_B] = HMI_ENCODER_CYCLE__IDLE;
    }
    void                setLogicLevel           (const bool ENCODER_IS_HIGH_ACTIVE){this->f_encoderLogicLevel = ENCODER_IS_HIGH_ACTIVE;}
    void                invertTurningDirection  (){this->f_invertedDirection = !this->f_invertedDirection;}
    e_MOVEMENT_t        getTurningDirection     (){e_MOVEMENT_t DIRECTION = this->direction; this->direction = MOVEMENT__IDLE; return DIRECTION;}
    bool                buttonPressed           (){return this->pushButton.fallingEdge();}
    bool                buttonReleased          (){return this->pushButton.risingEdge();}
    //Hal handling
    bool            newDataAvailable        (){return false;}
    void            setHalData              (u_HAL_DATA_t* P_DATA)
    {              
        if(P_DATA != nullptr)
        {
            u_HAL_DATA_t EXTRACTED_HAL_DATA__A;
            EXTRACTED_HAL_DATA__A.digitalIoData.state = P_DATA->encoderData.stateA;
            u_HAL_DATA_t EXTRACTED_HAL_DATA__B ;
            EXTRACTED_HAL_DATA__B.digitalIoData.state = P_DATA->encoderData.stateB;
            u_HAL_DATA_t EXTRACTED_HAL_DATA__Z;
            EXTRACTED_HAL_DATA__Z.digitalIoData.state = P_DATA->encoderData.stateZ;

            this->inputChannels[ENCODER_CHANNEL_A].setHalData(&EXTRACTED_HAL_DATA__A);   
            this->inputChannels[ENCODER_CHANNEL_B].setHalData(&EXTRACTED_HAL_DATA__B);   
            this->pushButton.setHalData(&EXTRACTED_HAL_DATA__Z);  

            const bool ENCODER_IS_HIGH_ACTIVE = this->f_encoderLogicLevel;
            
            for(uint8_t i = 0; i<2; i++)
            {
                switch (this->channelState[i])
                {
                    default:
                    case HMI_ENCODER_CYCLE__IDLE:
                        if(ENCODER_IS_HIGH_ACTIVE)
                        {
                            if(this->inputChannels[i].risingEdge())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__FIRST_EDGE;
                            }
                        }
                        else
                        {
                            if(this->inputChannels[i].fallingEdge())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__FIRST_EDGE;
                            }
                        }                    
                    break;

                    case  HMI_ENCODER_CYCLE__FIRST_EDGE:
                    if(ENCODER_IS_HIGH_ACTIVE)
                        {
                            if(this->inputChannels[i].ishigh())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__ACTIVE_STATE;
                            }
                        }
                        else
                        {
                            if(this->inputChannels[i].islow())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__ACTIVE_STATE;
                            }
                        }
                    break;

                    case HMI_ENCODER_CYCLE__ACTIVE_STATE:
                        if(ENCODER_IS_HIGH_ACTIVE)
                        {
                            if(this->inputChannels[i].fallingEdge())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__SECOND_EDGE;
                            }
                        }
                        else
                        {
                            if(this->inputChannels[i].risingEdge())
                            {
                                this->channelState[i] = HMI_ENCODER_CYCLE__SECOND_EDGE;
                            }
                        }
                    break;
                    
                    //Auswertung abgeschlossen beide Channel zurück setzen
                    case HMI_ENCODER_CYCLE__SECOND_EDGE:                    
                        this->channelState[0] = HMI_ENCODER_CYCLE__IDLE;
                        this->channelState[1] = HMI_ENCODER_CYCLE__IDLE;
                    break;
                }            
            }                

            //Richutngsauswertung sobald eine der beiden Spuren einen Zyklus abgeschlossen hat
            if(this->channelState[0] == HMI_ENCODER_CYCLE__SECOND_EDGE)
            {
                if(this->f_invertedDirection)
                {
                    this->direction = MOVEMENT__LEFT;
                }
                else
                {
                    this->direction = MOVEMENT__RIGHT;
                } 
            }
            else if(this->channelState[1] == HMI_ENCODER_CYCLE__SECOND_EDGE)
            {   
                if(this->f_invertedDirection)
                {
                    this->direction = MOVEMENT__RIGHT;
                }
                else
                {
                    this->direction = MOVEMENT__LEFT;
                } 
            }
            else
            {
                this->direction = MOVEMENT__IDLE;
            }       

            /*
            //Debug output
                Serial.print("ENCODER_DIRECTION: ");

                if(this->direction = MOVEMENT__LEFT)
                {                 
                    Serial.println("LEFT");
                }
                if(this->direction = MOVEMENT__RIGHT)
                {                 
                    Serial.println("RIGHT");
                }     
            */
        }    
    }
    u_HAL_DATA_t 	getHalData			    ()
	{	
		u_HAL_DATA_t DATA;
        memset(&DATA, 0, sizeof(u_HAL_DATA_t));
		
		return DATA; 
	}


    private: 
    bool                f_invertedDirection;
    bool                f_encoderLogicLevel;
    //Inputs
    digitalInput        inputChannels[2];
    digitalInput        pushButton;
    //richtungsauswertung 
    e_HMI_ENCODER_CYCLE_t channelState[2];
    e_MOVEMENT_t        direction;
};

#endif