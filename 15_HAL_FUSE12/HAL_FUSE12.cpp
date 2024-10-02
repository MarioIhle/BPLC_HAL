#include "HAL_FUSE12.h"

HAL_FUSE12::HAL_FUSE12()
{}
void HAL_FUSE12::init(const e_EC_ADDR_t ADDR)
{
    if(ADDR < FUSE12_ADDRESS_COUNT)
    {
        this->deviceAddress = FUSE12_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(FUSE12_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    this->errordetection.i2cError.countLimit    = 3;
    this->errordetection.i2cError.count         = 0;
    this->channels.p_ioObject                   = nullptr;
    
    this->to_parameterPoll.setInterval(1000);
    this->to_I2C.setInterval(50);                 
    
    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(FUSE12_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->deviceState = FUSE12_DEVICE_STATE__RUNNING;  
        this->printLog("FUSE12revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {
        this->deviceState = FUSE12_DEVICE_STATE__SAFE_STATE;
        this->printLog("FUSE12revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);  
    } 
}
void HAL_FUSE12::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    if(P_IO_OBJECT->getIoType() == IO_TYPE__DIGITAL_INPUT)
    {
        if(CHANNEL < EC_CHANNEL_1 || CHANNEL > FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
        }
        else if(this->channels.p_inObject != nullptr && CHANNEL == FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {   
            this->channels.p_inObject = P_IO_OBJECT;
        }
    } 
    else if (P_IO_OBJECT->getIoType() == IO_TYPE__OUTPUT_PUSH)
    {
        if(CHANNEL < EC_CHANNEL_1 || CHANNEL > FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
        }
        else if(this->channels.p_outObject != nullptr && CHANNEL == FUSE12_CHANNEL_COUNT)
        {
            this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
        }
        else
        {   
            this->channels.p_outObject = P_IO_OBJECT;
        }
    } 
}
void HAL_FUSE12::tick()
{
    if(this->noErrorSet())
    {  
        //I2C Error check
        const bool I2C_ERROR_COUNT_LIMIT_REACHED = (bool)(this->errordetection.i2cError.count >= this->errordetection.i2cError.countLimit);
        if(I2C_ERROR_COUNT_LIMIT_REACHED)
        {
            this->setError(FUSE12_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);

            #ifdef DEBUG_HAL_FUSE12
            Serial.println("errordetection count limit reachded");
            #endif
        }
        //Error behandlung
        if(this->getError()->errorCode != BPLC_ERROR__NO_ERROR)
        {
            this->deviceState = FUSE12_DEVICE_STATE__SAFE_STATE;
        }
            
        switch(this->deviceState)   //Durch FUSE12 Controller vorgegeben, darf hier nicht gesetzt werden da sonst asynchon. Im Fehlerfall wird in safestate gewechselt, dadurch nimmt APP.MCU OEN zurück und FUSE12 Controller geht auch in Safestate
        {
            default:
            case FUSE12_DEVICE_STATE__INIT:    
            case FUSE12_DEVICE_STATE__SAFE_STATE:    
                //Über Request wird zyklisch alle live Parameter abgefragt
                if(this->to_parameterPoll.checkAndReset())
                {
                    this->requestDriveParameter();                    
                } 
            break;

            case FUSE12_DEVICE_STATE__RUNNING:   //Normalbetreb
                if(this->channels.p_ioObject->newDataAvailable())
                {
                    this->sendDriveCommand(this->channels.p_ioObject->halCallback());
                }
                //Über Request wird zyklisch alle live Parameter abgefragt
                if(this->to_parameterPoll.check())
                {
                    this->requestDriveParameter(); 
                    this->to_parameterPoll.reset();
                }  
            break;

            case FUSE12_DEVICE_STATE__AUTOTUNING:
                //Status abfragen, solange Autotuning aktiv nix tun
                if(this->to_parameterPoll.check())
                {
                    this->requestDriveParameter(); 
                    this->to_parameterPoll.reset();
                }              
            break;                     
        }
    }
}
void HAL_FUSE12::sendDriveCommand(const u_HAL_DATA_t DRIVE_PARAMETER)
{  
    u_FUSE12_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_FUSE12_i2c_payload_t));

    COMMAND.extract.key         = (uint8_t)FUSE12_I2C_KEY__DRIVE_COMMAND;
    COMMAND.extract.direction   = (uint8_t)DRIVE_PARAMETER.dcDriveData.direction;
    COMMAND.extract.speed       = DRIVE_PARAMETER.dcDriveData.speed;
    this->sendFrame(COMMAND);    

    if(this->waitForACK())
    {
        this->errordetection.i2cError.count = 0;

#ifdef DEBUG_HAL_FUSE12
Serial.println("ACK empfangen");
#endif    
    }   
    else
    {
        this->errordetection.i2cError.count++;

#ifdef DEBUG_HAL_FUSE12
Serial.println("kein ACK empfangen");
#endif 
    } 
}
void HAL_FUSE12::requestDriveParameter()
{
    Wire.requestFrom(this->deviceAddress, sizeof(u_FUSE12_i2c_payload_t));
    
    if(this->waitForDriveParameter())
    {
        this->errordetection.i2cError.count = 0;

#ifdef DEBUG_HAL_FUSE12
Serial.println("Drive Parameter empfangen");
#endif
    }   
    else
    {
        this->errordetection.i2cError.count++;

#ifdef DEBUG_HAL_FUSE12
Serial.println("keine Drive Parameter empfangen");
#endif
    } 
}
void HAL_FUSE12::sendFrame(const u_FUSE12_i2c_payload_t COMMAND)
{
    Wire.beginTransmission(this->deviceAddress);   

    for(uint8_t bytes; bytes < sizeof(u_FUSE12_i2c_payload_t); bytes++)
    {
        //Serial.println(COMMAND.data[bytes]);
        Wire.write(COMMAND.data[bytes]);
    }                

    Wire.endTransmission();    
}

bool HAL_FUSE12::waitForDriveParameter()
{
    u_FUSE12_i2c_payload_t inCommand;
    inCommand.extract.key = FUSE12_I2C_KEY__COUNT;

    this->to_I2C.reset();      

    while(this->to_I2C.check() == false)  
    {     
        while(Wire.available()) 
        {  
            for(uint8_t i=0; i<sizeof(u_FUSE12_i2c_payload_t); i++)
            {     
                //Serial.print(Wire.peek());
                inCommand.data[i] = Wire.read();   
            }   
        } 
        //exit wenn Kommando empfangen  
        if(inCommand.extract.key == FUSE12_I2C_KEY__GET_DRIVE_STATE)
        {
            break;
        }     
    }
    
    //Empfangenen Errorcode auswerten, wenn plausibel
    const bool RECEIVED_ERROR_CODE_PLAUSIBLE = (bool)(inCommand.extract.error < BPLC_ERROR__COUNT);
    //Empfangene Parameter übernehemen
    if(RECEIVED_ERROR_CODE_PLAUSIBLE && inCommand.extract.key == FUSE12_I2C_KEY__GET_DRIVE_STATE)
    {
        this->deviceState = (e_FUSE12_DEVICE_STATE_t)inCommand.extract.deviceState;
        u_HAL_DATA_t tempBuffer;
        tempBuffer.dcDriveData.current = inCommand.extract.current;
        this->channels.p_ioObject->halCallback(&tempBuffer);
        this->setError((e_BPLC_ERROR_t)inCommand.extract.error, __FILENAME__, __LINE__);
    }
    else
    {        
        this->setError(FUSE12_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE, __FILENAME__, __LINE__);
    }

#ifdef DEBUG_HAL_FUSE12 
Serial.println("Drive Parameter:");
Serial.print("KEY: ");        Serial.println(inCommand.extract.key);
Serial.print("DEVICESTATE: ");Serial.println(inCommand.extract.deviceState);
Serial.print("DIRECTION: ");  Serial.println(inCommand.extract.direction);
Serial.print("SPEED: ");      Serial.println(inCommand.extract.speed);
Serial.print("ERROR: ");      Serial.println(inCommand.extract.errordetection);
Serial.print("CURRENT: ");    Serial.println(inCommand.extract.current);
Serial.println("");
#endif
    return (bool)(inCommand.extract.key == FUSE12_I2C_KEY__GET_DRIVE_STATE);
}