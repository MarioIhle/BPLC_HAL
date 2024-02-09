#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
uint8_t* p_INT_count_HAL_REV_A;

static void INT_ISR()
{
   *p_INT_count_HAL_REV_A = 1;
}

HAL_MCU11_revA::HAL_MCU11_revA()
{}
    
void HAL_MCU11_revA::begin()
{
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

    BPLC_LOG logPrint;
    logPrint.printLog("MCU11revA INIT SUCCESSFUL");  
}

void HAL_MCU11_revA::mapEncoder(rotaryEncoder* P_ENCODER)
{
    this->p_encoder = P_ENCODER;
}

void HAL_MCU11_revA::mapBuzzer(Output* P_BUZZER_OBJECT)
{
    this->p_buzzer = P_BUZZER_OBJECT;
}

void HAL_MCU11_revA:: mapLD1(Output* P_LD1_OBJECT)
{
    this->p_ld1 = P_LD1_OBJECT;
}

void HAL_MCU11_revA::mapLD2(Output* P_LD2_OBJECT)
{
    this->p_ld2 = P_LD2_OBJECT;
}

void HAL_MCU11_revA::mapLD3(Output* P_LD3_OBJECT)
{
    this->p_ld3 = P_LD3_OBJECT;
}

void HAL_MCU11_revA::mapOEN(Output* P_OEN_OBJECT)
{
    this->p_oen = P_OEN_OBJECT;
}

void HAL_MCU11_revA::mapINT(uint8_t* P_INT_COUNT)
{
    p_INT_count_HAL_REV_A = P_INT_COUNT;
}

void HAL_MCU11_revA::tick()
{  
    //encoder
    u_IO_DATA_BASE_t tempbuffer;
    tempbuffer.rotaryEncoderData.stateA             = digitalRead(this->PIN.ENCODER_A);
    tempbuffer.rotaryEncoderData.stateB             = digitalRead(this->PIN.ENCODER_B);
    tempbuffer.rotaryEncoderData.statePushButton    = digitalRead(this->PIN.ENCODER_BUTTON);
    this->p_encoder->halCallback(&tempbuffer);
    //p_oen schreiben
    if(this->p_oen->newDataAvailable())
    {
        analogWrite(this->PIN.OEN, this->p_oen->halCallback().analogIoData.value);
    }
    //BUZZER
    if(this->p_buzzer->newDataAvailable())
    {
        analogWrite(this->PIN.BUZZER, this->p_buzzer->halCallback().analogIoData.value);
    }
    //p_ld1
    if(this->p_ld1->newDataAvailable())
    {
        analogWrite(this->PIN.LD1, this->p_oen->halCallback().analogIoData.value);
    }
    //LD_COMMUNACTION_STATE
    if(this->p_ld2->newDataAvailable())
    {
        analogWrite(this->PIN.LD2, this->p_oen->halCallback().analogIoData.value);
    }
    //p_ld3
    if(this->p_ld3->newDataAvailable())
    {
        analogWrite(this->PIN.LD3, this->p_oen->halCallback().analogIoData.value);
    } 
}