#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11()
{}

HAL_MOT11::HAL_MOT11(const e_MOT11_ADDRESS_t ADDRESS)
{
    this->deviceAddress = ADDRESS;
}

e_APP_ERROR_t HAL_MOT11::begin()
{
    e_APP_ERROR_t error = APP_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup MOT11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case MOT11_CARD_1:
            Serial.println("1");
        break;
        case MOT11_CARD_2:
            Serial.println("2");
        break;
        case MOT11_CARD_3:
            Serial.println("3");
        break;
        case MOT11_CARD_4:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = APP_ERROR__MOT11_COMMUNICATION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(error == APP_ERROR__NO_ERROR)
    {   
        //Timeouts
        this->to_parameterPoll.setInterval(1000);
        this->to_I2C.setInterval(50);  
        
        //Errorout
        this->error.i2cError.countLimit = 3;
        this->error.i2cError.count      = 0;
        this->error.code                = motError_noError;
        //Statusmaschine
        this->driveState = driveState_start;      
    }
    else
    {
        this->error.code = motError_i2cConnectionFailed;                   
    }

    return error;
}

e_APP_ERROR_t HAL_MOT11::begin(const e_MOT11_ADDRESS_t ADDRESS)
{
    e_APP_ERROR_t error = APP_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup MOT11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case MOT11_CARD_1:
            Serial.println("1");
        break;
        case MOT11_CARD_2:
            Serial.println("2");
        break;
        case MOT11_CARD_3:
            Serial.println("3");
        break;
        case MOT11_CARD_4:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = APP_ERROR__MOT11_COMMUNICATION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(error == APP_ERROR__NO_ERROR)
    {   
        //I2C Address
        this->deviceAddress = ADDRESS;
        //Timeouts
        this->to_parameterPoll.setInterval(1000);
        this->to_I2C.setInterval(50);  
        
        //Errorout
        this->error.i2cError.countLimit = 3;
        this->error.i2cError.count      = 0;
        this->error.code                = motError_noError;
        //Statusmaschine
        this->driveState = driveState_start;      
    }
    else
    {
        this->error.code = motError_i2cConnectionFailed;             
    }

    return error;
}

void HAL_MOT11::tick()
{
    //I2C Error check
    const bool I2C_ERROR_COUNT_LIMIT_REACHED = (bool)(this->error.i2cError.count >= this->error.i2cError.countLimit);
    if(I2C_ERROR_COUNT_LIMIT_REACHED)
    {
        this->error.code = motError_i2cConnectionFailed;

        #ifdef DEBUG_HAL_MOT11
        Serial.println("count limit reachded");
        #endif
    }
    //Error behandlung
    if(this->error.code != motError_noError)
    {
        this->driveState = driveState_safeState;

        #ifdef DEBUG_HAL_MOT11
        Serial.println("go to safe state");
        #endif
    }

    switch(this->driveState)
    {
        case driveState_waitForStart:  
        case driveState_safeState:          
        break;

        case driveState_start:
            this->motParams.direction = this->motParams.old.direction;
            this->motParams.speed     = this->motParams.old.speed;
            this->sendDriveCommand(); 

            this->driveState = driveState_runningWithParameters;               
        break;

        case driveState_runningWithParameters:   //Normalbetreb
            if((this->motParams.old.direction != this->motParams.direction) || (this->motParams.old.speed != this->motParams.speed))
            {
                this->sendDriveCommand();
                this->motParams.old.direction = this->motParams.direction;
                this->motParams.old.speed     = this->motParams.speed;
            }
            //Über Request wird zyklisch alle live Parameter abgefragt
            if(this->to_parameterPoll.check())
            {
                this->requestDriveParameter(); 
                this->to_parameterPoll.reset();
            }  
        break;

        case driveState_stop:        
            this->motParams.speed     = 0;
            this->motParams.direction = movement_idle;     
            this->sendDriveCommand();   //Stop senden

            this->driveState = driveState_waitForStart;      
        break;

        case driveState_stopAndBreak:
            this->motParams.speed     = 255;
            this->motParams.direction = movement_idle; 
            this->sendDriveCommand();  

            this->driveState = driveState_waitForStart;
        break;

    }
}
//Nur Stop senden, letzte parameter aber speichern
void HAL_MOT11::stop()
{
    this->driveState = driveState_stop;
}
//Nur Stop und EMI bremse aktivieren senden, letzte parameter aber speichern
void HAL_MOT11::stopAndBreak()
{
    this->driveState = driveState_stopAndBreak;
}
//Nach Stopp, wieder mit letzten Parameter anlaufen
void HAL_MOT11::start()
{
    this->driveState = driveState_start;
}

void HAL_MOT11::setSpeed(const uint8_t SPEED)
{   
    this->motParams.speed = SPEED;
}

void HAL_MOT11::setDirection(const e_movement_t DIRECTION)
{
    this->motParams.direction = DIRECTION;    
}

void HAL_MOT11::setDirectionAndSpeed(const e_movement_t DIRECTION, const uint8_t SPEED)
{
    this->motParams.direction = DIRECTION; 
    this->motParams.speed     = SPEED;   
}

