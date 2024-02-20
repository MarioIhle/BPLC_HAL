#ifndef io_API_h
#define io_API_h

#include "Arduino.h"

#include "SpecialFunctions.h"
#include <Adafruit_ADS1X15.h>
#include "PCA9685.h"


//--------------------------------------------------------------------
//COUNTER KLASSE
class counter: public IO_Interface
{
    public:
    //init
                        counter         (){this->ioType = IO_TYPE__DIGITAL_COUNTER; this->count = 0;}
    //Applikation
    uint64_t 	        getCount        (){return this->count;}
    void                setCount        (uint64_t COUNT){this->count = COUNT;}
    void	            resetCount      (){this->count = 0;}
	void 	            increment       (){this->count++;}	
    void 	            decrement       (){this->count--;}
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable(){return false;}
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA){if(P_DATA->digitalIoData.state == true && this->previousState == false){this->count++;} this->previousState = P_DATA->digitalIoData.state; return *P_DATA;}
    

    private:
    volatile uint64_t   count; 
    e_IO_TYPE_t         ioType;
    volatile bool       previousState;
};
//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
#define SPANNUNGSTEILER (5900/1200)

class AnalogInput: public IO_Interface
{
    public:
                        AnalogInput         (const float MAX_VOLTAGE = 5.00);   
    //Getter für Applikation
    uint16_t            getValue            (){return this->value;}
    float               getValueInVolt      ();
    void                setAdcGain          (const adsGain_t GAIN){this->adcGain = GAIN;}

    void                setAlarm            (const uint16_t ALARM_VALUE){this->alarmValue = ALARM_VALUE;}
    bool                isAlarmValueReached (){return (bool)(this->value >= this->alarmValue);}
    void                setSampleTime       (const uint64_t SAMPLE_TIME){this->to_sampleTime.setInterval(SAMPLE_TIME);}
    
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return (bool)(this->to_sampleTime.checkAndReset());}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA){this->value = P_DATA->analogIoData.value; return *P_DATA;}


    private:
    e_IO_TYPE_t         ioType;
    uint16_t            value;   
    uint16_t            alarmValue;
    float               maxVoltage;
    Timeout             to_sampleTime;
    adsGain_t           adcGain;            //Verstärker einstellung des ADC(Auflösung für Spannugsberechnung)
};
//--------------------------------------------------------------------
//OUTPUT KLASSE
typedef enum
{
	OUTPUTMODE__OFF,
	OUTPUTMODE__ON, 
    OUTPUTMODE__VALUE,
	OUTPUTMODE__BLINK_ONCE,
    OUTPUTMODE__BLINK_CONTINIOUS,

	OUTPUTMODE__SIZE,
}e_outputMode_t;

class Output: public IO_Interface, blink
 {
	public:
    //Setup
                        Output          (const e_IO_TYPE_t OUTPUT_TYPE = IO_TYPE__OUTPUT_PUSH, const uint8_t ON_VALUE = 255);    
    void                setOnValue      (const uint8_t VALUE);
    //Applikation Handling
	void                blinkOnce		(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL);		                        //Blinkt für angeforderte Anzahl und Interval
    void                blinkContinious (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME); //Blinkt dauerhaft mit optinaler Pause
	void                set			    ();		                //Output ON
	void                reset		    ();		                //Output OFF
    void                setState        (const bool STATE);     //ON/OFF
    void                setValue        (const uint8_t VALUE);  //0-255
    //Hal handling
    e_IO_TYPE_t         getIoType       (){return this->ioType;}
    bool                newDataAvailable();
    u_HAL_DATA_t        halCallback     (u_HAL_DATA_t* P_DATA);


	private: 
    e_IO_TYPE_t         ioType;
    e_outputMode_t	    mode;           //Aktueller Modus
    uint8_t             value;  	    //Aktueller Wert
    bool                f_newDataAvailable;

    struct //Hauptsächlich für verarbeitende HAL interessant
    {
        uint8_t         onValue;	    //Welcher Wert wird geschieben bei object.set():
    }setting;    
};
//####################################################################
//SPEZIAL OBJEKTE
//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
class rotaryEncoder:public IO_Interface
{
	public:
                        rotaryEncoder           ();
    void                invertTurningDirection  ();
    e_MOVEMENT_t        getTurningDirection     ();
    bool                isButtonPressed         ();

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return false;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr);
    

    private: 
    e_IO_TYPE_t         ioType;
    bool                f_invertedDirection;
    digitalInput        A;
    digitalInput        B; 
    digitalInput        PB;   	   
};
//--------------------------------------------------------------------
//POSITION ENCODER KLASSE
class positionEncoder:public IO_Interface
{
	public:
                        positionEncoder                 (const uint16_t PULSES_PER_REV){this->pulsesPerRevolution = PULSES_PER_REV; this->ioType = IO_TYPE__POSITION_ENCODER;}
    uint16_t            getPositionInDegree             (){return map(this->poitionIncrement.getCount(), 0 , this->pulsesPerRevolution, 0, 360);}
    uint16_t            getPositionIncrement            (){return this->poitionIncrement.getCount();}

