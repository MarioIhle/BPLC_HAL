#include"BPLC_APP.h"

void BPLC_APP::setupNetwork()
{
    if(this->APP_COM.deviceAddress == 0)
    {
        //Netzwerk nicht in Benutzung
    }
    else if(this->APP_COM.deviceAddress == MASTER_NODE_ADDRESS)
    {
        this->printLog("Network setup as p_masterNode");
        MasterNode* p_networkNode = new MasterNode;
        p_networkNode->begin(&Serial2, 4);
        this->APP_COM.p_masterNode = p_networkNode;
    }
    else
    {
        this->printLog("Network setup as P_slaveNode with address: " + String(this->APP_COM.deviceAddress));
        SlaveNode* p_networkNode = new SlaveNode;
        p_networkNode->begin(this->APP_COM.deviceAddress, &Serial2, 4);
        this->APP_COM.P_slaveNode = p_networkNode;
    }
    //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
    this->APP_COM.to_communicationError.setInterval(60000);
}
void BPLC_APP::mapPortToNetwork(portInterface_APP* P_PORT)
{    
    this->printLog("PORT with Index: " + String(P_PORT->getDataBase()->getPortIndex()) + " mapped to Network");
    //Network setup
    if(this->APP_COM.deviceAddress < 0)
    {                       
        this->setSystemError(BPLC_ERROR__ADDRESS_NOT_DEFINED);
    }
    else if(this->APP_COM.deviceAddress == 1)
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
    if(this->APP_COM.deviceAddress > 0)
    {      
        //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
        if(this->APP_COM.to_communicationError.check())
        {
            this->setSystemError(BPLC_ERROR__COMMUNICATION_FAILED);
        }


        const bool DEVICE_IS_MASTER_NODE = (bool)(this->APP_COM.deviceAddress == MASTER_NODE_ADDRESS);

        if(DEVICE_IS_MASTER_NODE)
        {                       
            switch(this->APP_COM.p_masterNode->getError())
            {
                case MASTER_NODE_ERROR__NO_ERROR:
                    this->APP_COM.p_masterNode->tick();
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 2500, 2500);
                    this->APP_COM.to_communicationError.reset();
                break;

                case MASTER_NODE_ERROR__NO_SLAVE_AVAILABLE:
                    this->APP_COM.p_masterNode->tick();
                    this->APP_HAL.LD2_COMMUNICATION_STATE.blinkWithBreak(1, 100, 100);
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