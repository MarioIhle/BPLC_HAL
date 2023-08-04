#include "IOM_base.h"


//--------------------------------------------------------------------
//FLANKENAUSWERTUNG KLASSE
//--------------------------------------------------------------------
Flankenauswertung::Flankenauswertung()
{}
//____________________________________________________________________
Flankenauswertung::Flankenauswertung(s_digitalInputState_t* P_STATE)
{
    this->p_halState = P_STATE;
}
//____________________________________________________________________
void Flankenauswertung::begin(s_digitalInputState_t* P_STATE)
{
    this->p_halState = P_STATE;
}
//____________________________________________________________________
bool Flankenauswertung::low()
{
    return (bool)(this->p_halState->previousState == false && this->p_halState->state == false);
}
//____________________________________________________________________
bool Flankenauswertung::high()
{
    return (bool)(this->p_halState->previousState == true && this->p_halState->state == true);
}
//____________________________________________________________________
bool Flankenauswertung::negFlank()
{
    return (bool)(this->p_halState->previousState == true && this->p_halState->state == false);
}
//____________________________________________________________________
bool Flankenauswertung::posFlank()
{
  return (bool)(this->p_halState->previousState == false && this->p_halState->state == true);
}

//--------------------------------------------------------------------
//DIGITALSETTER KLASSE
//--------------------------------------------------------------------
DigitalSetter::DigitalSetter()
{}
//____________________________________________________________________
DigitalSetter::DigitalSetter(uint8_t PORT, HAL_IO* P_HAL)
{
  this->p_hal   = P_HAL;
  this->port    = PORT;
}
//____________________________________________________________________
void DigitalSetter::begin(uint8_t PORT, HAL_IO* P_HAL)
{
  this->p_hal   = P_HAL;
  this->port    = PORT;
}
//____________________________________________________________________
void DigitalSetter::setState(const bool STATE)
{
  this->p_hal->digitalOutputState[this->port] = STATE;
}
//____________________________________________________________________
void DigitalSetter::set()
{
  this->p_hal->digitalOutputState[this->port] = true;  
}
//____________________________________________________________________
void DigitalSetter::reset()
{  
  this->p_hal->digitalOutputState[this->port] = false;
}
//____________________________________________________________________
bool DigitalSetter::get()
{  
  return this->p_hal->digitalOutputState[this->port];
}
//--------------------------------------------------------------------
//ANALOG KLASSE
//--------------------------------------------------------------------
AnalogSetter::AnalogSetter()
{}
//____________________________________________________________________
AnalogSetter::AnalogSetter(uint8_t PORT, HAL_IO* P_HAL)
{
  this->p_hal = P_HAL;
  this->port  = PORT;
}
//____________________________________________________________________
void AnalogSetter::begin(uint8_t PORT, HAL_IO* P_HAL)
{
  this->p_hal = P_HAL;
  this->port  = PORT;
}
//____________________________________________________________________
void AnalogSetter::setValue(const uint8_t VALUE)
{
  this->p_hal->analogOutputState[this->port] = VALUE;
}
//____________________________________________________________________
void AnalogSetter::setMax()
{
  this->p_hal->analogOutputState[this->port] = 255;
}
//____________________________________________________________________
void AnalogSetter::setMin()
{  
  this->p_hal->analogOutputState[this->port] = 0;
}
//____________________________________________________________________
uint16_t AnalogSetter::get()
{  
  return this->p_hal->analogOutputState[this->port];
}
//--------------------------------------------------------------------
//DIGITAL INPUT KLASSE
//--------------------------------------------------------------------
DigitalInput::DigitalInput()
{}
//____________________________________________________________________
DigitalInput::DigitalInput(e_digitalInputPorts_t PORT, HAL_IO* P_HAL)
{
  this->get.begin(&P_HAL->digitalInputState[(uint8_t)PORT]);
} 
//____________________________________________________________________
void DigitalInput::begin(e_digitalInputPorts_t PORT, HAL_IO* P_HAL)
{
  this->get.begin(&P_HAL->digitalInputState[(uint8_t)PORT]);
}

