#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
HAL_MCU11_revA* p_THIS_HAL_REV_A;

static void INT_ENCODER()
{
   p_THIS_HAL_REV_A->readEncoder();
}

HAL_MCU11_revA::HAL_MCU11_revA()
{}
    
void HAL_MCU11_revA::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->PIN.encoder[0], INPUT);
    pinMode(this->PIN.encoder[1], INPUT);
    pinMode(this->PIN.encoder[2], INPUT);
    attachInterrupt(this->PIN.encoder[0], INT_ENCODER, CHANGE); 
    attachInterrupt(this->PIN.encoder[1], INT_ENCODER, CHANGE);
    attachInterrupt(this->PIN.encoder[2], INT_ENCODER, CHANGE);
    p_THIS_HAL_REV_A = this;
    //p_ld1-3
    pinMode(this->PIN.led[0], OUTPUT);
    pinMode(this->PIN.led[1], OUTPUT);
    pinMode(this->PIN.led[2], OUTPUT);    
    //P_BUZZER
    pinMode(this->PIN.buzzer, OUTPUT);
    //p_oen
    pinMode(this->PIN.p_oen, OUTPUT);
    //INT
    pinMode(this->PIN.INT, INPUT_PULLUP);
    attachInterrupt(this->PIN.INT, INT_callBack, FALLING);       
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
    //I2C
    Wire.begin();
}

void HAL_MCU11_revA::mapEncoder(RotaryEncoder* P_ENCODER)
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

void HAL_MCU11_revA::tick()
{  
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
        analogWrite(this->PIN.led[0], this->p_ld1->halCallback().value);
    }
    //LD_COMMUNACTION_STATE
    if(this->p_ld2->isThereANewPortValue())
    {
        analogWrite(this->PIN.led[1], this->p_ld2->halCallback().value);
    }
    //p_ld3
    if(this->p_ld3->isThereANewPortValue())
    {
        analogWrite(this->PIN.led[2], this->p_ld3->halCallback().value);  
    }    
}

void HAL_MCU11_revA::readEncoder()
{
    this->p_encoder->A.halCallback(digitalRead(this->PIN.encoder[0]));
    this->p_encoder->B.halCallback(digitalRead(this->PIN.encoder[1]));
    this->p_encoder->pushButton.halCallback(digitalRead(this->PIN.encoder[2]));
}