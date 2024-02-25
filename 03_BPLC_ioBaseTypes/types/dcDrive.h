#ifndef dcDrive_h
#define dcDrive_h

#include "Arduino.h"
#include "00_ioInterface.h"


typedef enum
{
    DRIVE_STATE__IDLE,
    DRIVE_STATE__START,
    DRIVE_STATE__STOP,
    DRIVE_STATE__STOP_AND_BREAK,  
    DRIVE_STATE__RUNNING,

    DRIVE_STATE__COUNT,
}e_DRIVE_STATE_t;

class dcDrive: public IO_Interface
{
    public: 
                        dcDrive                 ();
    //Drive Commands
    void                stop                    ()
	{
		//Bei Stopp letzte Parameter merken 
		this->motParams.old.direction = this->motParams.direction; 
		this->motParams.old.speed     = this->motParams.speed;
		this->motParams.direction     = MOVEMENT__IDLE; 
		this->motParams.speed     	  = 0;   
		f_thereAreNewDriveParametersAvailable = true;
	}
    void                start                   ()
	{
		//Bei Start letzte gemerkte Parameter laden 
		this->motParams.direction     = this->motParams.old.direction; 
		this->motParams.speed     	  = this->motParams.old.speed;   
		f_thereAreNewDriveParametersAvailable = true;
	}
    void                stopAndBreak            ()
	{
		//Bei Stopp letzte Parameter merken 
		this->motParams.old.direction = this->motParams.direction; 
		this->motParams.old.speed     = this->motParams.speed;
		this->motParams.direction     = MOVEMENT__IDLE; 
		this->motParams.speed     	  = 255;   
		f_thereAreNewDriveParametersAvailable = true;
	}
    void                setSpeed                (const uint8_t SPEED)
	{   
		if(this->motParams.speed != SPEED)
		{
			this->motParams.speed = SPEED;
			f_thereAreNewDriveParametersAvailable = true; 
		}	
	}
    void                setDirection            (const e_MOVEMENT_t DIRECTION)
	{
		if(this->motParams.direction != DIRECTION)
		{
			this->motParams.direction = DIRECTION;
			f_thereAreNewDriveParametersAvailable = true; 
		}	   
	}
    void                setDirectionAndSpeed    (const e_MOVEMENT_t DIRECTION, const uint8_t SPEED)
	{
		this->motParams.direction     = DIRECTION; 
		this->motParams.speed     	  = SPEED;   
		f_thereAreNewDriveParametersAvailable = true;
	}

    //Getter 
    float               getCurrent              (){return this->motParams.current;}
    e_MOVEMENT_t        getDirection            (){return this->motParams.direction;}
    uint8_t             getSpeed                (){return this->motParams.speed;}
    e_DRIVE_STATE_t     getDriveState           (){return this->driveState;}

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return this->f_thereAreNewDriveParametersAvailable;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
	{	 
		this->motParams.current  		= P_DATA->dcDriveData.current;
		u_HAL_DATA_t BUFFER;
		BUFFER.dcDriveData.direction 	= this->motParams.direction;
		BUFFER.dcDriveData.speed 		= this->motParams.speed;     
		this->f_thereAreNewDriveParametersAvailable = false;
		return BUFFER; 
	}


    private:
    e_DRIVE_STATE_t     driveState; 
    bool                f_thereAreNewDriveParametersAvailable;
    e_IO_TYPE_t         ioType;
    //Motor Parameter
    struct 
    {
      e_MOVEMENT_t      direction;   //Aktuell angesteuerte Drehrichtung
      uint8_t           speed;       //Aktuell angesteuerte Geschwindigkeit
      float             current;     //Aktuelle Stromaufnahme
    
      struct  //Merke Struktur um nach Stop, letzte geschriebene Bewegung fortzusetzten 
      {
        e_MOVEMENT_t    direction;  
        uint8_t         speed;
      }old;    
    }motParams;
};
#endif