//--------------------------------------------------------------------
//ANALOG INPUT KLASSE
//--------------------------------------------------------------------
AnalogInput::AnalogInput()
{}
//____________________________________________________________________
AnalogInput::AnalogInput(e_analogInputPorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
void AnalogInput::begin(e_analogInputPorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
uint16_t AnalogInput::read()
{
    return this->p_HAL->analogInputState[(uint8_t)this->port];
}

//--------------------------------------------------------------------
//TEMPERATUR SENSOR KLASSE
//--------------------------------------------------------------------
TemperaturSensor::TemperaturSensor()
{}
//____________________________________________________________________
TemperaturSensor::TemperaturSensor(e_tempSensPorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
void TemperaturSensor::begin(e_tempSensPorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
int16_t TemperaturSensor::getTemperatur()
{
  int16_t NEW_TEMP = map(this->p_HAL->rawTempSensorState[(uint8_t)this->port], 175, 350, 21, 65); //<-- je nach Hardware anpassen   
  NEW_TEMP = (NEW_TEMP * 0.3) + (this->oldTemp * 0.7);
  this->oldTemp = NEW_TEMP;
  return NEW_TEMP;
}

//--------------------------------------------------------------------
//ROTARY ENCODER KLASSE
//--------------------------------------------------------------------
RotaryEncoder::RotaryEncoder()
{}
//____________________________________________________________________
RotaryEncoder::RotaryEncoder(e_digitalInputPorts_t PUSH_BUTTON, e_digitalInputPorts_t A, e_digitalInputPorts_t B, HAL_IO* P_HAL)     
{
  this->getPushButton.begin(&P_HAL->digitalInputState[(uint8_t)PUSH_BUTTON]);
  this->A.begin(&P_HAL->digitalInputState[(uint8_t)A]);
  this->B.begin(&P_HAL->digitalInputState[(uint8_t)B]);
}
//____________________________________________________________________
void RotaryEncoder::begin(e_digitalInputPorts_t PUSH_BUTTON, e_digitalInputPorts_t A, e_digitalInputPorts_t B, HAL_IO* P_HAL)
{   
  this->getPushButton.begin(&P_HAL->digitalInputState[(uint8_t)PUSH_BUTTON]);
  this->A.begin(&P_HAL->digitalInputState[(uint8_t)A]);
  this->B.begin(&P_HAL->digitalInputState[(uint8_t)B]);
}
//____________________________________________________________________
e_direction_t RotaryEncoder::getTurningDirection()	
{
  e_direction_t direction = idle;    
  if(this->A.negFlank() && this->B.high())
  {
    direction = right;
  }
  else if(this->B.negFlank() && this->A.high())
  {
    direction = left;
  }
  return direction;
}

//--------------------------------------------------------------------
//DIGITAL OUTPUT KLASSE
//--------------------------------------------------------------------
DigitalOutput::DigitalOutput()
{}
//____________________________________________________________________
DigitalOutput::DigitalOutput(e_digitalOutputPorts_t PORT, HAL_IO* P_HAL)
{ 
  this->state.begin((uint8_t)PORT, P_HAL);  
}
//____________________________________________________________________
void DigitalOutput::begin(e_digitalOutputPorts_t PORT, HAL_IO* P_HAL)
{                   
  this->state.begin((uint8_t)PORT, P_HAL);  
}

//--------------------------------------------------------------------
//ANALOG OUTPUT KLASSE
//--------------------------------------------------------------------
Relais::Relais()
{}
//____________________________________________________________________
Relais::Relais(e_relaisPorts_t PORT, HAL_IO* P_HAL)
{
  this->state.begin((uint8_t)PORT, P_HAL);  
}
//____________________________________________________________________
void Relais::begin(e_relaisPorts_t PORT, HAL_IO* P_HAL)
{
  this->state.begin((uint8_t)PORT, P_HAL);  
}

//--------------------------------------------------------------------
//ANALOG OUTPUT KLASSE
//--------------------------------------------------------------------
AnalogOutput::AnalogOutput()
{}
//____________________________________________________________________
AnalogOutput::AnalogOutput(e_analogOutputPorts_t PORT, HAL_IO* P_HAL)
{
  this->value.begin((uint8_t)PORT, P_HAL);  
}
//____________________________________________________________________
void AnalogOutput::begin(e_analogOutputPorts_t PORT, HAL_IO* P_HAL)
{
  this->value.begin((uint8_t)PORT, P_HAL);  
}

//--------------------------------------------------------------------
//H-BRÜCKE KLASSE
//-------------------------------------------------------------------- 
H_Bridge::H_Bridge()
{}
//____________________________________________________________________
H_Bridge::H_Bridge(e_hBridgePorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
void H_Bridge::begin(e_hBridgePorts_t PORT, HAL_IO* P_HAL)
{
    this->p_HAL = P_HAL;
    this->port  = PORT;
}
//____________________________________________________________________
void H_Bridge::setSpeed(const uint8_t HB_SPEED)
{
    this->p_HAL->hBridgeState[this->port].enable = HB_SPEED;
}
//____________________________________________________________________
void H_Bridge::setDirection(const e_direction_t DIRECTION)
{
    switch (DIRECTION)
    {
        default:
        case idle:
            this->p_HAL->hBridgeState[this->port].input_1 = false;
            this->p_HAL->hBridgeState[this->port].input_2 = false;
        break; 

        case left:
            this->p_HAL->hBridgeState[this->port].input_1 = true;
            this->p_HAL->hBridgeState[this->port].input_2 = false;
        break; 

        case right:
            this->p_HAL->hBridgeState[this->port].input_1 = false;
            this->p_HAL->hBridgeState[this->port].input_2 = true;
        break; 
    }  
}