#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
volatile uint64_t* p_ISR_COUNT_MCU_REVB = nullptr;

static void INT_ISR_MCU_REV_B()
{
    *p_ISR_COUNT_MCU_REVB = *p_ISR_COUNT_MCU_REVB + 1;
}
HAL_MCU11_revB::HAL_MCU11_revB(volatile uint64_t* P_ISR_COUNT)
{
    p_ISR_COUNT_MCU_REVB = P_ISR_COUNT;
}
void HAL_MCU11_revB::init(const e_EC_ADDR_t ADDR)
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
    //ld1-3
    pinMode(this->PIN.LD1, OUTPUT);
    pinMode(this->PIN.LD2, OUTPUT);
    pinMode(this->PIN.LD3, OUTPUT);    
    //BUZZER
    pinMode(this->PIN.BUZZER, OUTPUT);
    //oen
    pinMode(this->PIN.OEN, OUTPUT);
    //INT
    pinMode(this->PIN.INT, INPUT);
    attachInterrupt(this->PIN.INT, INT_ISR_MCU_REV_B, FALLING);       
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232, SERIAL_8N1, 32, 33);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
    //I2C
    Wire.begin();

    this->printLog("MCU11revB INIT SUCCESSFUL", __FILENAME__, __LINE__);  
}
void HAL_MCU11_revB::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
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
void HAL_MCU11_revB::tick()
{  
    //Hier prüfen bevor Pointer ins nichts zeigen
    this->tickSafety();

    if(this->noErrorSet())
    {
        //encoder
        u_HAL_DATA_t tempbuffer;
        memset(&tempbuffer, 0, sizeof(u_HAL_DATA_t));
        tempbuffer.encoderData.stateA = digitalRead(this->PIN.ENCODER_A);
        tempbuffer.encoderData.stateB = digitalRead(this->PIN.ENCODER_B);
        tempbuffer.encoderData.stateZ = digitalRead(this->PIN.ENCODER_BUTTON);
        this->p_encoder->halCallback(&tempbuffer);
        //p_oen schreiben       
        if(this->p_oen->newDataAvailable())
        {         
            const bool OEN_STATE = this->p_oen->halCallback().digitalIoData.state;  
            //NPN PullUp schaltung, ausgang also invertieren
            if(OEN_STATE > 0)
            {
                digitalWrite(this->PIN.OEN, HIGH);
            }
            else
            {
                digitalWrite(this->PIN.OEN, LOW);
            }
            
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
}
void HAL_MCU11_revB::tickSafety()
{
    if(this->p_buzzer == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(this->p_encoder == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(this->p_ld1 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(this->p_ld2 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(this->p_ld3 == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(this->p_oen == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
    if(p_ISR_COUNT_MCU_REVB == nullptr)
    {
        this->setError(MCU11_ERROR__CHANNEL_POINTER_NOT_SET, __FILENAME__, __LINE__);
    }
}