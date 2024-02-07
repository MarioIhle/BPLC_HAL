#ifndef BPLC_IOM_h
#define BPLC_IOM_h

#include "Arduino.h"
#include "SpecialFunctions.h"
#include <Adafruit_ADS1X15.h>
#include "PCA9685.h"

//####################################################################
//ALLGEMEINE TYPEN
//####################################################################

//Encoder oder Motoren richtung
typedef enum
{
    movement_idle,
    movement_left,
    movement_right,
    movement_break,

    movement_count,
}e_movement_t;

//Portinformation
typedef union
{        
    struct         
    {
        bool state;
    }digitalInputParameter;

    struct         
    {
        uint32_t value;
    }analogInputParameter;

    struct         
    {
        uint8_t         speed;
        e_movement_t    direction;
        float           current;
    }dcDriveParameter;    

    struct         
    {
        float temperatur;
    }tempSensParameter;    
    
}u_IO_DATA_BASE_t;


typedef enum
{
    IO_TYPE__NOT_DEFINED,
    IO_TYPE__DIGITAL_INPUT,
    IO_TYPE__RPM_SENS,

    IO_TYPE__COUNT,
}e_IO_TYPE_t;



//####################################################################
//BASIS OBJEKTE
//####################################################################
class IO_Interface
{
    public:
    virtual e_IO_TYPE_t          getIoType      () = 0; 
    virtual u_IO_DATA_BASE_t     halCallback    (u_IO_DATA_BASE_t DATA) = 0;
};


//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class digitalInput: public IO_Interface
{
    public:
    //setup
                        digitalInput(){this->ioType = IO_TYPE__DIGITAL_INPUT; this->state = false; this->previousState = false;}
    //Applikation
    bool 	            ishigh      (){return (bool)(this->state == true && this->previousState == true);}
    bool	            islow       (){return (bool)(this->state == false && this->previousState == false);}
	bool 	            risingEdge  (){return (bool)(this->state == true && this->previousState == false);}	
	bool 	            fallingEdge (){return (bool)(this->state == false && this->previousState == true);}	
    //Hal handling
    e_IO_TYPE_t         getIoType   (){return this->ioType;}
    u_IO_DATA_BASE_t    halCallback (u_IO_DATA_BASE_t DATA){this->previousState = this->state; this->state = DATA.digitalInputParameter.state;}
    

    private:
    bool        state; 
    bool        previousState;
    e_IO_TYPE_t ioType;
};

//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
//--------------------------------------------------------------------
#define SPANNUNGSTEILER (5900/1200)

class AnalogInput: public IO_Interface
{
    public:
                        AnalogInput(const float MAX_VOLTAGE = 5.00);   
    //Getter für Applikation
    uint16_t            getValue            ();
    float               getValueInVolt      ();

    void                setAlarm            (const uint16_t ALARM_VALUE);
    bool                isAlarmValueReached (); //true wenn VALUE >= AlarmValue

    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    u_IO_DATA_BASE_t    halCallback         (u_IO_DATA_BASE_t DATA){this->value = DATA.analogInputParameter.value;}


    private:
    uint16_t    value;   
    uint16_t    alarmValue;
    float       maxVoltage;
    adsGain_t   adcGain;            //Verstärker einstellung des ADC(Auflösung für Spannugsberechnung)
};

//--------------------------------------------------------------------
//OUTPUT KLASSE
//Digital & Analog spielt keine Rolle
//--------------------------------------------------------------------
typedef enum
{
	OUTPUTMODE__OFF,
	OUTPUTMODE__ON, 
    OUTPUTMODE__VALUE,
	OUTPUTMODE__BLINK,
    OUTPUTMODE__BLINK_WITH_BREAK,

	OUTPUTMODE__SIZE,
}e_outputMode_t;

typedef enum
{
	OUTPUTTYPE__PULL,               //0= float, 1= GND
	OUTPUTTYPE__PUSH,               //0= float, 1=VCC    
	OUTPUTTYPE__PUSH_PULL,          //0= GND,   1=VCC  
    OUTPUTTYPE__PUSH_PULL_INVERT,   //0= VCC,   1=GND inverteriter Ausgang, benötigt für H-Brücke mit DO11

	OUTPUTTYPE__SIZE,
}e_outputType_t;


