#include "HAL_MCU11.h"

HAL_MCU11_revB::HAL_MCU11_revB()
{}
    
void HAL_MCU11_revB::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->PIN.encoder[0], INPUT);
    pinMode(this->PIN.encoder[1], INPUT);
    pinMode(this->PIN.encoder[2], INPUT);
    //P_LD1-3
    pinMode(this->PIN.led[0], OUTPUT);
    pinMode(this->PIN.led[1], OUTPUT);
    pinMode(this->PIN.led[2], OUTPUT);    
    //P_BUZZER
    pinMode(this->PIN.buzzer, OUTPUT);
    //P_OEN
    pinMode(this->PIN.P_OEN, OUTPUT);
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

void HAL_MCU11_revB::mapEncoder(DigitalInput* P_PORT_A, DigitalInput* P_PORT_B, DigitalInput* P_PORT_PUSHBUTTON)
{
    this->P_Encoder_A = P_PORT_A;
    this->P_Encoder_B = P_PORT_B;
    this->P_Encoder_Z = P_PORT_PUSHBUTTON;
}

void HAL_MCU11_revB::mapBuzzer(Output* P_BUZZER_OBJECT)
{
    this->P_BUZZER = P_BUZZER_OBJECT;
}

void HAL_MCU11_revB:: mapLD1(Output* P_LD1_OBJECT)
{
    this->P_LD1 = P_LD1_OBJECT;
}

void HAL_MCU11_revB::mapLD2(Output* P_LD2_OBJECT)
{
    this->P_LD2 = P_LD2_OBJECT;
}

void HAL_MCU11_revB::mapLD3(Output* P_LD3_OBJECT)
{
    this->P_LD3 = P_LD3_OBJECT;
}

void HAL_MCU11_revB::mapOEN(Output* P_OEN_OBJECT)
{
    this->P_OEN = P_OEN_OBJECT;
}

void HAL_MCU11_revB::tick()
{  
    //Encoder lesen
    this->P_Encoder_A->setPortState(digitalRead(this->PIN.encoder[0]));
    this->P_Encoder_B->setPortState(digitalRead(this->PIN.encoder[1]));
    this->P_Encoder_Z->setPortState(digitalRead(this->PIN.encoder[2]));
    //P_OEN schreiben
    if(this->P_OEN->isThereANewPortValue())
    {
        digitalWrite(this->PIN.P_OEN, this->P_OEN->getValue().value);
    }
    //buzzer
    if(this->P_BUZZER->isThereANewPortValue())
    {
        analogWrite(this->PIN.buzzer, this->P_BUZZER->getValue().value);
    }
    //P_LD1
    if(this->P_LD1->isThereANewPortValue())
    {
        analogWrite(this->PIN.led[0], this->P_LD1->getValue().value);
    }
    //LD_COMMUNACTION_STATE
    if(this->P_LD2->isThereANewPortValue())
    {
        analogWrite(this->PIN.led[1], this->P_LD2->getValue().value);
    }
    //P_LD3
    if(this->P_LD3->isThereANewPortValue())
    {
        analogWrite(this->PIN.led[2], this->P_LD3->getValue().value);  
    }      
}