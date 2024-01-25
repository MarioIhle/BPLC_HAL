#include "HAL_MCU11.h"

//Callback für Hardware Interrupt 
uint8_t* p_INT_count_HAL_REV_B;

static void INT_ISR()
{
   *p_INT_count_HAL_REV_B = 2;
}

HAL_MCU11_revB::HAL_MCU11_revB()
{}
    
void HAL_MCU11_revB::begin()
{
    //encoder
    pinMode(this->PIN.encoder_A, INPUT);
    pinMode(this->PIN.encoder_B, INPUT);
    pinMode(this->PIN.encoder_Z, INPUT); 
    //p_ld1-3
    pinMode(this->PIN.ld1, OUTPUT);
    pinMode(this->PIN.ld2, OUTPUT);
    pinMode(this->PIN.ld3, OUTPUT);     
    //p_buzzer
    pinMode(this->PIN.buzzer, OUTPUT);
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
}

void HAL_MCU11_revB::mapEncoder(RotaryEncoder* P_ENCODER)
{
    this->p_encoder = P_ENCODER;
}

void HAL_MCU11_revB::mapBuzzer(Output* P_BUZZER_OBJECT)
{
    this->p_buzzer = P_BUZZER_OBJECT;
}

void HAL_MCU11_revB::mapLD1(Output* P_LD1_OBJECT)
{
    this->p_ld1 = P_LD1_OBJECT;
}

void HAL_MCU11_revB::mapLD2(Output* P_LD2_OBJECT)
{
    this->p_ld2 = P_LD2_OBJECT;
}

void HAL_MCU11_revB::mapLD3(Output* P_LD3_OBJECT)
{
    this->p_ld3 = P_LD3_OBJECT;
}

void HAL_MCU11_revB::mapOEN(Output* P_OEN_OBJECT)
{
    this->p_oen = P_OEN_OBJECT;
}

void HAL_MCU11_revB::mapINT(uint8_t* P_INT_COUNT)
{
    p_INT_count_HAL_REV_B = P_INT_COUNT;
}

void HAL_MCU11_revB::tick()
{  
    //encoder
    this->p_encoder->halCallback(digitalRead(this->PIN.encoder_A), digitalRead(this->PIN.encoder_B), digitalRead(this->PIN.encoder_Z));
    //p_oen schreiben
    if(this->p_oen->isThereANewPortValue())
    {
        digitalWrite(this->PIN.OEN, this->p_oen->halCallback().value);
    }
    //buzzer
    if(this->p_buzzer->isThereANewPortValue())
    {
        analogWrite(this->PIN.buzzer, this->p_buzzer->halCallback().value);
    }
    //p_ld1
    if(this->p_ld1->isThereANewPortValue())
    {
        analogWrite(this->PIN.ld2, this->p_ld1->halCallback().value);
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