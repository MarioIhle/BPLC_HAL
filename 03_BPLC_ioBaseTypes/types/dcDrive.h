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
                        dcDrive                 (){this->initInternals();}
    //Drive Commands
    void                stop                    ()
	{
		//Bei Stopp letzte Parameter merken 
		this->motParams.old.direction = this->motParams.direction; 
		this->motParams.old.speed     = this->motParams.speed;
		this->motParams.direction     = MOVEMENT__IDLE; 
		this->motParams.speed     	  = 0;   
		f_newDriveParametersAvailable = true;
	}
    void                start                   ()
	{
		//Bei Start letzte gemerkte Parameter laden 
		this->motParams.direction     = this->motParams.old.direction; 
		this->motParams.speed     	  = this->motParams.old.speed;   
		f_newDriveParametersAvailable = true;
	}
    void                stopAndBreak            ()
	{
		//Bei Stopp letzte Parameter merken 
		this->motParams.old.direction = this->motParams.direction; 
		this->motParams.old.speed     = this->motParams.speed;
		this->motParams.direction     = MOVEMENT__IDLE; 
		this->motParams.speed     	  = 255;   
		f_newDriveParametersAvailable = true;
	}
    void                setSpeed                (const uint8_t SPEED)
	{   
		if(this->motParams.speed != SPEED)
		{
			this->motParams.speed = SPEED;
			f_newDriveParametersAvailable = true; 
		}	
	}
    void                setDirection            (const e_MOVEMENT_t DIRECTION)
	{
		if(this->motParams.direction != DIRECTION)
		{
			this->motParams.direction = DIRECTION;
			f_newDriveParametersAvailable = true; 
		}	   
	}
    void                setDirectionAndSpeed    (const e_MOVEMENT_t DIRECTION, const uint8_t SPEED)
	{
		this->motParams.direction     = DIRECTION; 
		this->motParams.speed     	  = SPEED;   
		f_newDriveParametersAvailable = true;
	}

    //Getter 
    float               getCurrent              (){return this->motParams.current;}
    e_MOVEMENT_t        getDirection            (){return this->motParams.direction;}
    uint8_t             getSpeed                (){return this->motParams.speed;}
    e_DRIVE_STATE_t     getDriveState           (){return this->driveState;}

    //Hal handling
    bool                newDataAvailable        ()
	{
		const bool NEW_DATA_AVAILABLE 		= this->f_newDriveParametersAvailable;
		this->f_newDriveParametersAvailable = false;
		return NEW_DATA_AVAILABLE;
	}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr)
	{	
		//Es können(muss aber nicht) auch daten gelesen weredn, ohne neues Kommando
		if(P_DATA != nullptr)
		{
			const bool DIRECTION_IS_DIFFERENT_ON_HARDWARE 	= this->motParams.direction != P_DATA->dcDriveData.direction;		//Vielleicht für Fehlererkennung?? sollte immmer der geschriebene Wert sein
			const bool SPEED_IS_DIFFERENT_ON_HARDWARE 		= this->motParams.speed  	!= P_DATA->dcDriveData.speed;			//Vielleicht für Fehlererkennung?? sollte immmer der geschriebene Wert sein
			this->motParams.current  						= P_DATA->dcDriveData.current;		
			
			if(DIRECTION_IS_DIFFERENT_ON_HARDWARE
			|| SPEED_IS_DIFFERENT_ON_HARDWARE)
			{
				this->f_newDriveParametersAvailable = true;
			}
		}

		//Befehle ausgeben
		u_HAL_DATA_t BUFFER;
		BUFFER.dcDriveData.direction 	= this->motParams.direction;
		BUFFER.dcDriveData.speed 		= this->motParams.speed;  
		
		return BUFFER; 
	}


    private:

    e_DRIVE_STATE_t     driveState; 
    bool                f_newDriveParametersAvailable;
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

	void initInternals()
	{
		this->ioType 						= IO_TYPE__DC_DRIVE;
		this->driveState 					= DRIVE_STATE__IDLE;
		this->f_newDriveParametersAvailable = false;
		memset(&this->motParams, 0, sizeof(this->motParams));
	}
};
#endif