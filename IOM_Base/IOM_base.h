#ifndef IOM_base_h
#define IOM_base_h

#include "Arduino.h"
#include "HAL_DI11.h"
#include "HAL_AI11.h"
#include "HAL_DO11.h"
#include "HAL_REL11.h"

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
    DigitalInput();
    DigitalInput(const unint8_t PIN);

    void    begin       (const unint8_t PIN);
    bool 	ishigh		();
    bool	islow		();
	bool 	posFlank	();	
	bool 	negFlank	();	

    private:
    unint8_t    pin;
    bool        lasteState;
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder {
	public:
    RotaryEncoder(const unit8_t A, const unit8_t B, const unit8_t Z);
   
    void                begin   (const unit8_t A, const unit8_t B, const unit8_t Z);

    e_direction_t       getTurningDirection();
    bool                buttonPressed;
    
  	private:
    Flankenauswertung   A;
    Flankenauswertung   B; 
    Flankenauswertung   Z; 
};












//--------------------------------------------------------------------
//DIGITALSETTER KLASSE
//--------------------------------------------------------------------
class DigitalOutput
{
    public:
    DigitalSetter   (const unint8_t PIN);
    void    begin   (const unint8_t PIN);
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
#endif