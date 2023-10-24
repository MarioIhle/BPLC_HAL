#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->PINS.encoder[0], INPUT);
    pinMode(this->PINS.encoder[1], INPUT);
    pinMode(this->PINS.encoder[2], INPUT);
    ENCODER.begin(&Encoder_A, &Encoder_B, &Encoder_Z);
    //LD1-3
    pinMode(this->PINS.led[0], OUTPUT);
    this->LD1.begin(255);
    pinMode(this->PINS.led[1], OUTPUT);
    this->LD2.begin(255);
    pinMode(this->PINS.led[2], OUTPUT);    
    this->LD3.begin(255);    
    //BUZZER
    pinMode(this->PINS.buzzer, OUTPUT);
    this->BUZZER.begin(150);
    //OEN
    this->OEN.begin(true);    
    pinMode(this->PINS.OEN, OUTPUT);
    //INT
    pinMode(this->PINS.INT, INPUT);
    attachInterrupt(this->PINS.INT, INT_callBack, CHANGE);   
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
}

void HAL_MCU11::tick()
{  
    //Encoder lesen
    Encoder_A.setPortState(digitalRead(this->PINS.encoder[0]));
    Encoder_B.setPortState(digitalRead(this->PINS.encoder[1]));
    Encoder_Z.setPortState(digitalRead(this->PINS.encoder[2]));
    //OEN schreiben
    if(this->OEN.isThereANewPortValue())
    {
        digitalWrite(this->PINS.OEN, this->OEN.getValue().value);
    }
    //buzzer
    if(this->BUZZER.isThereANewPortValue())
    {
        analogWrite(this->PINS.buzzer, this->BUZZER.getValue().value);
    }
    //LD1-3
    if(this->LD1.isThereANewPortValue())
    {
        analogWrite(this->PINS.led[0], this->LD1.getValue().value);
    }
    if(this->LD2.isThereANewPortValue())
    {
        analogWrite(this->PINS.led[1], this->LD2.getValue().value);
    }
    if(this->LD3.isThereANewPortValue())
    {
        analogWrite(this->PINS.led[2], this->LD3.getValue().value);  
    }      
}