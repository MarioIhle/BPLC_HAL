#ifndef BPLC_IOM_h
#define BPLC_IOM_h

#include "Arduino.h"
#include "SpecialFunctions.h"
#include <Adafruit_ADS1X15.h>
#include "PCA9685.h"

//#############################################
//DEBUG INFO PRINT
//####################################################################

//#define DEBUG_IOM   //Nur mit je 1 Objekt pro Klasse sinnvoll Debugbar, sonst zu viel Spam


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
typedef struct
{        
    uint16_t value;
    uint16_t previousValue;
}s_portValue_t;


//####################################################################
//BASIS OBJEKTE
//####################################################################

//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput
{
    public:
    DigitalInput();

    //Getter für Applikation
    bool 	ishigh ();
    bool	islow  ();
	bool 	risingEdge();	
	bool 	fallingEdge();	

    //Setter für HAL
    void halCallback(const bool STATE);

    private:
    s_portValue_t   inputState;   
};

//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
//--------------------------------------------------------------------
#define SPANNUNGSTEILER (5900/1200)

class AnalogInput
{
    public:
    AnalogInput(const float MAX_VOLTAGE = 5.00);   

    //Getter für Applikation
    uint16_t    getValue       ();
    float       getValueInVolt ();

    void        setAlarm            (const uint16_t ALARM_VALUE);
    bool        isAlarmValueReached (); //true wenn VALUE >= AlarmValue

    //Setter für HAL
    void halCallback    (const uint16_t     VALUE);
    void setADCGain     (const adsGain_t    ADC_GAIN);

    private:
    s_portValue_t   inputValue;   
    uint16_t        alarmValue;
    float           maxVoltage;
    adsGain_t       adcGain;            //Verstärker einstellung des ADC(Auflösung für Spannugsberechnung)
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


class Output {

	public:
	Output (); 
    Output (const e_outputType_t OUTPUT_TYPE);
	Output (const uint8_t ON_VALUE);		
    Output (const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE);
    
    void begin (const uint8_t ON_VALUE);
	void begin (const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE);
    
    //Setter
	void blink		    (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL);		                            //Blinkt für angeforderte Anzahl und Interval
    void blinkWithBreak (const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME); //Blinkt dauerhaft mit optinaler Pause
	void set			();		//Output ON
	void reset		    ();		//Output OFF
    void setState       (const bool STATE);     //Über Parameter ON/OFF
    void setValue       (const uint8_t VALUE);  //Über Parameter 0-255
    void setOnValue     (const uint8_t VALUE);

    //Für HAL
    s_portValue_t   halCallback         ();   
    e_outputType_t  getOutputType       ();
    bool            isThereANewPortValue();

	private: 
    void tick  ();		//Wird von jeweiliger Hal getickt, bevor neue Portstate abgeholt wird

    e_outputMode_t	mode;               //Aktueller Modus
    s_portValue_t   actualValue;  	    //Aktueller Wert

    struct //Hauptsächlich für verarbeitende HAL interessant
    {
        e_outputType_t  outputType;     //open drain, open source, push pull
        uint8_t         onValue;			    //Welcher Wert wird geschieben bei object.set():
    }setting;    
    
    struct 
    {
        uint8_t blinks_requested;	//Angefragte Blinks
	    uint8_t count;      		//Counter der blinks 	    
        Timeout to_blink;			//Timeout für Blink Interval
        Timeout to_break;           //Timeout für Pausen interval
    }blinkParameter;	
};


//####################################################################
//SPEZIAL OBJEKTE
//####################################################################

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder 
{
	public:
                        RotaryEncoder   ();
    e_movement_t        getTurningDirection ();
    bool                isButtonPressed     ();
    
    DigitalInput   A;
    DigitalInput   B; 
    DigitalInput   pushButton; 

  	private:    
};

//--------------------------------------------------------------------
//PLATIN TEMPERTUR SENSOR KLASSE
//--------------------------------------------------------------------
class PT10x
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
//MOTOR KLASSE
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

class MOTOR
{
    public: 
    MOTOR();

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
class Software_H_Bridge{
    public:
    Software_H_Bridge();
    Software_H_Bridge(Output* P_PWM_L, Output* P_PWM_R);

    void begin       ();
    void begin       (Output* P_PWM_L, Output* P_PWM_R);
    void setSpeed    (const uint8_t HB_SPEED);
    void setDirection(const e_movement_t DIRECTION);


    private:

    Output* p_L_PWM;
    Output* p_R_PWM;

    e_movement_t   driveDirection;
    uint8_t        driveSpeed;
};

//--------------------------------------------------------------------
//RPM Sensor
//-------------------------------------------------------------------- 
#define MAX_SAMPLES_UNTIL_CALCULATION   500

class rpmSensor
{
    public:
                rpmSensor   ();
    void        begin       (const uint16_t PULSES_PER_REV = 1, const uint16_t SAMPLE_TIME = 500);
    uint16_t    getRPM      ();
    void        tick        ();

    DigitalInput dataObject;


    private:  

    unsigned long   startTime;
    uint32_t        samples;
    uint16_t        rpm;
    uint16_t        pulsesPerRevolution;

    Timeout         to_rpmCalculation;
};


//--------------------------------------------------------------------
//Servo
//-------------------------------------------------------------------- 

class servoMotor
{
    public:
                servoMotor          ();
    void        setMinMaxAngle      (const uint16_t MIN = 0, const uint16_t MAX = 180);
    void        setServoPosition    (const uint16_t POSITION);


    Output CHANNEL;


    private:    
    PCA9685_ServoEval pwmValueCalculator;
    
    uint16_t    minAngle;
    uint16_t    maxAngle;
};
#endif