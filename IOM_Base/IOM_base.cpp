#include "IOM_base.h"

//####################################################################
//BASIS OBJEKTE
//####################################################################

//--------------------------------------------------------------------
//DIGITALINPUT
DigitalInput::DigitalInput    ()
{}

bool DigitalInput::ishigh()
{
    const bool PIN_IS_HIGH = (bool)(this->inputState.value == true && this->inputState.previousValue == true);

#ifdef DEBUG_IOM
if(PIN_IS_HIGH)
{
	Serial.println("PIN_IS_HIGH");	
}
#endif

    return PIN_IS_HIGH;
}

bool DigitalInput::islow()
{
    const bool PIN_IS_LOW = (bool)(this->inputState.value == false && this->inputState.previousValue == false);

#ifdef DEBUG_IOM
if(PIN_IS_LOW)
{
	Serial.println("PIN_IS_LOW");	
}
#endif

    return PIN_IS_LOW;
}

bool DigitalInput::posFlank()
{
    const bool POSITIVE_FLANK_OCCURED = (bool)(this->inputState.value == true && this->inputState.previousValue == false);

#ifdef DEBUG_IOM
if(POSITIVE_FLANK_OCCURED)
{
	Serial.println("POSITIVE_FLANK_OCCURED");	
}
#endif

    return POSITIVE_FLANK_OCCURED;
}

bool DigitalInput::negFlank()
{
    const bool NEGATIVE_FLANK_OCCURED = (bool)(this->inputState.value == false && this->inputState.previousValue == true);

#ifdef DEBUG_IOM
if(NEGATIVE_FLANK_OCCURED)
{
	Serial.println("NEGATIVE_FLANK_OCCURED");	
}
#endif

    return NEGATIVE_FLANK_OCCURED;
}

void DigitalInput::setPortState(const bool STATE)
{
	this->inputState.previousValue 	= this->inputState.value;
	this->inputState.value 			= STATE;
}

//--------------------------------------------------------------------
//ANALOGINPUT
AnalogInput::AnalogInput()
{}
   
uint16_t AnalogInput::getValue()
{
	return this->inputValue.value;
}

float AnalogInput::getValueInVolt()
{
	return this->inputValueInVolt;
}

void AnalogInput::setAlarm(const uint16_t ALARM_VALUE)
{
	this->alarmValue = ALARM_VALUE;
}

bool AnalogInput::isAlarmValueReached()
{
	return (bool)(this->inputValue.value >= this->alarmValue);
}

void AnalogInput::setPortValue(const uint16_t VALUE)
{
	this->inputValue.previousValue 	= this->inputValue.value;
	this->inputValue.value 			= VALUE;
}

void AnalogInput::setValueInVolt(const float VALUE_IN_VOLT)
{
	this->inputValueInVolt = VALUE_IN_VOLT;
}

//--------------------------------------------------------------------
//OUTPUT
Output::Output() 
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= 255;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

