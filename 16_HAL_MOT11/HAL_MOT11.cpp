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

    this->channels.p_ioObject                   = nullptr;    
    this->to_parameterPoll.setInterval(2500);
     
    //I2C Verbindung prüfen
    if(I2C_check::begin(this->deviceAddress) == false)
    {
        this->setError(MOT11_ERROR__I2C_CONNECTION_FAILED, __FILENAME__, __LINE__);        
    }

    //Applikationsparameter initialisieren
    if(this->noErrorSet())
    {   
        this->i2c.begin();
        this->state = MOT11_DEVICE_STATE__INIT;  
        this->printLog("MOT11revA CARD (" + String(this->deviceAddress) + ") INIT SUCCESSFUL", __FILENAME__, __LINE__);        
    }    
    else
    {
        this->state = MOT11_DEVICE_STATE__SAFE_STATE;
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
    //Zyklisch Parameter abfragen, wird über Callback bei Slave behandelt, daher Timing unabhänig
    if(this->to_parameterPoll.checkAndReset())
    {
        this->requestDriveParameter();                    
    }      
    if(this->channels.p_ioObject != nullptr)
    {
        switch(this->state)   //Durch MOT11 Controller vorgegeben, darf hier nicht gesetzt werden da sonst asynchon. Im Fehlerfall wird in safestate gewechselt, dadurch nimmt APP.MCU OEN zurück und MOT11 Controller geht auch in Safestate
        {
            default:
            case IO_TYPE__NOT_DEFINED:
                this->setError(DIN11_ERROR__IO_OBJECT_NOT_SUITABLE, __FILENAME__, __LINE__);
                break;  

            case MOT11_DEVICE_STATE__RUNNING:   //Normalbetreb            
                if(this->channels.p_ioObject->newDataAvailable())
                {            
                    this->sendDriveCommand(this->channels.p_ioObject->halCallback());
                }             
                break;      
        }
    }        
}
void HAL_MOT11::controlCommand(const e_EC_COMMAND_t COMMAND)
{
    switch (COMMAND)
    {
        case EC_COMMAND__MOT11_START_CURRENT_TUNING:
            this->printLog("START CURRENT TUNING", __FILENAME__, __LINE__);
            this->startCurrentAutotuning();
            break;
        
        default:
            this->printLog("WRONG COMMAND FOR THIS EXTENSION CARD", __FILENAME__, __LINE__);
            break;
    }
}
void HAL_MOT11::startCurrentAutotuning()
{
    u_MOT11_DATA_FRAME_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_MOT11_DATA_FRAME_t));
    
    COMMAND.extract.key = (uint8_t)MOT11_I2C_KEY__START_CURRENT_AUTOTUNING;

    this->i2c.sendCommand(this->deviceAddress, COMMAND.data, sizeof(COMMAND));
}
void HAL_MOT11::sendDriveCommand(const u_HAL_DATA_t DRIVE_PARAMETER)
{  
    u_MOT11_DATA_FRAME_t COMMAND;
    memset(&COMMAND, 0, sizeof(u_MOT11_DATA_FRAME_t));

    COMMAND.extract.key         = (uint8_t)MOT11_I2C_KEY__DRIVE_COMMAND;
    COMMAND.extract.direction   = (uint8_t)DRIVE_PARAMETER.dcDriveData.direction;
    COMMAND.extract.speed       = DRIVE_PARAMETER.dcDriveData.speed;
    
    this->i2c.sendCommand(this->deviceAddress, COMMAND.data, sizeof(COMMAND));
}
void HAL_MOT11::requestDriveParameter()
{
    u_MOT11_DATA_FRAME_t BUFFER;
    const bool SLAVE_DATA_RECEIVED = this->i2c.getSlaveData(this->deviceAddress, BUFFER.data, sizeof(u_MOT11_DATA_FRAME_t));
    
    if(SLAVE_DATA_RECEIVED)
    {
        this->state         = (e_MOT11_DEVICE_STATE_t) BUFFER.extract.deviceState;    
        this->error         = (e_BPLC_ERROR_t)BUFFER.extract.error;
        //Daten an IO Objekt weiter geben
        u_HAL_DATA_t inDATA;    
        inDATA.dcDriveData.direction    = (e_MOVEMENT_t)BUFFER.extract.direction;
        inDATA.dcDriveData.speed        = BUFFER.extract.speed;
        inDATA.dcDriveData.current      = BUFFER.extract.current;
        this->channels.p_ioObject->halCallback(&inDATA);
    }
    else
    {
        //irgendwas anders empfangen
    }
    

#ifdef DEBUG_HAL_MOT11 
Serial.println("Drive Parameter:");
Serial.print("KEY: ");        Serial.println(BUFFER.extract.key);
Serial.print("DEVICESTATE: ");Serial.println(BUFFER.extract.deviceState);
Serial.print("DIRECTION: ");  Serial.println(BUFFER.extract.direction);
Serial.print("SPEED: ");      Serial.println(BUFFER.extract.speed);
Serial.print("ERROR: ");      Serial.println(BUFFER.extract.error);
Serial.print("CURRENT: ");    Serial.println(BUFFER.extract.current);
Serial.println("");
#endif
}