#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11()
{}

void HAL_MOT11::begin(const e_MOT11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress             = I2C_ADDRESS;
    this->error.code                = BPLC_ERROR__NO_ERROR;
    this->error.i2cError.countLimit = 3;
    this->error.i2cError.count      = 0;
 
    this->to_parameterPoll.setInterval(1000);
    this->to_I2C.setInterval(50);                 

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup MOT11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case MOT11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case MOT11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case MOT11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case MOT11_CARD_4_ADDRESS:
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
        this->error.code = MOT11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren
    if(this->error.code == BPLC_ERROR__NO_ERROR)
    {         
        this->deviceState = deviceState_running;      
    }
    else
    {
        this->deviceState = deviceState_safeState;
    }
}

void HAL_MOT11::mapObjectToPort(MOTOR* P_OBJECT)
{
    if(this->ports.used == PORT_USEAGE__NOT_IN_USE)
    {
        this->ports.p_object = P_OBJECT;
        this->ports.used     = PORT_USEAGE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->error.code = MOT11_ERROR__PORT_ALREADY_DEFINED;
    }
}

void HAL_MOT11::tick()
{
    //I2C Error check
    const bool I2C_ERROR_COUNT_LIMIT_REACHED = (bool)(this->error.i2cError.count >= this->error.i2cError.countLimit);
    if(I2C_ERROR_COUNT_LIMIT_REACHED)
    {
        this->error.code = MOT11_ERROR__I2C_CONNECTION_FAILED;

        #ifdef DEBUG_HAL_MOT11
        Serial.println("error count limit reachded");
        #endif
    }
    //Error behandlung
    if(this->error.code != BPLC_ERROR__NO_ERROR)
    {
        this->deviceState = deviceState_safeState;
    }

    if(this->error.code != MOT11_ERROR__I2C_CONNECTION_FAILED)
    {
        if(this->ports.used == PORT_USEAGE__MAPPED_TO_OBJECT)
        {
            switch(this->deviceState)   //Durch MOT11 Controller vorgegeben, darf hier nicht gesetzt werden da sonst asynchon. Im Fehlerfall wird in safestate gewechselt, dadurch nimmt APP.MCU OEN zurück und MOT11 Controller geht auch in Safestate
            {
                default:
                case deviceState_init:    
                case deviceState_safeState:    
                    //Über Request wird zyklisch alle live Parameter abgefragt
                    if(this->to_parameterPoll.check())
                    {
                        this->requestDriveParameter(); 
                        this->to_parameterPoll.reset();
                    } 
                break;

                case deviceState_running:   //Normalbetreb
                    if(this->ports.p_object->newDriveParameterAvailable())
                    {
                        this->sendDriveCommand(this->ports.p_object->getDirection(), this->ports.p_object->getSpeed());
                    }
                    //Über Request wird zyklisch alle live Parameter abgefragt
                    if(this->to_parameterPoll.check())
                    {
                        this->requestDriveParameter(); 
                        this->to_parameterPoll.reset();
                    }  
                break;

                case deviceState_autotuning:
                    //Status abfragen, solange Autotuning aktiv nix tun
                    if(this->to_parameterPoll.check())
                    {
                        this->requestDriveParameter(); 
                        this->to_parameterPoll.reset();
                    }              
                break;
            }        
        }
        else
        {
            this->error.code = MOT11_ERROR__NO_PORT_IN_USE;
        }
    }
}

e_BPLC_ERROR_t HAL_MOT11::getError()
{
    return this->error.code;
}

void HAL_MOT11::sendDriveCommand(const e_movement_t DIRECTION, const uint8_t SPEED)
{  
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));

    COMMAND.extract.key         = (uint8_t)mot11_i2c_key__driveCommand;
    COMMAND.extract.direction   = (uint8_t)DIRECTION;
    COMMAND.extract.speed       = SPEED;

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
    const bool RECEIVED_ERROR_CODE_PLAUSIBLE = (bool)(inCommand.extract.error < BPLC_ERROR__COUNT);
    //Empfangene Parameter übernehemen
    if(RECEIVED_ERROR_CODE_PLAUSIBLE && inCommand.extract.key == mot11_i2c_key__getDriveState)
    {
        this->deviceState = (e_deviceState_t)inCommand.extract.deviceState;
        this->ports.p_object->setCurrent(inCommand.extract.current);
        this->error.code = (e_BPLC_ERROR_t)inCommand.extract.error;
    }
    else
    {        
        this->error.code = MOT11_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE;
    }

#ifdef DEBUG_HAL_MOT11 
Serial.println("Drive Parameter:");
Serial.print("KEY: ");        Serial.println(inCommand.extract.key);
Serial.print("DEVICESTATE: ");Serial.println(inCommand.extract.deviceState);
Serial.print("DIRECTION: ");  Serial.println(inCommand.extract.direction);
Serial.print("SPEED: ");      Serial.println(inCommand.extract.speed);
Serial.print("ERROR: ");      Serial.println(inCommand.extract.error);
Serial.print("CURRENT: ");    Serial.println(inCommand.extract.current);
Serial.println("");
#endif
    return (bool)(inCommand.extract.key == mot11_i2c_key__getDriveState);
}