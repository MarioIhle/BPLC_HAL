#include "HAL_MCU11.h"

HAL_MCU11::HAL_MCU11()
{}
    
void HAL_MCU11::begin()
{
    pinMode(this->pins.encoder[0], INPUT);
    pinMode(this->pins.encoder[1], INPUT);
    pinMode(this->pins.encoder[2], INPUT);

    pinMode(this->pins.led[0], OUTPUT);
    pinMode(this->pins.led[1], OUTPUT);
    pinMode(this->pins.led[2], OUTPUT);

    pinMode(this->pins.OEN, OUTPUT);
    pinMode(this->pins.INT, INPUT);
    pinMode(this->pins.buzzer, OUTPUT);

    //Serial.begin(this->baudrate.USB);       //USB
    //Serial1.begin(this->baudrate.RS232);    //RS232
    //Serial2.begin(this->baudrate.RS485);    //RS485
}

void HAL_MCU11::tick()
{  
    //Encoder lesen
    for(uint8_t pin = 0; pin < ENCODER_PIN__COUNT; pin++)
    {
        this->ioState.encoder[pin].previousState = this->ioState.encoder[pin].state;
        this->ioState.encoder[pin].state = digitalRead(this->pins.encoder[pin]);        
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

    //Buzzer
    if(this->buzzer.beepCount < this->buzzer.beeps_requested && this->buzzer.to_buzzer.check())
    {
        this->buzzer.state = !this->buzzer.state;
        if(this->buzzer.state)
        {
            analogWrite(this->pins.buzzer, 50);
            this->buzzer.beepCount++;
        }
        else
        {
            analogWrite(this->pins.buzzer, 0);
        }

        this->buzzer.to_buzzer.reset();        
    }
    else if(this->buzzer.beepCount == this->buzzer.beeps_requested)
    {
        analogWrite(this->pins.buzzer, 0);
        this->buzzer.state = false;
    }

}

e_direction_t_MCU HAL_MCU11::getEncoderDirection()
{
    const bool NEGATIVE_FLANK_A = (bool)(this->ioState.encoder[ENCODER_PIN__A].state == false   && this->ioState.encoder[ENCODER_PIN__A].previousState == true);
    const bool NEGATIVE_FLANK_B = (bool)(this->ioState.encoder[ENCODER_PIN__B].state == true    && this->ioState.encoder[ENCODER_PIN__B].previousState == false);

    e_direction_t_MCU direction = MCU_idle;    

    if(NEGATIVE_FLANK_A && this->ioState.encoder[ENCODER_PIN__B].state == true)
    {
        direction = MCU_right;
    }
    else if(NEGATIVE_FLANK_B && this->ioState.encoder[ENCODER_PIN__A].state == true)
    {
        direction = MCU_left;
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

void HAL_MCU11::beep(const uint8_t BEEPS, const int INTERVAL)
{
    this->buzzer.beeps_requested = BEEPS+1;
    this->buzzer.to_buzzer.setInterval(INTERVAL);
    this->buzzer.beepCount = 0;    
}