Output::Output(const e_outputType_t OUTPUT_TYPE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= 255;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

Output::Output(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

Output::Output(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

void Output::begin(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

void Output::begin(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;

	this->actualValue.previousValue = 1;	//Damit initial der Wert von der jeweiligen HAL abgeholt wird (prevoiours != now)
}

void Output::tick()
{
	switch(this->mode)
	{
		case OUTPUTMODE__OFF:
			this->actualValue.value = false;			
		break;

		case OUTPUTMODE__ON:
			this->actualValue.value = this->setting.onValue;
		break;

		case OUTPUTMODE__VALUE:
			//keine Änderung, einfach gesetzten Wert halten 
		break;

		case OUTPUTMODE__BLINK:
			if(this->blinkParameter.to_blink.check())
			{   
				if(this->blinkParameter.count < this->blinkParameter.blinks_requested)   
				{
					if(this->actualValue.value == 0)
					{
						this->actualValue.value = this->setting.onValue;
						this->blinkParameter.count++;
					}
					else
					{
						this->actualValue.value = 0;
					}

					this->blinkParameter.to_blink.reset();
				}
				else
				{
					this->mode = OUTPUTMODE__OFF;
				}			
			}
		break;

        case OUTPUTMODE__BLINK_WITH_BREAK:
            //Blinken
            if(this->blinkParameter.to_break.check())
			{      
				if(this->blinkParameter.to_blink.check())
				{
					if(this->blinkParameter.count < this->blinkParameter.blinks_requested)   
					{
						if(this->actualValue.value == 0)
						{
							this->actualValue.value = this->setting.onValue;
							this->blinkParameter.count++;
						}
						else
						{
							this->actualValue.value = false;
						}
					}
					else
					{
						//Blinken abgeschlossen, Break Timeout starten	
						this->actualValue.value = false;		
                		this->blinkParameter.to_break.reset();                          
						this->blinkParameter.count  = 0;						
					}		
					this->blinkParameter.to_blink.reset();
				}		
			}
		break;

		default:
			this->mode = OUTPUTMODE__OFF;
		break;
	}
}

void Output::blink(const uint8_t BLINKS, const int BLINK_INTERVAL)
{
	this->blinkParameter.blinks_requested = BLINKS;
    this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
    this->blinkParameter.count = 0; 

    this->mode = OUTPUTMODE__BLINK;
}

void Output::blinkWithBreak(const uint8_t BLINKS, const int BLINK_INTERVAL, const int BREAK_TIME)
{
	this->blinkParameter.blinks_requested = BLINKS;
    this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
    this->blinkParameter.to_break.setInterval(BREAK_TIME);
    this->blinkParameter.count = 0; 

    this->mode = OUTPUTMODE__BLINK_WITH_BREAK;
}

void Output::set()
{
	this->mode = OUTPUTMODE__ON;
}

void Output::reset()
{
	this->mode = OUTPUTMODE__OFF;
}

void Output::setValue(const uint8_t VALUE)
{
	this->actualValue.value = VALUE;
	this->mode 				= OUTPUTMODE__VALUE;
}

void Output::setOnValue(const uint8_t VALUE)
{
    this->setting.onValue = VALUE;
}

s_portValue_t Output::getValue()
/**
 * @return Achtung kein BOOL! TRUE = max.255
*/
{
	return this->actualValue;	
}

e_outputType_t Output::getOutputType()
{
	return this->setting.outputType;
}

bool Output::isThereANewPortValue()
{
	const bool THERE_IS_A_NEW_PORT_VALUE_TO_WRITE 	= (bool)(this->actualValue.value != this->actualValue.previousValue);
	this->actualValue.previousValue 				= this->actualValue.value;
	return THERE_IS_A_NEW_PORT_VALUE_TO_WRITE;
}

//####################################################################
//SPEZIAL OBJEKTE
//####################################################################

//--------------------------------------------------------------------
//ROTARY ENCODER
RotaryEncoder::RotaryEncoder()
{}

RotaryEncoder::RotaryEncoder(DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON)
{   
    this->p_A = P_PORT_A;
    this->p_B = P_PORT_B;
    this->p_pushButton = P_PORT_PUSHBUTTON;   
}

void RotaryEncoder::begin   (DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON)
{
    this->p_A = P_PORT_A;
    this->p_B = P_PORT_B;
    this->p_pushButton = P_PORT_PUSHBUTTON;    
}

e_direction_t RotaryEncoder::getTurningDirection()
{
    e_direction_t direction = idle;    

    if(this->p_A->negFlank() && this->p_B->ishigh())
    {
        direction = right;
    }
    else if(this->p_B->negFlank() && this->p_A->ishigh())
    {
        direction = left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->p_pushButton->posFlank();
}

//--------------------------------------------------------------------
//H-Bridge
H_Bridge::H_Bridge()
{}

H_Bridge::H_Bridge(Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_EN 	= P_EN_L;
	this->p_R_EN 	= P_EN_R;
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;

	this->p_L_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_R_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void H_Bridge::begin()
{
	this->p_L_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_R_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void H_Bridge::begin(Output* P_EN_L, Output* P_EN_R, Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_EN 	= P_EN_L;
	this->p_R_EN 	= P_EN_R;
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;
	
	this->p_L_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_R_EN->begin(OUTPUTTYPE__PUSH_PULL, true);
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void H_Bridge::setDirection(const e_direction_t DIRECTION)
{
	switch(DIRECTION)
	{
		case idle:
			this->p_L_EN->reset();
			this->p_R_EN->reset();
		break;

		case left:
			this->p_L_EN->reset();
			this->p_R_EN->set();
		break;

		case right:
			this->p_L_EN->set();
			this->p_R_EN->reset();
		break;

		default:
			this->driveDirection = idle;
		break;
	}
}

void H_Bridge::setSpeed(const uint8_t SPEED)
{
	this->p_R_PWM->setValue(SPEED);
	this->p_L_PWM->setValue(SPEED);
}