class Output: public IO_Interface
 {
	public:
	                    Output          (); 
                        Output          (const e_outputType_t OUTPUT_TYPE);
	                    Output          (const uint8_t ON_VALUE);		
                        Output          (const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE);
    
    void                begin           (const uint8_t ON_VALUE);
	void                begin           (const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE = 255);    
    //Setter
	void                blink		    (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL);		                            //Blinkt für angeforderte Anzahl und Interval
    void                blinkWithBreak  (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME); //Blinkt dauerhaft mit optinaler Pause
	void                set			    ();		//Output ON
	void                reset		    ();		//Output OFF
    void                setState        (const bool STATE);     //Über Parameter ON/OFF
    void                setValue        (const uint8_t VALUE);  //Über Parameter 0-255
    void                setOnValue      (const uint8_t VALUE);

    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    u_IO_DATA_BASE_t    halCallback     (u_IO_DATA_BASE_t DATA){this->value = DATA.analogInputParameter.value;}

	private: 
    void tick  ();		//Wird von jeweiliger Hal getickt, bevor neue Portstate abgeholt wird

    e_outputMode_t	    mode;           //Aktueller Modus
    uint8_t             value;  	    //Aktueller Wert

    struct //Hauptsächlich für verarbeitende HAL interessant
    {
        e_outputType_t  outputType;     //open drain, open source, push pull
        uint8_t         onValue;	    //Welcher Wert wird geschieben bei object.set():
    }setting;    
    
    struct 
    {
        uint8_t         blinks_requested;	//Angefragte Blinks
	    uint8_t         count;      		//Counter der blinks 	    
        Timeout         to_blink;			//Timeout für Blink Interval
        Timeout         to_break;           //Timeout für Pausen interval
    }blinkParameter;	
};


//####################################################################
//SPEZIAL OBJEKTE
//####################################################################

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class rotaryEncoder  
{
	public:
                        rotaryEncoder           ();
    void                invertTurningDirection  ();
    e_movement_t        getTurningDirection     ();
    bool                isButtonPressed         ();
    void                halCallback             (const bool STATE_A, const bool STATE_B, const bool STATE_Z);
    

    private: 

    bool            f_invertedDirection;
    digitalInput    A;
    digitalInput    B; 
    digitalInput    Z;   	   
};

//--------------------------------------------------------------------
//PLATIN TEMPERTUR SENSOR KLASSE
//--------------------------------------------------------------------
class PT10x: 
{
    public:
    PT10x();

    void begin(AnalogInput* P_PORT_1, AnalogInput* P_PORT_2,  AnalogInput* P_PORT_VCC);

    float getTemperatur   ();

    private:   
    AnalogInput* p_PORT_1;
    AnalogInput* p_PORT_2;
    AnalogInput* p_PORT_VCC;
};


//--------------------------------------------------------------------
//dcDrive KLASSE
//-------------------------------------------------------------------- 
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
    dcDrive();

    //Drive Commands
    void stop                 ();
    void start                ();
    void stopAndBreak         ();
    void setSpeed             (const uint8_t SPEED);
    void setDirection         (const e_movement_t DIRECTION);
    void setDirectionAndSpeed (const e_movement_t DIRECTION, const uint8_t SPEED);

    //Getter 
    float           getCurrent   ();
    e_movement_t    getDirection ();
    uint8_t         getSpeed     ();
    e_DRIVE_STATE_t getDriveState();

    //Für Hal
    void setCurrent(const float CURRENT);
    bool newDriveParameterAvailable();

    private:
    e_DRIVE_STATE_t driveState; 
    bool            f_thereAreNewDriveParametersAvailable;
    //Motor Parameter
    struct 
    {
      e_movement_t direction;   //Aktuell angesteuerte Drehrichtung
      uint8_t      speed;       //Aktuell angesteuerte Geschwindigkeit
      float        current;     //Aktuelle Stromaufnahme
    
      struct  //Merke Struktur um nach Stop, letzte geschriebene Bewegung fortzusetzten 
      {
        e_movement_t direction;  
        uint8_t      speed;
      }old;    
    }motParams;
};

//--------------------------------------------------------------------
//SOFTWARE H-BRÜCKE KLASSE
//-------------------------------------------------------------------- 
class Software_H_Bridge: public IO_Interface
{
    public:
    Software_H_Bridge();
};

//--------------------------------------------------------------------
//RPM Sensor
//-------------------------------------------------------------------- 
#define MAX_SAMPLES_UNTIL_CALCULATION   500

class rpmSensor: public IO_Interface
{
    public:
                rpmSensor   ();
    void        begin       (const uint16_t PULSES_PER_REV = 1, const uint16_t SAMPLE_TIME = 500);
    uint16_t    getRPM      ();

    
    //Hal handling
    e_IO_TYPE_t         getIoType   (){return this->ioType;}
    u_IO_DATA_BASE_t    halCallback (u_IO_DATA_BASE_t DATA){this->previousState = this->state; this->state = DATA.digitalInputParameter.state;}

    private:      
    digitalInput    dataObject;
    unsigned long   startTime;
    uint32_t        samples;
    uint16_t        rpm;
    uint16_t        pulsesPerRevolution;

    Timeout         to_rpmCalculation;
};


//--------------------------------------------------------------------
//Servo
//-------------------------------------------------------------------- 

class servoMotor: public IO_Interface
{
    public:
                    servoMotor          ();
    void            begin               (const uint16_t MIN = 0, const uint16_t MAX = 180);
    void            setServoPosition    (const uint16_t POSITION);
    u_IO_DATA_BASE_t   halCallback         ();
    bool            isThereANewPortValue();

    private:    
    Timeout         to_ServoPwm;
    uint16_t        minAngle;
    uint16_t        maxAngle;
    u_IO_DATA_BASE_t   pwmValue;
};
#endif