#include"BPLC_APP.h"

void BPLC_APP::setupNetwork()
{
    if(this->APP_APP.settings.device.communication.deviceAddress == 0)
    {
        //Netzwerk nicht in Benutzung
    }
    else if(this->APP_APP.settings.device.communication.deviceAddress == MASTER_NODE_ADDRESS)
    {
        this->printLog("Network setup as masterNode", __FILENAME__, __LINE__);
        MasterNode* p_networkNode = new MasterNode;
        p_networkNode->begin(&Serial2, 4);
        this->APP_COM.p_masterNode = p_networkNode;
    }
    else
    {
        this->printLog("Network setup as slaveNode with address: " + String(this->APP_APP.settings.device.communication.deviceAddress), __FILENAME__, __LINE__);
        SlaveNode* p_networkNode = new SlaveNode;
        p_networkNode->begin(this->APP_APP.settings.device.communication.deviceAddress, &Serial2, 4);
        this->APP_COM.P_slaveNode = p_networkNode;
    }
    //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
    this->APP_COM.to_communicationError.setInterval(60000);
}
void BPLC_APP::mapPortToNetwork(portInterface_APP* P_PORT)
{    
    //Network setup
    if(this->APP_APP.settings.device.communication.deviceAddress == 0)
    {                       
        this->systemErrorManager.setError(BPLC_ERROR__NETWORK_ADDRESS_NOT_DEFINED, __FILENAME__, __LINE__);
    }
    else if(this->APP_APP.settings.device.communication.deviceAddress == 1)
    {
        this->APP_COM.p_masterNode->mapPortToNetwork(P_PORT);
    }
    else
    {
        this->APP_COM.P_slaveNode->mapPortToNetwork(P_PORT);
    }
}
void BPLC_APP::tickNetwork()
{
    if(this->APP_APP.settings.device.communication.deviceAddress > 0)
    {      
        //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
        if(this->APP_COM.to_communicationError.check())
        {
            this->systemErrorManager.setError(BPLC_ERROR__COMMUNICATION_FAILED, __FILENAME__, __LINE__);
        }

        const bool DEVICE_IS_MASTER_NODE = (bool)(this->APP_APP.settings.device.communication.deviceAddress == MASTER_NODE_ADDRESS);

        if(DEVICE_IS_MASTER_NODE)
        {                       
            switch(this->APP_COM.p_masterNode->getError())
            {
                case MASTER_NODE_ERROR__NO_ERROR:
                    this->APP_COM.p_masterNode->tick();
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 2500, 2500);
                    this->APP_COM.to_communicationError.reset();
                break;

                case MASTER_NODE_ERROR__NO_SLAVE_AVAILABLE:
                    this->APP_COM.p_masterNode->tick();
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 100, 100);
                break;

                default:

                case MASTER_NODE_ERROR__COUNT:
                break;
            }                    
        }
        else
        {            
            this->APP_COM.P_slaveNode->tick();

            switch(this->APP_COM.P_slaveNode->getNodeState())
            {
                case SLAVE_NODE_STATE__NOT_AVAILABLE:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 100, 100);
                break;

                case SLAVE_NODE_STATE__RESYNC_PORTS:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 50, 50);
                break;
                
                case SLAVE_NODE_STATE__AVAILABLE:
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 2500, 2500);
                    this->APP_COM.to_communicationError.reset();
                break;
            }
        }        
    }
}