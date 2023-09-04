#ifndef IOM_base_h
#define IOM_base_h

#include "Arduino.h"
#include "HAL_DI11.h"
#include "SpecialFunctions.h"

//--------------------------------------------------------------------
//Typdefinitionen
//--------------------------------------------------------------------
typedef enum
{
    idle,
    left,
    right,
}e_direction_t;

//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput
{
    public:
    DigitalInput    ();
    DigitalInput    (const uint8_t PORT, HAL_DI11* CARD);
    void begin      (const uint8_t PORT, HAL_DI11* CARD);
    //Getter für App
    bool 	ishigh		();
    bool	islow		();
	bool 	posFlank	();	
	bool 	negFlank	();	

    private:
    uint8_t     port;
    HAL_DI11*   p_card;    
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder 
{
	public:
                        RotaryEncoder   (const uint8_t PORT_A, const uint8_t PORT_B, const uint8_t PORT_PUSHBUTTON, HAL_DI11* CARD);   
    void                begin           (const uint8_t PORT_A, const uint8_t PORT_B, const uint8_t PORT_PUSHBUTTON, HAL_DI11* CARD);

    e_direction_t       getTurningDirection ();
    bool                buttonPressed       ();
    
  	private:
    DigitalInput   A;
    DigitalInput   B; 
    DigitalInput   pushButton; 

    HAL_DI11* card;
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