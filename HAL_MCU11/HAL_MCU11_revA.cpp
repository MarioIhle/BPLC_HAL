#include "HAL_MCU11_revA.h"

HAL_MCU11_revA::HAL_MCU11_revA()
{}
    
void HAL_MCU11_revA::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->PIN.encoder[0], INPUT);
    pinMode(this->PIN.encoder[1], INPUT);
    pinMode(this->PIN.encoder[2], INPUT);
    ENCODER.begin(&Encoder_A, &Encoder_B, &Encoder_Z);
    //LD_DEVICE_STATE-3
    pinMode(this->PIN.led[0], OUTPUT);
    this->LD_DEVICE_STATE.begin(255);
    pinMode(this->PIN.led[1], OUTPUT);
    this->LD_COMMUNICATION_STATE.begin(255);
    pinMode(this->PIN.led[2], OUTPUT);    
    this->LD_ERROR_OUT.begin(255);    
    //BUZZER
    pinMode(this->PIN.buzzer, OUTPUT);
    this->BUZZER.begin(50);
    //OEN
    this->OEN.begin(true);    
    pinMode(this->PIN.OEN, OUTPUT);
    //INT
    pinMode(this->PIN.INT, INPUT_PULLUP);
    attachInterrupt(this->PIN.INT, INT_callBack, CHANGE);   
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
    //I2C
    Wire.begin();
}

void HAL_MCU11_revA::tick()
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
    //LD_DEVICE_STATE
    if(this->LD_DEVICE_STATE.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[0], this->LD_DEVICE_STATE.getValue().value);
    }
    //LD_COMMUNACTION_STATE
    if(this->LD_COMMUNICATION_STATE.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[1], this->LD_COMMUNICATION_STATE.getValue().value);
    }
    //LD_ERROR_OUT
    if(this->LD_ERROR_OUT.isThereANewPortValue())
    {
        analogWrite(this->PIN.led[2], this->LD_ERROR_OUT.getValue().value);  
    }      
}