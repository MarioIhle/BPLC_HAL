#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
uint8_t* p_INT_count_HAL_REV_A;

static void INT_ISR()
{
   *p_INT_count_HAL_REV_A = 2;
}

HAL_MCU11_revA::HAL_MCU11_revA()
{}
    
void HAL_MCU11_revA::begin()
{
    //encoder
    pinMode(this->PIN.encoder_A, INPUT);
    pinMode(this->PIN.encoder_B, INPUT);
    pinMode(this->PIN.encoder_Z, INPUT);
    //p_ld1-3
    pinMode(this->PIN.ld1, OUTPUT);
    pinMode(this->PIN.ld2, OUTPUT);
    pinMode(this->PIN.ld3, OUTPUT);    
    //P_BUZZER
    pinMode(this->PIN.buzzer, OUTPUT);
    //p_oen
    pinMode(this->PIN.p_oen, OUTPUT);
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
    this->p_encoder->halCallback(digitalRead(this->PIN.encoder_A), digitalRead(this->PIN.encoder_B), digitalRead(this->PIN.encoder_Z));
    //p_oen schreiben
    if(this->p_oen->isThereANewPortValue())
    {
        digitalWrite(this->PIN.p_oen, this->p_oen->halCallback().value);
    }
    //buzzer
    if(this->p_buzzer->isThereANewPortValue())
    {
        analogWrite(this->PIN.buzzer, this->p_buzzer->halCallback().value);
    }
    //p_ld1
    if(this->p_ld1->isThereANewPortValue())
    {
        analogWrite(this->PIN.ld1, this->p_ld1->halCallback().value);
    }
    //LD_COMMUNACTION_STATE
    if(this->p_ld2->isThereANewPortValue())
    {
        analogWrite(this->PIN.ld2, this->p_ld2->halCallback().value);
    }
    //p_ld3
    if(this->p_ld3->isThereANewPortValue())
    {
        analogWrite(this->PIN.ld3, this->p_ld3->halCallback().value);  
    }    
}