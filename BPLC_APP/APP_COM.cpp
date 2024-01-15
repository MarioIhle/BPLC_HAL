#include"BPLC_APP.h"

void BPLC_APP::setDeviceAddress(const uint8_t DEVICE_ADDRESS)
{
    this->APP_COM.deviceAddress = DEVICE_ADDRESS;
}

void BPLC_APP::setupNetwork()
{
    if(this->APP_COM.deviceAddress == 0)
    {
        //Netzwerk nicht in Benutzung
    }
    else if(this->APP_COM.deviceAddress == 1)
    {
        this->APP_COM.Master.begin(&Serial2, 4);
    }
    else
    {
        this->APP_COM.Slave.begin(this->APP_COM.deviceAddress, &Serial2, 4);
    }
    //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
    this->APP_COM.to_communicationError.setInterval(60000);
}

void BPLC_APP::mapPortToNetwork(BertaPort* P_PORT)
{
    this->APP_COM.Slave.mapPortToNetwork(P_PORT);
}

void BPLC_APP::tickNetwork()
{
    if(this->APP_COM.deviceAddress > 0)
    {      
        const bool DEVICE_IS_MASTER_NODE = (bool)(this->APP_COM.deviceAddress == 1);

        //Network setup
        if(DEVICE_IS_MASTER_NODE)
        {
            this->APP_COM.Master.tick();
            this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 2500, 2500);
        }
        else
        {
            //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
            if(this->APP_COM.to_communicationError.check())
            {
                this->setSystemError(BPLC_ERROR__COMMUNICATION_FAILED);
            }

            this->APP_COM.Slave.tick();

            switch (this->APP_COM.Slave.getNodeState())
            {
                case SLAVE_NODE_STATE__NOT_AVAILABLE:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 100, 100);
                break;

                case SLAVE_NODE_STATE__RESYNC_PORTS:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 50, 50);
                break;
                
                case SLAVE_NODE_STATE__AVAILABLE:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 2500, 2500);
                    this->APP_COM.to_communicationError.reset();
                break;
            }
        }        
    }
}