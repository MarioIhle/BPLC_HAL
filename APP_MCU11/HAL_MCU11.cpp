#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->pins.encoder[0], INPUT);
    pinMode(this->pins.encoder[1], INPUT);
    pinMode(this->pins.encoder[2], INPUT);
    ENCODER.begin(&Encoder_A, &Encoder_B, &Encoder_Z);
    //LD1-3
    pinMode(this->pins.led[0], OUTPUT);
    this->LD1.begin(255);

    pinMode(this->pins.led[1], OUTPUT);
    this->LD2.begin(255);

    pinMode(this->pins.led[2], OUTPUT);    
    this->LD3.begin(255);
    
    //BUZZER
    pinMode(this->pins.buzzer, OUTPUT);
    this->BUZZER.begin(50);

    //OEN
    this->OEN.begin(true);    
    pinMode(this->pins.OEN, OUTPUT);
    //INT
    pinMode(this->pins.INT, INPUT);
    attachInterrupt(this->pins.INT, INT_callBack, CHANGE);
   
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
}

void HAL_MCU11::tick()
{  
    s_portValue_t TEMP_PORT_VALUE;
    //Encoder lesen
    Encoder_A.setPortState(digitalRead(this->pins.encoder[0]));
    Encoder_B.setPortState(digitalRead(this->pins.encoder[1]));
    Encoder_Z.setPortState(digitalRead(this->pins.encoder[2]));

    //OEN schreiben
    OEN.tick();
    TEMP_PORT_VALUE = this->OEN.getValue();
    digitalWrite(this->pins.OEN, TEMP_PORT_VALUE.value);

    //buzzer
    this->BUZZER.tick();
    TEMP_PORT_VALUE = this->BUZZER.getValue();
    analogWrite(this->pins.buzzer, TEMP_PORT_VALUE.value);

    //LD1-3
    this->LD1.tick();
    TEMP_PORT_VALUE = this->LD1.getValue();
    analogWrite(this->pins.led[0], TEMP_PORT_VALUE.value);

    this->LD2.tick();
    TEMP_PORT_VALUE = this->LD1.getValue();
    analogWrite(this->pins.led[1], TEMP_PORT_VALUE.value);

    this->LD3.tick();
    TEMP_PORT_VALUE = this->LD1.getValue();    
    analogWrite(this->pins.led[2], TEMP_PORT_VALUE.value);  
}