    //Hal handling
    e_IO_TYPE_t         getIoType                       (){return this->ioType;}
    bool                newDataAvailable                (){return false;}
    u_HAL_DATA_t        halCallback                     (u_HAL_DATA_t* P_DATA = nullptr);
    

    private: 
    e_IO_TYPE_t         ioType;
    counter             poitionIncrement;
    uint16_t            pulsesPerRevolution;
    digitalInput        A;
    digitalInput        B; 
    digitalInput        Z;   	   
};
//--------------------------------------------------------------------
//DC DRIVE KLASSE
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
    void                stop                    ();
    void                start                   ();
    void                stopAndBreak            ();
    void                setSpeed                (const uint8_t SPEED);
    void                setDirection            (const e_MOVEMENT_t DIRECTION);
    void                setDirectionAndSpeed    (const e_MOVEMENT_t DIRECTION, const uint8_t SPEED);

    //Getter 
    float               getCurrent              ();
    e_MOVEMENT_t        getDirection            ();
    uint8_t             getSpeed                ();
    e_DRIVE_STATE_t     getDriveState           ();

    //Hal handling
    e_IO_TYPE_t         getIoType               (){return this->ioType;}
    bool                newDataAvailable        (){return this->f_thereAreNewDriveParametersAvailable;}
    u_HAL_DATA_t        halCallback             (u_HAL_DATA_t* P_DATA = nullptr);

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
//--------------------------------------------------------------------
//RPM Sensor
#define MAX_SAMPLES_UNTIL_CALCULATION   500

class rpmSensor: public IO_Interface
{
    public:
                        rpmSensor           ();
    void                begin               (const uint16_t PULSES_PER_REV = 1, const uint16_t SAMPLE_TIME = 500);
    uint16_t            getRPM              ();
    
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return false;}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA = nullptr);


    private:     
    e_IO_TYPE_t         ioType;
    counter             sampleCounter;
    unsigned long       startTime;

    uint16_t            rpm;
    uint16_t            pulsesPerRevolution;

    Timeout             to_rpmCalculation;
};
//--------------------------------------------------------------------
//Servo
class servoMotor: public IO_Interface
{
    public:
                        servoMotor          ();
    void                begin               (const uint16_t MIN = 0, const uint16_t MAX = 180);
    void                setServoPosition    (const uint16_t POSITION);
    //Hal handling
    e_IO_TYPE_t         getIoType           (){return this->ioType;}
    bool                newDataAvailable    (){return this->f_newPositionAvailable;}
    u_HAL_DATA_t        halCallback         (u_HAL_DATA_t* P_DATA = nullptr);


    private:    
    e_IO_TYPE_t         ioType;
    uint16_t            minAngle;
    uint16_t            maxAngle;
    uint16_t            pwmValue;
    bool                f_newPositionAvailable;
};

#endif