e_APP_ERROR_t HAL_MOT11::getError()
{
    e_APP_ERROR_t tempError = APP_ERROR__NO_ERROR;

    switch(this->error.code)
    {
        case motError_noError:
            tempError = APP_ERROR__NO_ERROR;
        break;

        case motError_i2cConnectionFailed:
            tempError = APP_ERROR__MOT11_COMMUNICATION_FAILED;
        break;

        case motError_overcurrent:
            tempError = APP_ERROR__MOT11_OVER_CURRENT;
        break;

        case motError_overtemperature:
            tempError = APP_ERROR__MOT11_OVER_TEMPERATURE;
        break;

        case motError_OEN_disabled: //kein Error für MCU an sich
            tempError = APP_ERROR__NO_ERROR;
        break;

        case motError_notTeached:
            tempError = APP_ERROR__MOT11_CURRENT_NOT_TEACHED;
        break;
    }

    Serial.print("error: "); Serial.println(tempError);
    return tempError;
}

float HAL_MOT11::getCurrent()
{
    return this->motParams.current;
}

e_movement_t HAL_MOT11::getDirection()
{
    return this->motParams.direction;
}

uint8_t HAL_MOT11::getSpeed()
{
    return this->motParams.speed;
}

void HAL_MOT11::sendDriveCommand()
{  
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));

    COMMAND.extract.key         = mot11_i2c_key__driveCommand;
    COMMAND.extract.direction   = this->motParams.direction;
    COMMAND.extract.speed       = this->motParams.speed;

    this->sendFrame(COMMAND);    

    if(this->waitForACK())
    {
        this->error.i2cError.count = 0;

#ifdef DEBUG_HAL_MOT11
Serial.println("ACK empfangen");
#endif    
    }   
    else
    {
        this->error.i2cError.count++;

#ifdef DEBUG_HAL_MOT11
Serial.println("kein ACK empfangen");
#endif 
    } 
}

void HAL_MOT11::requestDriveParameter()
{
    Wire.requestFrom(this->deviceAddress, sizeof(u_mot11_i2c_payload_t));
    
    if(this->waitForDriveParameter())
    {
        this->error.i2cError.count = 0;

#ifdef DEBUG_HAL_MOT11
Serial.println("Drive Parameter empfangen");
#endif
    }   
    else
    {
        this->error.i2cError.count++;

#ifdef DEBUG_HAL_MOT11
Serial.println("keine Drive Parameter empfangen");
#endif
    } 
}

void HAL_MOT11::sendFrame(const u_mot11_i2c_payload_t COMMAND)
{
    Wire.beginTransmission(this->deviceAddress);   

    for(uint8_t bytes; bytes < sizeof(u_mot11_i2c_payload_t); bytes++)
    {
        //Serial.println(COMMAND.data[bytes]);
        Wire.write(COMMAND.data[bytes]);
    }                

    Wire.endTransmission();    
}

bool HAL_MOT11::waitForACK()
{
    uint8_t inByte = NAK;
    
    Wire.requestFrom(this->deviceAddress, 1); 

    this->to_I2C.reset();  

    while(this->to_I2C.check() == false)
    {   
        while(Wire.available()) 
        {       
            inByte = Wire.read();    
        }
    	//Fast exit wenn Kommando empfangen 
        if(inByte == ACK)
        {
            break;
        }  
    }

    return (bool)(inByte == ACK);
}

bool HAL_MOT11::waitForDriveParameter()
{
    u_mot11_i2c_payload_t inCommand;
    inCommand.extract.key = mot11_i2c_key__count;

    this->to_I2C.reset();      

    while(this->to_I2C.check() == false)  
    {     
        while(Wire.available()) 
        {  
            for(uint8_t i=0; i<sizeof(u_mot11_i2c_payload_t); i++)
            {     
                //Serial.print(Wire.peek());
                inCommand.data[i] = Wire.read();   
            }   
        } 
        //Fast exit wenn Kommando empfangen  
        if(inCommand.extract.key == mot11_i2c_key__getDriveState)
        {
            break;
        }     
    }

    //Empfangenen Errorcode auswerten, wenn plausibel
    const bool RECEIVED_ERROR_CODE_PLAUSIBLE = (bool)(inCommand.extract.error < motError_count);

    if(RECEIVED_ERROR_CODE_PLAUSIBLE)
    {
        this->error.code = (e_motError_t)inCommand.extract.error;
    }

#ifdef DEBUG_HAL_MOT11 
Serial.println("Drive Parameter:");
Serial.print("KEY: ");        Serial.println(inCommand.extract.key);
Serial.print("DIRECTION: ");  Serial.println(inCommand.extract.direction);
Serial.print("SPEED: ");      Serial.println(inCommand.extract.speed);
Serial.print("ERROR: ");      Serial.println(inCommand.extract.error);
Serial.print("CURRENT: ");    Serial.println(inCommand.extract.current);
Serial.println("");
#endif
    return (bool)(inCommand.extract.key == mot11_i2c_key__getDriveState);
}