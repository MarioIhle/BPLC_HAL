#include "BPLC_IOM.h"

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

bool DigitalInput::posEdge()
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

bool DigitalInput::negEdge()
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
   
void AnalogInput::setMaxVoltage(const float VOLTAGE)
{
	this->maxVoltage = VOLTAGE;
}

uint16_t AnalogInput::getValue()
{
	return this->inputValue.value;
}

float AnalogInput::getValueInVolt()
{
	const float VALUE_IN_VOLT = mapf(this->inputValue.value, 0, 65535, 0, this->maxVoltage);
	return VALUE_IN_VOLT;
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
}

Output::Output(const e_outputType_t OUTPUT_TYPE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= 255;
    this->mode 					= OUTPUTMODE__OFF;
}

Output::Output(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

Output::Output(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::begin(const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUTTYPE__PUSH;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::begin(const e_outputType_t OUTPUT_TYPE, const uint8_t ON_VALUE)
{
	this->setting.outputType 	= OUTPUT_TYPE;
	this->setting.onValue 		= ON_VALUE;
    this->mode 					= OUTPUTMODE__OFF;
}

void Output::blink(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL)
{
	this->blinkParameter.blinks_requested = BLINKS;
	this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
	this->blinkParameter.to_blink.now();
	this->blinkParameter.count = 0;		
	
	this->mode = OUTPUTMODE__BLINK;	
}

void Output::blinkWithBreak(const uint8_t BLINKS, const unsigned long BLINK_INTERVAL, const unsigned long BREAK_TIME)
{
	const bool ALREADY_IN_BLINK__WITH_BRAKE_MODE 	= (bool)(this->mode == OUTPUTMODE__BLINK_WITH_BREAK);
	const bool PARAMETERS_DID_NOT_CHANGED 			= (bool)((this->blinkParameter.blinks_requested == BLINKS) 
															&&(this->blinkParameter.to_blink.getInterval() == BLINK_INTERVAL)
															&&(this->blinkParameter.to_break.getInterval() == BREAK_TIME));

	if(ALREADY_IN_BLINK__WITH_BRAKE_MODE && PARAMETERS_DID_NOT_CHANGED)
	{
		//nichts ändern, sonst wird Blinktakt immer neu gestartet
	}
	else
	{
		this->blinkParameter.blinks_requested = BLINKS;
		this->blinkParameter.to_blink.setInterval(BLINK_INTERVAL);
		this->blinkParameter.to_blink.now();
		this->blinkParameter.to_break.setInterval(BREAK_TIME);
		this->blinkParameter.to_break.now();
		this->blinkParameter.count = 0; 
	}
	
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
{
	//Portdaten wurden abgeholt und können jetzt abgeglichen werden
	this->actualValue.previousValue = this->actualValue.value;
	return this->actualValue;	
}

e_outputType_t Output::getOutputType()
{
	return this->setting.outputType;
}

bool Output::isThereANewPortValue()
{
	//Interne Statusmaschine ticken, um Blinken usw. zu ermöglichen
	this->tick();
	//Wenn aus der Internen Statusmaschine neue Portdaten hervor gehen wird dies hier erkannt
	const bool THERE_IS_A_NEW_PORT_VALUE_TO_WRITE = (bool)(this->actualValue.value != this->actualValue.previousValue);
	return THERE_IS_A_NEW_PORT_VALUE_TO_WRITE;
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

e_movement_t RotaryEncoder::getTurningDirection()
{
    e_movement_t direction = movement_idle;    

    if(this->p_A->negEdge() && this->p_B->ishigh())
    {
        direction = movement_right;
    }
    else if(this->p_B->negEdge() && this->p_A->ishigh())
    {
        direction = movement_left;
    }

    return direction;
}

bool RotaryEncoder::isButtonPressed()
{
    return this->p_pushButton->posEdge();
}

//--------------------------------------------------------------------
//PT10x 
PT10x::PT10x(AnalogInput* P_PORT, const float VOLATGE_AT_0_DEG, const float VOLTAGE_AT_100_DEG)
{}

void PT10x::begin(AnalogInput* P_PORT, const float VOLATGE_AT_0_DEG, const float VOLTAGE_AT_100_DEG)
{
	this->p_PORT 				= P_PORT;
	this->voltage.atZero 		= VOLATGE_AT_0_DEG;
	this->voltage.atOneHundred 	= VOLTAGE_AT_100_DEG;
}

int  PT10x::getTemperatur()
{
	const float VOLTAGE_AT_PORT = this->p_PORT->getValueInVolt();

	return  map(VOLTAGE_AT_PORT, this->voltage.atZero, this->voltage.atOneHundred, 0, 100);
}

//--------------------------------------------------------------------
//MOTOR
MOTOR::MOTOR()
{}

void MOTOR::stop()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 0;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::stopAndBreak()
{
	//Bei Stopp letzte Parameter merken 
    this->motParams.old.direction = this->motParams.direction; 
    this->motParams.old.speed     = this->motParams.speed;
    this->motParams.direction     = movement_idle; 
    this->motParams.speed     	  = 255;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::start()
{
	//Bei Start letzte gemerkte Parameter laden 
    this->motParams.direction     = this->motParams.old.direction; 
    this->motParams.speed     	  = this->motParams.old.speed;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::setSpeed(const uint8_t SPEED)
{   
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}

void MOTOR::setDirection(const e_movement_t DIRECTION)
{
    this->motParams.direction     = DIRECTION; 
	f_thereAreNewDriveParametersAvailable = true;    
}

void MOTOR::setDirectionAndSpeed(const e_movement_t DIRECTION, const uint8_t SPEED)
{
    this->motParams.direction     = DIRECTION; 
    this->motParams.speed     	  = SPEED;   
	f_thereAreNewDriveParametersAvailable = true;
}

float MOTOR::getCurrent()
{
    return this->motParams.current;
}

e_movement_t MOTOR::getDirection()
{
    return this->motParams.direction;
}

uint8_t MOTOR::getSpeed()
{
    return this->motParams.speed;
}

e_DRIVE_STATE_t MOTOR::getDriveState()
{
	return this->driveState;
}

void MOTOR::setCurrent(const float CURRENT)
{
	this->motParams.current = CURRENT;
}

bool MOTOR::newDriveParameterAvailable()
{
	const bool newDriveParameterAvailable 		= this->f_thereAreNewDriveParametersAvailable;
	this->f_thereAreNewDriveParametersAvailable = false;
	return newDriveParameterAvailable;
}

//--------------------------------------------------------------------
//Software H-Bridge
Software_H_Bridge::Software_H_Bridge()
{}

Software_H_Bridge::Software_H_Bridge(Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::begin()
{
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::begin(Output* P_PWM_L, Output* P_PWM_R)
{
	this->p_L_PWM 	= P_PWM_L;
	this->p_R_PWM 	= P_PWM_R;
	
	this->p_L_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
	this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
}

void Software_H_Bridge::setDirection(const e_movement_t DIRECTION)
{
	switch(DIRECTION)
	{
		case movement_idle:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
		break;

		case movement_left:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
			this->p_R_PWM->setValue(this->driveSpeed);
			this->p_L_PWM->setValue(this->driveSpeed);
		break;

		case movement_right:
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL_INVERT, 255);
			this->p_R_PWM->begin(OUTPUTTYPE__PUSH_PULL, 255);
			this->p_R_PWM->setValue(this->driveSpeed);
			this->p_L_PWM->setValue(this->driveSpeed);
		break;

		default:
			this->driveDirection = movement_idle;
		break;
	}
}

void Software_H_Bridge::setSpeed(const uint8_t SPEED)
{
	this->driveSpeed = SPEED;

	this->p_R_PWM->setValue(SPEED);
	this->p_L_PWM->setValue(SPEED);
}
