#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->PIN.encoder[0], INPUT);
    pinMode(this->PIN.encoder[1], INPUT);
    pinMode(this->PIN.encoder[2], INPUT);
    ENCODER.begin(&Encoder_A, &Encoder_B, &Encoder_Z);
    //LD1-3
    pinMode(this->PIN.led[0], OUTPUT);
    this->LD1.begin(255);
    pinMode(this->PIN.led[1], OUTPUT);
    this->LD2.begin(255);
    pinMode(this->PIN.led[2], OUTPUT);    
    this->LD3.begin(255);    
    //BUZZER
    pinMode(this->PIN.buzzer, OUTPUT);
    this->BUZZER.begin(150);
    //OEN
    this->OEN.begin(true);    
    pinMode(this->PIN.OEN, OUTPUT);
    //INT
    pinMode(this->PIN.INT, INPUT);
    attachInterrupt(this->PIN.INT, INT_callBack, CHANGE);   
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
}

void HAL_MCU11::tick()
{  
    //Encoder lesen
    Encoder_A.setPortState(digitalRead(this->PIN.encoder[0]));
    Encoder_B.setPortState(digitalRead(this->PIN.encoder[1]));
    Encoder_Z.setPortState(digitalRead(this->PIN.encoder[2]));
    //OEN schreiben
    if(this->OEN.isThereANewPortValue())
    {
        digitalWrite(this->PIN.OEN, this->OEN.getValue().value);
    }
    //buzzer
    if(this->BUZZER.isThereANewPortValue())
    {
        analogWrite(this->PIN.buzzer, this->BUZZER.getValue().value);
    }
    //LD1-3
    if(this->LD1.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[0], this->LD1.getValue().value);
    }
    if(this->LD2.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[1], this->LD2.getValue().value);
    }
    if(this->LD3.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[2], this->LD3.getValue().value);  
    }      
}