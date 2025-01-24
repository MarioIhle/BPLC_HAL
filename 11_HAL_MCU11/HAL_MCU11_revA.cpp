#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
volatile e_MCU_INT_ISR_t*  P_ISR_STATE_MCU_REVA = nullptr;

static void INT_ISR_MCU_REV_A()
{
    *P_ISR_STATE_MCU_REVA = MCU_INT_ISR__NEW_INT;
}
HAL_MCU11_revA::HAL_MCU11_revA(volatile e_MCU_INT_ISR_t* P_ISR_STATE)
{
    P_ISR_STATE_MCU_REVA = P_ISR_STATE;
}
void HAL_MCU11_revA::init(const e_EC_ADDR_t ADDR)
{
    //Alles Initialisieren
    this->p_buzzer  = nullptr;
    this->p_encoder = nullptr;
    this->p_ld1     = nullptr;
    this->p_ld2     = nullptr;
    this->p_ld3     = nullptr;
    this->p_oen     = nullptr;
     
    //encoder
    pinMode(this->PIN.ENCODER_A, INPUT);
    pinMode(this->PIN.ENCODER_B, INPUT);
    pinMode(this->PIN.ENCODER_BUTTON, INPUT);
    //p_ld1-3
    pinMode(this->PIN.LD1, OUTPUT);
    pinMode(this->PIN.LD2, OUTPUT);
    pinMode(this->PIN.LD3, OUTPUT);    
    //P_BUZZER
    pinMode(this->PIN.BUZZER, OUTPUT);
    //p_oen
    pinMode(this->PIN.OEN, OUTPUT);
    //INT
    pinMode(this->PIN.INT, INPUT_PULLUP);
    attachInterrupt(this->PIN.INT, INT_ISR_MCU_REV_A, FALLING);       
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232, SERIAL_8N1, 32, 33);
    Serial2.begin(this->baudrate.RS485);    //RS485
    //I2C
    Wire.begin();

    this->printLog("MCU11revA INIT SUCCESSFUL", __FILENAME__, __LINE__);  
}
void HAL_MCU11_revA::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    switch(CHANNEL)
    {
        case MCU_CHANNEL__ENCODER:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__ROTARY_ENCODER)
            {
                this->p_encoder = P_IO_OBJECT;
            }
            break;

        case MCU_CHANNEL__BUZZER:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
            {
                this->p_buzzer = P_IO_OBJECT;
            }            
            break;

        case MCU_CHANNEL__OEN:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
            {
                this->p_oen = P_IO_OBJECT;
            }
            break;

        case MCU_CHANNEL__LD1:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
            {
                this->p_ld1 = P_IO_OBJECT;
            }
            break;

        case MCU_CHANNEL__LD2:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
            {
                this->p_ld2 = P_IO_OBJECT;
            }
            break;

        case MCU_CHANNEL__LD3:
            if(P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
            {
                this->p_ld3 = P_IO_OBJECT;
            }
            break;

        default:
        case MCU_CHANNEL__COUNT:
            this->setError(MCU11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
            break;
    }
}
void HAL_MCU11_revA::tick()
{  
    const bool NO_ERROR = (!this->tickSafety());
    
    if(NO_ERROR)
    {
        //encoder
        u_HAL_DATA_t encoderInput;
        memset(&encoderInput, 0, sizeof(u_HAL_DATA_t));
        encoderInput.encoderData.stateA    = digitalRead(this->PIN.ENCODER_A);
        encoderInput.encoderData.stateB    = digitalRead(this->PIN.ENCODER_B);
        encoderInput.encoderData.stateZ    = digitalRead(this->PIN.ENCODER_BUTTON);        
        this->p_encoder->halCallback(&encoderInput);
        //p_oen schreiben
        if(this->p_oen->newDataAvailable())
        {
            digitalWrite(this->PIN.OEN, this->p_oen->halCallback().digitalIoData.state);
        }
        //BUZZER
        if(this->p_buzzer->newDataAvailable())
        {
            analogWrite(this->PIN.BUZZER, this->p_buzzer->halCallback().analogIoData.value);
        }
        //p_ld1
        if(this->p_ld1->newDataAvailable())
        {
            analogWrite(this->PIN.LD1, this->p_ld1->halCallback().analogIoData.value);
        }
        //LD_COMMUNACTION_STATE
        if(this->p_ld2->newDataAvailable())
        {
            analogWrite(this->PIN.LD2, this->p_ld2->halCallback().analogIoData.value);
        }
        //p_ld3
        if(this->p_ld3->newDataAvailable())
        {
            analogWrite(this->PIN.LD3, this->p_ld3->halCallback().analogIoData.value);
        } 
    }   
    //!Super schrott fix, geht aber nicht anders
    //Problemstellung: Interruptpins der PCF sind mitinander verbunden daher kommt es manchmal zu einem Fehlerzustand auf den PCF und die INT leitung bleibt dann low
    //Damit die INT leitung wieder freigegeben wird, muss eine I2C kommunikation stattdinden
    const bool INT_LINE_GOT_STUCK_LOW_DUE_TO_PCF_ERROR = (digitalRead(this->PIN.INT)== LOW);
    if(INT_LINE_GOT_STUCK_LOW_DUE_TO_PCF_ERROR)
    {
        INT_ISR_MCU_REV_A();
    }    
}
void HAL_MCU11_revA::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {       
        default:
            this->printLog("WRONG COMMAND FOR THIS EXTENSION CARD", __FILENAME__, __LINE__);
            break;
    }
}
bool HAL_MCU11_revA::tickSafety()
{
    bool errorDetected = false;
    
    if(this->p_buzzer == nullptr)
    {
        errorDetected = true;
    }
    else if(this->p_encoder == nullptr)
    {
        errorDetected = true;
    }
    else if(this->p_ld1 == nullptr)
    {
        errorDetected = true;
    }
    else if(this->p_ld2 == nullptr)
    {
        errorDetected = true;
    }
    else if(this->p_ld3 == nullptr)
    {
        errorDetected = true;
    }
    else if(this->p_oen == nullptr)
    {
        errorDetected = true;
    }
    else if(P_ISR_STATE_MCU_REVA == nullptr)
    {
        errorDetected = true;
    }
    else
    {
        errorDetected = false;
    }
    
    return errorDetected;
}