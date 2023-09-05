#ifndef IOM_base_h
#define IOM_base_h

#include "Arduino.h"
#include "SpecialFunctions.h"

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

//DigitalInput states
typedef struct
{        
    bool state;
    bool previousState;
}s_digitalInputState_t;

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
    s_digitalInputState_t   inputState;   
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder 
{
	public:
                        RotaryEncoder   (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);   
    void                begin           (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON);

    e_direction_t       getTurningDirection ();
    bool                buttonPressed       ();
    
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

class Output {

	public:
    //------------------------------------------------------------------------
    //Constructor Initialisierung
	Output				(); 
	Output				(const uint8_t ON_VALUE);		
    
	void 	begin		(const uint8_t ON_VALUE);
    void  	tick		();		//Muss getickt werden jeden loop

	void	blink		    (const uint8_t BLINKS, const int BLINK_INTERVAL);		                //Blinkt für angeforderte Anzahl und Interval
    void    blinkWithBreak  (const uint8_t BLINKS, const int BLINK_INTERVAL, const int BREAK_TIME); //Blinkt dauerhaft mit optinaler Pause
	void 	set			    ();		//Output ON
	void 	reset		    ();		//Output OFF
    void    setValue        (const uint8_t VALUE);
    uint8_t getValue        ();   
			
	private:	
	
	e_outputMode_t	mode;       //Aktueller Modus

	uint8_t blinks_requested;	//Angefragte Blinks
	uint8_t count;      		//Counter der blinks  
	uint8_t actualValue;  	    //Aktueller Wert 
	uint8_t onValue;			//Welcher Wert wird geschieben bei object.set():	

	Timeout to_blink;			//Timeout für Blink Interval
    Timeout to_break;           //Timeout für Pausen interval
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
    void 	    setValue(const uint8_t VALUE);
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