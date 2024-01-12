#ifndef BPLC_IOM_h
#define BPLC_IOM_h

#include "Arduino.h"
#include "SpecialFunctions.h"

//####################################################################
//DEBUG INFO PRINT
//####################################################################

//#define DEBUG_IOM   //Nur mit je 1 Objekt pro Klasse sinnvoll Debugbar, sonst zu viel Spam


//####################################################################
//ALLGEMEINE TYPEN
//####################################################################
#define uint8_t byte

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
	bool 	posEdge();	
	bool 	negEdge();	

    //Setter für HAL
    void setPortState(const bool STATE);

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
    AnalogInput();
    void        setMaxVoltage(const float VOLTAGE);

    //Getter für Applikation
    uint16_t    getValue       ();
    float       getValueInVolt ();

    void        setAlarm            (const uint16_t ALARM_VALUE);
    bool        isAlarmValueReached (); //true wenn VALUE >= AlarmValue

    //Setter für HAL
    void setPortValue       (const uint16_t VALUE);
    void setRawPortVoltage (const float PORT_VOLTAGE);

    private:
    s_portValue_t   inputValue;   
    float           rawPortVoltage;
    uint16_t        alarmValue;
    float           maxVoltage = 5.00;  //default
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
    void setValue       (const uint8_t VALUE);
    void setOnValue     (const uint8_t VALUE);

    //Für HAL
    s_portValue_t   getValue            ();   
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
                        RotaryEncoder   (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);   
    void                begin           (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);

    e_movement_t        getTurningDirection ();
    bool                isButtonPressed     ();
    
  	private:
    DigitalInput*   p_A;
    DigitalInput*   p_B; 
    DigitalInput*   p_pushButton; 
};

//--------------------------------------------------------------------
//PLATIN TEMPERTUR SENSOR KLASSE
//--------------------------------------------------------------------
typedef enum
{
    PT1000__HALF_BRIDGE,
    PT1000__FULL_BRIDGE,

}e_SENSOR_CONFIG_t;


class PT10x
{
    public:
    PT10x();

    void begin(AnalogInput* P_PORT, const float VOLATGE_AT_0_DEG, const float VOLTAGE_AT_100_DEG);
    void begin(AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, const float VOLATGE_AT_0_DEG, const float VOLTAGE_AT_100_DEG);

    int  getTemperatur   ();

    private:
    struct 
    {
        float atZero;
        float atOneHundred;
    }voltage;
    
    e_SENSOR_CONFIG_t  sensorCofig;
    int16_t oldTemp; 
    AnalogInput* p_PORT_1;
    AnalogInput* p_PORT_2;
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
#endif