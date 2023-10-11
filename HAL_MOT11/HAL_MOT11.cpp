#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11   ()
{}

HAL_MOT11::HAL_MOT11   (e_MOT11_ADDRESS_t ADDRESS)
{
    this->i2c_address = ADDRESS;
}

void HAL_MOT11::begin()
{
    this->to_Heartbeat.setInterval(5000);
    this->to_I2C.setInterval(50);
}

void HAL_MOT11::begin (e_MOT11_ADDRESS_t ADDRESS)
{
    this->i2c_address = ADDRESS;
    this->to_Heartbeat.setInterval(5000);
    this->to_I2C.setInterval(50);
}

void HAL_MOT11::tick()
{
    //Über Heartbeat wird zyklisch Errors und Stromaufnahme abgefragt
    if(this->to_Heartbeat.check())
    {
        this->sendHeartbeat(); 
        this->to_Heartbeat.reset();
    }    

    switch(this->driveState)
    {
        case driveState_waitForStart:            
        break;

        case driveState_start:
            this->motParams.direction = this->motParams.old.direction;
            this->motParams.speed     = this->motParams.old.speed;
            this->sendDriveCommand();                
        break;

        case driveState_runningWithParameters:   //Normalbetreb
            const bool DRIVE_DIRECTION_CHANGED = (bool)(this->motParams.old.direction != this->motParams.direction);
            const bool DRIVE_SPEED_CHANGED     = (bool)(this->motParams.old.speed != this->motParams.speed);
        
            if(DRIVE_DIRECTION_CHANGED || DRIVE_SPEED_CHANGED)
            {
                this->sendDriveCommand();
                this->motParams.old.direction = this->motParams.direction;
                this->motParams.old.speed     = this->motParams.speed;
            }
        break;

        case driveState_stop:        
            this->motParams.speed     = 0;
            this->motParams.direction = idle;     
            this->sendDriveCommand();   //Stop senden
            this->driveState = driveState_waitForStart;
        break;

        case driveState_stopAndBreak:
            this->motParams.speed     = 255;
            this->motParams.direction = idle; 
            this->sendDriveCommand();  
            this->driveState = driveState_waitForStart;
        break;

    }
    
}
//Nur Stop senden, danach aber wierder letzte Parameter laden, bei Start diese senden
void HAL_MOT11::stop()
{
    this->driveState = driveState_stop;
}
//Nur Temporärer Stop und EMI bremse aktivieren senden, bei start wieder mit letzten Parameter anlaufen
void HAL_MOT11::stopAndBreak()
{
    this->driveState = driveState_stopAndBreak;
}
//Nur Temporärer Stop senden, bei start wieder mit letzten Parameter anlaufen
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

e_motError_t HAL_MOT11::getError()
{
    return this->error.code;
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
#ifdef DEBUG_HAL_MOT11
Serial.println("ACK empfangen");
#endif    
    }   r
    else
    {
#ifdef DEBUG_HAL_MOT11
Serial.println("kein ACK empfangen");
#endif 
    } 
}

void HAL_MOT11::sendHeartbeat()
{
    Wire.requestFrom(this->i2c_address, sizeof(u_mot11_i2c_payload_t));
    
    if(this->waitForHeartbeat())
    {
        Serial.println("Heartbeat empfangen");
    }   
    else
    {
        Serial.println("kein Heartbeat empfangen");
    } 
}

void HAL_MOT11::sendFrame(const u_mot11_i2c_payload_t COMMAND)
{
    Wire.beginTransmission(this->i2c_address);   

    for(uint8_t bytes; bytes < sizeof(u_mot11_i2c_payload_t); bytes++)
    {
        //Serial.println(COMMAND.data[bytes]);
        Wire.write(COMMAND.data[bytes]);
    }                

    Wire.endTransmission();    
}

bool HAL_MOT11::waitForACK()
{
    uint8_t inByte      = NAK;
    
    Wire.requestFrom(this->i2c_address, 1); 

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

bool HAL_MOT11::waitForHeartbeat()
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
        if(inCommand.extract.key == mot11_i2c_key__heartbeat)
        {
            break;
        }     
    }

#ifdef DEBUG_HAL_MOT11 
Serial.println("Heartbeat info:");
Serial.print("KEY: ");        Serial.println(inCommand.extract.key);
Serial.print("DIRECTION: ");  Serial.println(inCommand.extract.direction);
Serial.print("SPEED: ");      Serial.println(inCommand.extract.speed);
Serial.print("ERROR: ");      Serial.println(inCommand.extract.error);
Serial.print("CURRENT: ");    Serial.println(inCommand.extract.current);
Serial.println("");
#endif
    return (bool)(inCommand.extract.key == mot11_i2c_key__heartbeat);
}