#include "HAL_MOT11.h"

HAL_MOT11::HAL_MOT11(const e_MOT11_ADDRESS_t I2C_ADDRESS)
{
    this->deviceAddress = I2C_ADDRESS;
}
void HAL_MOT11::init()
{
    this->errordetection.i2cError.countLimit    = 3;
    this->errordetection.i2cError.count         = 0;
    this->channels.p_ioObject                   = nullptr;
    
    this->to_parameterPoll.setInterval(1000);
    this->to_I2C.setInterval(50);                 
    
    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(MOT11_ERROR__I2C_CONNECTION_FAILED);        
    }

    //Applikationsparameter initialisieren
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {   
        this->deviceState = deviceState_running;  
        this->printLog("MOT11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL");        
    }    
    else
    {
        this->deviceState = deviceState_safeState;
        this->printLog("MOT11revA CARD (" + String(this->deviceAddress) + ") INIT FAILED");  
    } 
}
void HAL_MOT11::mapObjectToChannel(IO_Interface* P_IO_OBJECT, const uint8_t CHANNEL)
{
    if(CHANNEL < 1 || CHANNEL > MOT11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__CHANNEL_OUT_OF_RANGE);
    }
    else if(this->channels.p_ioObject != nullptr && CHANNEL == MOT11_CHANNEL_COUNT)
    {
        this->setError(DIN11_ERROR__ALL_CHANNELS_ALREADY_IN_USE);
    }
    else
    {   
        this->channels.p_ioObject = P_IO_OBJECT;
    }
}
void HAL_MOT11::tick()
{
    if(this->getError() == BPLC_ERROR__NO_ERROR)
    {  
        //I2C Error check
        const bool I2C_ERROR_COUNT_LIMIT_REACHED = (bool)(this->errordetection.i2cError.count >= this->errordetection.i2cError.countLimit);
        if(I2C_ERROR_COUNT_LIMIT_REACHED)
        {
            this->setError(MOT11_ERROR__I2C_CONNECTION_FAILED);

            #ifdef DEBUG_HAL_MOT11
            Serial.println("errordetection count limit reachded");
            #endif
        }
        //Error behandlung
        if(this->getErrorCode() != BPLC_ERROR__NO_ERROR)
        {
            this->deviceState = deviceState_safeState;
        }

        if(this->getErrorCode() != MOT11_ERROR__I2C_CONNECTION_FAILED)
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
    }
}
void HAL_MOT11::startCurrentAutotuning()
{
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));
    
    COMMAND.extract.key = (uint8_t)mot11_i2c_key__startCurrentAutotuning;

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
void HAL_MOT11::sendDriveCommand(const u_IO_DATA_BASE_t DRIVE_PARAMETER)
{  
    u_mot11_i2c_payload_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_mot11_i2c_payload_t));

    COMMAND.extract.key         = (uint8_t)mot11_i2c_key__driveCommand;
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
        u_IO_DATA_BASE_t tempBuffer;
        tempBuffer.dcDriveData.current = inCommand.extract.current;
        this->channels.p_ioObject->halCallback(&tempBuffer);
        this->setError((e_BPLC_ERROR_t)inCommand.extract.error);
    }
    else
    {        
        this->setError(MOT11_ERROR__RECEIVED_DRIVE_PARAMETER_NOT_PLAUSIBLE);
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
    return (bool)(inCommand.extract.key == mot11_i2c_key__getDriveState);
}