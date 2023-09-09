#ifndef IOM_base_h
#define IOM_base_h

#include "Arduino.h"
#include "SpecialFunctions.h"

#define DEBUG_IOM   //Nur mit je 1 Objekt pro Klasse sinnvoll Debugbar, sonst zu viel Spam
//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
//Encoder oder Motoren richtung
typedef enum
{
    idle,
    left,
    right,
}e_direction_t;

//Port
typedef struct
{        
    int value;
    int previousValue;
}s_portValue_t;

//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput
{
    public:
    DigitalInput    ();
   
    //Getter für Applikation
    bool 	ishigh		();
    bool	islow		();
	bool 	posFlank	();	
	bool 	negFlank	();	

    //Setter für HAL
    void setPortState(const bool STATE);

    private:
    s_portValue_t   inputState;   
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder 
{
	public:
                        RotaryEncoder   ();
                        RotaryEncoder   (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);   
    void                begin           (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);

    e_direction_t       getTurningDirection ();
    bool                isButtonPressed     ();
    
  	private:
    DigitalInput*   p_A;
    DigitalInput*   p_B; 
    DigitalInput*   p_pushButton; 
};

//--------------------------------------------------------------------
//OUTPUT KLASSE
//Digital & Analog spielt keine Rolle
//--------------------------------------------------------------------
typedef enum
{
	OUTPUTMODE__OFF,
	OUTPUTMODE__ON, 
	OUTPUTMODE__BLINK,
    OUTPUTMODE__BLINK_WITH_BREAK,

	OUTPUTMODE__SIZE,
}e_outputMode_t;

typedef enum
{
	OUTPUTTYPE__OPEN_DRAIN,     //0= float, 1= GND
	OUTPUTTYPE__OPEN_SOURCE,    //0= float, 1=VCC    
	OUTPUTTYPE__PUSH_PULL,      //0= GND,   1=VCC  

	OUTPUTTYPE__SIZE,
}e_outputType_t;


class Output {

	public:
    //------------------------------------------------------------------------
    //Constructor Initialisierung
	Output				(); 
    Output              (const e_outputType_t OUTPUT_TYPE);
	Output				(const uint8_t ON_VALUE);		
    Output              (const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE);
    
    void 	begin		(const uint8_t ON_VALUE);
	void 	begin		(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE);
    void  	tick		();		//Muss getickt werden jeden loop

	void	blink		    (const uint8_t BLINKS, const int BLINK_INTERVAL);		                //Blinkt für angeforderte Anzahl und Interval
    void    blinkWithBreak  (const uint8_t BLINKS, const int BLINK_INTERVAL, const int BREAK_TIME); //Blinkt dauerhaft mit optinaler Pause
	void 	set			    ();		//Output ON
	void 	reset		    ();		//Output OFF
    void    setOnValue      (const uint8_t VALUE);

    //Für HAL
    s_portValue_t   getValue        ();   
    e_outputType_t  getOutputType   ();

	private:
    void            setOutputValue(const uint8_t VALUE);

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









/*

//--------------------------------------------------------------------
//DIGITALSETTER KLASSE
//--------------------------------------------------------------------
class DigitalSetter
{
    public:
    DigitalSetter   (const uint8_t PIN);
    void    begin   (const uint8_t PIN);
    void 	setState(const bool STATE);
    void    set     ();
    void    reset   ();
    bool    get     ();

    private:
    uint8_t     port;
};

//--------------------------------------------------------------------
//ANALOGSETTER KLASSE
//--------------------------------------------------------------------
class AnalogSetter
{
    public:
    AnalogSetter        ();

    void        begin   ();
    void 	    setOnValue(const uint8_t VALUE);
    void        setMax  ();
    void        setMin  ();
    uint16_t    get     ();
    private:

    uint8_t   port;
};

//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
//--------------------------------------------------------------------
class AnalogInput {
	public:
    AnalogInput();

    void        begin(); 
    uint16_t    read();    
  	private:
     
	e_analogInputPorts_t    port;
};

//--------------------------------------------------------------------
//TEMPERATUR SENSOR KLASSE
//--------------------------------------------------------------------
class TemperaturSensor{
    public:
    TemperaturSensor();

    void    begin           ();
    int16_t getTemperatur   ();

    private:
    int16_t oldTemp; 
};



//--------------------------------------------------------------------
//DIGITAL OUTPUT KLASSE
//--------------------------------------------------------------------
class DigitalOutput{
    public:
    DigitalOutput();
     void begin   ();
    DigitalSetter state; 
};

//--------------------------------------------------------------------
//RELAIS KLASSE
//--------------------------------------------------------------------
class Relais{
    public:
    Relais();
    void            begin   ();     
    DigitalSetter   state;
};

//--------------------------------------------------------------------
//ANALOG OUTPUT KLASSE
//--------------------------------------------------------------------
class AnalogOutput{
    public:
    AnalogOutput();
    void begin  ();
    AnalogSetter value;
};

//--------------------------------------------------------------------
//H-BRÜCKE KLASSE
//-------------------------------------------------------------------- 
class H_Bridge{
    public:
    H_Bridge();
    void    begin       ();
    void    setSpeed    (const uint8_t HB_SPEED);
    void    setDirection(const e_direction_t DIRECTION);

    private:
  
	
};
*/
#endif