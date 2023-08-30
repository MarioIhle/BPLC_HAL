#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin()
{
    pinMode(this->pins.encoder.A, INPUT);
    pinMode(this->pins.encoder.B, INPUT);
    pinMode(this->pins.encoder.Z, INPUT);

    pinMode(this->pins.led.LD1, OUTPUT);
    pinMode(this->pins.led.LD2, OUTPUT);
    pinMode(this->pins.led.LD3, OUTPUT);

    pinMode(this->pins.OEN, OUTPUT);
    pinMode(this->pins.INT, INPUT);

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
        this->ioState.encoder[pin] = digitalRead(this->pins.encoder[pins]);  
    }

    //LEDs schreiben
    for(uint8_t led = 0; led < ENCODER_PIN__COUNT; led++)
    {
        digitalWrite(this->pins.led[led], this->ioState.led[led]);
    }
    //OEN schreiben
    digitalWrite(this->pins.OEN, this->ioState.OEN);

    //INT lesen
    this->ioState.INT = (this->pins.INT);
}

e_direction_t HAL_MCU11::getEncoderDirection()
{
    const bool NEGATIVE_FLANK_A = (bool)(this->ioState.encoder.A.state == false && this->ioState.encoder.A.previousState == true);
    const bool NEGATIVE_FLANK_B = (bool)(this->ioState.encoder.B == true && this->ioState.encoder.B.previousState == false);

    e_direction_t direction = idle;    

    if(NEGATIVE_FLANK_A && this->ioState.encoder.B == true)
    {
        direction = right;
    }
    else if(NEGATIVE_FLANK_B && this->ioState.encoder.A == true)
    {
        direction = left;
    }

    return direction;
}

bool HAL_MCU11::isEncoderButtonPressed()
{
    return this->ioState.encoder.Z;
}

void HAL_MCU11::setOEN(const bool STATE)
{
    digitalWrite(this->pins.OEN, this->ioState.OEN);
}

bool HAL_MCU11::getINT()
{
    return this->ioState.INT = digitalRead(this->pins.INT);
}
