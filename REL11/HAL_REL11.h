#ifndef HAL_DI11_h
#define HAL_DI11_h

//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE PORTS
//-------------------------------------------------------------
//DIGITAL INPUTS
typedef enum
{
    DI1,
    DI2,
    DI3,
    DI4,
    DI5,
    DI6,
    DI7,
    DI8,
    DI_COUNT,
}e_digitalInputPorts_t;

//-------------------------------------------------------------
//SPEZIAL IO TYPEN
//-------------------------------------------------------------
//DigitalInput
typedef struct
{        
    bool state;
    bool previousState;
}s_digitalInputState_t;

//ROTARY ENCODER
typedef struct
{
    s_digitalInputState_t   A;
    s_digitalInputState_t   B;
    s_digitalInputState_t   Z;    
}s_rotaryEncoderState_t;

typedef struct
{
    uint8_t A;
    uint8_t B;
    uint8_t Z;
}s_rotaryEncoderPins_t;
//__________________________________
//H-BRÜCKE
typedef struct
{
    uint8_t input_1;
    uint8_t input_2;
    uint8_t enable;
}s_H_bridgePins_t;

typedef struct
{    
    bool    input_1;
    bool    input_2;
    uint8_t enable;
}s_H_bridgeState_t;

//-------------------------------------------------------------
//HAL_IO KLASSE
//-------------------------------------------------------------
#define INPUT_READ_COUNT 2

class HAL_REL11 {

    public:
    HAL_REL11();
    void        begin();
    void        tick();
    
    bool    relState[3];

    private:
    void    setREl();
    uint8_t pins[3];
     
};

#endif