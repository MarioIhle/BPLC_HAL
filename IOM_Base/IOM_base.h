#ifndef IOM_base_h
#define IOM_base_h

//#include "HAL_DI11.h"

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
    Flankenauswertung(s_digitalInputState_t* P_STATE);

    void    begin       (s_digitalInputState_t* P_STATE);
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
    DigitalSetter   (uint8_t PORT, HAL_IO* P_HAL);
    void    begin   (uint8_t PORT, HAL_IO* P_HAL);
    void 	setState(const bool STATE);
    void    set     ();
    void    reset   ();
    bool    get     ();

    private:
    HAL_IO*     p_hal;
    uint8_t     port;
};
//--------------------------------------------------------------------
//ANALOGSETTER KLASSE
//--------------------------------------------------------------------
class AnalogSetter
{
    public:
    AnalogSetter        ();
    AnalogSetter        (uint8_t PORT, HAL_IO* P_HAL);
    void        begin   (uint8_t PORT, HAL_IO* P_HAL);
    void 	    setValue(const uint8_t VALUE);
    void        setMax  ();
    void        setMin  ();
    uint16_t    get     ();
    private:
    HAL_IO*   p_hal;
    uint8_t   port;
};

//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
class DigitalInput {
	public:
    DigitalInput();
    DigitalInput        (e_digitalInputPorts_t PORT, HAL_IO* P_HAL); 
    void    begin       (e_digitalInputPorts_t PORT, HAL_IO* P_HAL);
    Flankenauswertung   get;  
};

//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
//--------------------------------------------------------------------
class AnalogInput {
	public:
    AnalogInput();
    AnalogInput      (e_analogInputPorts_t PORT, HAL_IO* P_HAL);
    void        begin(e_analogInputPorts_t PORT, HAL_IO* P_HAL); 
    uint16_t    read();    
  	private:
    HAL_IO*                 p_HAL;      
	e_analogInputPorts_t    port;
};

//--------------------------------------------------------------------
//TEMPERATUR SENSOR KLASSE
//--------------------------------------------------------------------
class TemperaturSensor{
    public:
    TemperaturSensor();
    TemperaturSensor        (e_tempSensPorts_t PORT, HAL_IO* P_HAL);
    void    begin           (e_tempSensPorts_t PORT, HAL_IO* P_HAL);
    int16_t getTemperatur   ();

    private:
    int16_t oldTemp;
    HAL_IO* p_HAL;      
	e_tempSensPorts_t port;
};

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
class RotaryEncoder {
	public:
    RotaryEncoder();
    RotaryEncoder               (e_digitalInputPorts_t PUSH_BUTTON, e_digitalInputPorts_t A, e_digitalInputPorts_t B, HAL_IO* P_HAL);      
    void                begin   (e_digitalInputPorts_t PUSH_BUTTON, e_digitalInputPorts_t A, e_digitalInputPorts_t B, HAL_IO* P_HAL);

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
    DigitalOutput(e_digitalOutputPorts_t PORT, HAL_IO* P_HAL);
    void begin   (e_digitalOutputPorts_t PORT, HAL_IO* P_HAL);
    DigitalSetter state; 
};

//--------------------------------------------------------------------
//RELAIS KLASSE
//--------------------------------------------------------------------
class Relais{
    public:
    Relais();
    Relais                  (e_relaisPorts_t PORT, HAL_IO* P_HAL);
    void            begin   (e_relaisPorts_t PORT, HAL_IO* P_HAL);     
    DigitalSetter   state;
};

//--------------------------------------------------------------------
//ANALOG OUTPUT KLASSE
//--------------------------------------------------------------------
class AnalogOutput{
    public:
    AnalogOutput();
    AnalogOutput(e_analogOutputPorts_t PORT, HAL_IO* P_HAL);
    void begin  (e_analogOutputPorts_t PORT, HAL_IO* P_HAL);
    AnalogSetter value;
};

//--------------------------------------------------------------------
//H-BRÜCKE KLASSE
//-------------------------------------------------------------------- 
class H_Bridge{
    public:
    H_Bridge();
    H_Bridge            (e_hBridgePorts_t PORT, HAL_IO* P_HAL);
    void    begin       (e_hBridgePorts_t PORT, HAL_IO* P_HAL);
    void    setSpeed    (const uint8_t HB_SPEED);
    void    setDirection(const e_direction_t DIRECTION);

    private:
    HAL_IO*             p_HAL;    
	e_hBridgePorts_t    port;
};
#endif