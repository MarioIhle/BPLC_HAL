#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11()
{}
void HAL_MOT11::init(const e_EC_ADDR_t ADDR)
{
    if(ADDR < MOT11_ADDRESS_COUNT)
    {
        this->deviceAddress = MOT11_I2C_ADDRESSES[ADDR];             
    }
    else
    {
        this->setError(MOT11_ERROR__I2C_ADDRESS_OUT_OF_RANGE, __FILENAME__, __LINE__);
    } 

    this->errordetection.i2cError.countLimit    = 3;
    this->errordetection.i2cError.count         = 0;
    this->channels.p_ioObject                   = nullptr;
    
    this->to_parameterPoll.setInterval(1000);
    this->to_I2C.setInterval(50);                 
    
    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(MOT11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->deviceState = MOT11_DEVICE_STATE__RUNNING;  
        this->printLog("MOT11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {
        this->deviceState = MOT11_DEVICE_STATE__SAFE_STATE;
        this->printLog("MOT11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED", __FILENAME__, __LINE__);  
    } 
}
void HAL_MOT11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const e_EC_CHANNEL_t CHANNEL)
{
    if(CHANNEL < EC_CHANNEL_1 || CHANNEL > MOT11_CHANNEL_COUNT)
    {
        this->setError(MOT11_ERROR__CHANNEL_OUT_OF_RANGE, __FILENAME__, __LINE__);
    }
    else if(this->channels.p_ioObject != nullptr && CHANNEL == MOT11_CHANNEL_COUNT)
    {
        this->setError(MOT11_ERROR__ALL_CHANNELS_ALREADY_IN_USE, __FILENAME__, __LINE__);
    }
    else
    {   
        this->channels.p_ioObject = P_IO_OBJECT;
    }
}
void HAL_MOT11::tick()
{
    if(this->noErrorSet())
    {  
        //I2C Error check
        const bool I2C_ERROR_COUNT_LIMIT_REACHED = (bool)(this->errordetection.i2cError.count >= this->errordetection.i2cError.countLimit);
        if(I2C_ERROR_COUNT_LIMIT_REACHED)
        {
            this->setError(MOT11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);

            #ifdef DEBUG_HAL_MOT11
            Serial.println("errordetection count limit reachded");
            #endif
        }
        //Error behandlung
        if(this->getError()->errorCode != BPLC_ERROR__NO_ERROR)
        {
            this->deviceState = MOT11_DEVICE_STATE__SAFE_STATE;
        }
            
        switch(this->deviceState)   //Durch MOT11 Controller vorgegeben, darf hier nicht gesetzt werden da sonst asynchon. Im Fehlerfall wird in safestate gewechselt, dadurch nimmt APP.MCU OEN zurück und MOT11 Controller geht auch in Safestate
        {
            default:
            case MOT11_DEVICE_STATE__INIT:    
            case MOT11_DEVICE_STATE__SAFE_STATE:    
                //Über Request wird zyklisch alle live Parameter abgefragt
                if(this->to_parameterPoll.checkAndReset())
                {
                    this->requestDriveParameter();                    
                } 
            break;

            case MOT11_DEVICE_STATE__RUNNING:   //Normalbetreb
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

            case MOT11_DEVICE_STATE__AUTOTUNING:
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
void HAL_MOT11::startCurrentAutotuning()
{
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));
    
    COMMAND.extract.key = (uint8_t)MOT11_I2C_KEY__START_CURRENT_AUTOTUNING;

    this->sendFrame(COMMAND);

    if(this->waitForACK())
    {
        this->errordetection.i2cError.count = 0;

#ifdef DEBUG_HAL_MOT11
Serial.println("ACK empfangen");
#endif    
    }   
    else
    {
        this->errordetection.i2cError.count++;

#ifdef DEBUG_HAL_MOT11
Serial.println("kein ACK empfangen");
#endif 
    } 
}
void HAL_MOT11::sendDriveCommand(const u_HAL_DATA_t DRIVE_PARAMETER)
{  
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));

    COMMAND.extract.key         = (uint8_t)MOT11_I2C_KEY__DRIVE_COMMAND;
    COMMAND.extract.direction   = (uint8_t)DRIVE_PARAMETER.dcDriveData.direction;
    COMMAND.extract.speed       = DRIVE_PARAMETER.dcDriveData.speed;
    this->sendFrame(COMMAND);    

    if(this->waitForACK())
    {
        this->errordetection.i2cError.count = 0;

#ifdef DEBUG_HAL_MOT11
Serial.println("ACK empfangen");
#endif    
    }   
    else
    {
        this->errordetection.i2cError.count++;

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
        this->errordetection.i2cError.count = 0;

#ifdef DEBUG_HAL_MOT11
Serial.println("Drive Parameter empfangen");
#endif
    }   
    else
    {
        this->errordetection.i2cError.count++;

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
    inCommand.extract.key = MOT11_I2C_KEY__COUNT;

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
        //exit wenn Kommando empfangen  
        if(inCommand.extract.key == MOT11_I2C_KEY__GET_DRIVE_STATE)
        {
            break;
        }     
    }
    
    //Empfangenen Errorcode auswerten, wenn plausibel
    const bool RECEIVED_ERROR_CODE_PLAUSIBLE = (bool)(inCommand.extract.error < BPLC_ERROR__COUNT);
    //Empfangene Parameter übernehemen
    if(RECEIVED_ERROR_CODE_PLAUSIBLE && inCommand.extract.key == MOT11_I2C_KEY__GET_DRIVE_STATE)
    {
        this->deviceState = (e_MOT11_DEVICE_STATE_t)inCommand.extract.deviceState;
        u_HAL_DATA_t tempBuffer;
        tempBuffer.dcDriveData.current = inCommand.extract.current;
        this->channels.p_ioObject->halCallback(&tempBuffer);
        this->setError((e_BPLC_ERROR_t)inCommand.extract.error, __FILENAME__, __LINE__);
    }
    else
    {        
        this->setError(MOT11_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE, __FILENAME__, __LINE__);
    }

#ifdef DEBUG_HAL_MOT11 
Serial.println("Drive Parameter:");
Serial.print("KEY: ");        Serial.println(inCommand.extract.key);
Serial.print("DEVICESTATE: ");Serial.println(inCommand.extract.deviceState);
Serial.print("DIRECTION: ");  Serial.println(inCommand.extract.direction);
Serial.print("SPEED: ");      Serial.println(inCommand.extract.speed);
Serial.print("ERROR: ");      Serial.println(inCommand.extract.errordetection);
Serial.print("CURRENT: ");    Serial.println(inCommand.extract.current);
Serial.println("");
#endif
    return (bool)(inCommand.extract.key == MOT11_I2C_KEY__GET_DRIVE_STATE);
}