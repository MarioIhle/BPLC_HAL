#ifndef IOM_base_h
#define IOM_base_h

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
//FLANKENAUSWERTUNG KLASSE
//--------------------------------------------------------------------
class Flankenauswertung
{
    public:
    Flankenauswertung();
    Flankenauswertung(void* P_STATE);

    void    begin       ();
    bool 	high		();
    bool	low		    ();
	bool 	posFlank	();	
	bool 	negFlank	();	
    private:
    s_digitalInputState_t*  p_halState;
};

//--------------------------------------------------------------------
//DIGITALSETTER KLASSE
//--------------------------------------------------------------------
class DigitalSetter
{
    public:
    DigitalSetter   ();
    void    begin   ();
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
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput {
	public:
    DigitalInput();

    void    begin       ();
    Flankenauswertung   get;  
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
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder {
	public:
    RotaryEncoder();
   
    void                begin   ();

    e_direction_t       getTurningDirection();
    Flankenauswertung   getPushButton;
    
  	private:
    Flankenauswertung   A;
    Flankenauswertung   B; 
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