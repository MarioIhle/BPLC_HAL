#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
volatile uint64_t* p_ISR_COUNT_MCU_REVA;

static void INT_ISR()
{
    *p_ISR_COUNT_MCU_REVA = *p_ISR_COUNT_MCU_REVA + 1;
}
HAL_MCU11_revA::HAL_MCU11_revA(volatile uint64_t* P_ISR_COUNT)
{
    p_ISR_COUNT_MCU_REVA = P_ISR_COUNT;
}
void HAL_MCU11_revA::init()
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
    attachInterrupt(this->PIN.INT, INT_ISR, FALLING);       
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
    //I2C
    Wire.begin();

    this->printLog("MCU11revA INIT SUCCESSFUL");  
}
void HAL_MCU11_revA::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
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
            this->setError(MCU11_ERROR__CHANNEL_OUT_OF_RANGE);
            break;
    }
}
void HAL_MCU11_revA::tick()
{  
    this->tickSafety();
    //encoder
    u_HAL_DATA_t tempbuffer;
    tempbuffer.encoderData.stateA             = digitalRead(this->PIN.ENCODER_A);
    tempbuffer.encoderData.stateB             = digitalRead(this->PIN.ENCODER_B);
    tempbuffer.encoderData.stateZ    = digitalRead(this->PIN.ENCODER_BUTTON);
    this->p_encoder->halCallback(&tempbuffer);
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
void HAL_MCU11_revA::tickSafety()
{
    if(this->p_buzzer == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(this->p_encoder == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(this->p_ld1 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(this->p_ld2 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(this->p_ld3 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(this->p_oen == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
    if(p_ISR_COUNT_MCU_REVA == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET);
    }
}