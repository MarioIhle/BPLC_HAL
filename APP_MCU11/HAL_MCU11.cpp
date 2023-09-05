#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin(void (*INT_callBack)(void))
{
    //encoder
    pinMode(this->pins.encoder[0], INPUT);
    pinMode(this->pins.encoder[1], INPUT);
    pinMode(this->pins.encoder[2], INPUT);
    //LD1-3
    pinMode(this->pins.led[0], OUTPUT);
    this->LD1.begin(255);

    pinMode(this->pins.led[1], OUTPUT);
    this->LD2.begin(255);

    pinMode(this->pins.led[2], OUTPUT);    
    this->LD3.begin(255);
    
    //buzzer
    pinMode(this->pins.buzzer, OUTPUT);
    this->BUZZER.begin(50);

    //OEN
    pinMode(this->pins.OEN, OUTPUT);
    //INT
    pinMode(this->pins.INT, INPUT);
    attachInterrupt(this->pins.INT, INT_callBack, FALLING);
   
    //Serielle Schnittstellen
    Serial.begin(this->baudrate.USB);       //USB
    Serial1.begin(this->baudrate.RS232);    //RS232
    Serial2.begin(this->baudrate.RS485);    //RS485
}

void HAL_MCU11::tick()
{  
    //Encoder lesen
    for(uint8_t pin = 0; pin < ENCODER_PIN__COUNT; pin++)
    {
        this->ioState.encoder[pin].previousState = this->ioState.encoder[pin].state;
        this->ioState.encoder[pin].state = digitalRead(this->pins.encoder[pin]);        
    }

    //OEN schreiben
    digitalWrite(this->pins.OEN, this->ioState.OEN);

    //INT lesen
    this->ioState.INT = (this->pins.INT);

    //buzzer
    this->BUZZER.tick();
    analogWrite(this->pins.buzzer, this->BUZZER.getValue());

    //LD1-3
    this->LD1.tick();
    this->LD2.tick();
    this->LD3.tick();
    analogWrite(this->pins.led[0], this->LD1.getValue());
    analogWrite(this->pins.led[1], this->LD2.getValue());
    analogWrite(this->pins.led[2], this->LD3.getValue());  
}

e_direction_t HAL_MCU11::getEncoderDirection()
{
    const bool NEGATIVE_FLANK_A = (bool)(this->ioState.encoder[ENCODER_PIN__A].state == false   && this->ioState.encoder[ENCODER_PIN__A].previousState == true);
    const bool NEGATIVE_FLANK_B = (bool)(this->ioState.encoder[ENCODER_PIN__B].state == false    && this->ioState.encoder[ENCODER_PIN__B].previousState == true);

    e_direction_t direction = idle;    

    if(NEGATIVE_FLANK_A && this->ioState.encoder[ENCODER_PIN__B].state == false)
    {
        direction = left;
    }
    else if(NEGATIVE_FLANK_B && this->ioState.encoder[ENCODER_PIN__A].state == false)
    {
        direction = right;
    }

    return direction;
}

bool HAL_MCU11::isEncoderButtonPressed()
{
    return (bool)(this->ioState.encoder[ENCODER_PIN__Z].state == false && this->ioState.encoder[ENCODER_PIN__Z].previousState == true);
}

void HAL_MCU11::setOEN(const bool STATE)
{
    digitalWrite(this->pins.OEN, this->ioState.OEN);
}

bool HAL_MCU11::getINT()
{
    return this->ioState.INT = digitalRead(this->pins.INT);
}