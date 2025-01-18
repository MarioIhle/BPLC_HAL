#include"BPLC_APP.h"

void comTask(void* taskParameter)
{
    nodeInterface* p_networkNode = (nodeInterface*) taskParameter;

    disableCore1WDT();
    esp_task_wdt_init(2, true);                
    esp_task_wdt_add(NULL);    

    while(true)
    {
        esp_task_wdt_reset();  
        p_networkNode->tick();
        delay(1);
    }    
}
void BPLC_APP::setupNetwork()
{
    if(this->APP_COM.p_comNode == nullptr)
    {
        //Master Node erzeugen
        if(this->APP_APP.settings.device.communication.deviceAddress == MASTER_NODE_ADDRESS)
        {  
            this->printLog("Network setup as masterNode", __FILENAME__, __LINE__);
            this->APP_COM.p_comNode = new MasterNode;
            this->APP_COM.p_comNode->begin(1, &Serial2, 4);
        }
        //Slave Node erzeugen
        else
        {
            this->printLog("Network setup as slaveNode with address: " + String(this->APP_APP.settings.device.communication.deviceAddress), __FILENAME__, __LINE__);
            this->APP_COM.p_comNode = new SlaveNode;
            this->APP_COM.p_comNode->begin(this->APP_APP.settings.device.communication.deviceAddress, &Serial2, 4);
        }
        //Task erstellen
        xTaskCreatePinnedToCore(comTask, "comTask", 4096, this->APP_COM.p_comNode, 1, NULL, 0);
        this->printLog("CREATE NEW COM TASK", __FILENAME__, __LINE__);
        //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
        this->APP_COM.to_communicationError.setInterval(60000);
    }
    else
    {
        this->printResetReason("COM_NODE_ALREADY_DEFINED", __FILENAME__, __LINE__);
        abort();
    }
}
void BPLC_APP::mapPortToNetwork(portInterface_APP* P_PORT)
{    
    //Network setup
    if(this->APP_COM.p_comNode == nullptr)
    {                       
        this->systemErrorManager.setError(BPLC_ERROR__NETWORK_ADDRESS_NOT_DEFINED, __FILENAME__, __LINE__);
    }
    else 
    {
        this->APP_COM.p_comNode->mapPortToNetwork(P_PORT);
    }
}
void BPLC_APP::tickNetwork()
{
    if(this->APP_COM.p_comNode != nullptr)
    {      
        //BPLC error, wenn 1min keine Kommunikation stattgefunden hat
        if(this->APP_COM.to_communicationError.check())
        {
            this->systemErrorManager.setError(BPLC_ERROR__COMMUNICATION_FAILED, __FILENAME__, __LINE__);
        }                            

        switch(this->APP_COM.p_comNode->getState())
        {
            case COM_NODE_STATE__AVAILABLE:
                this->APP_HAL.LD2_COMMUNICATION_STATE.fade(2500, 2500);
                this->APP_COM.to_communicationError.reset();
            break;

            case COM_NODE_STATE__NO_SLAVE_AVAILABLE:   
            case COM_NODE_STATE__NOT_AVAILABLE:
                this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 100, 100);
            break;

            case COM_NODE_STATE__TRYING_TO_ATTACH_TO_MASTER:
            case COM_NODE_STATE__RESYNC_PORTS:
                this->APP_HAL.LD2_COMMUNICATION_STATE.blinkContinious(1, 50, 50);
            break;               
        }              
    }
}