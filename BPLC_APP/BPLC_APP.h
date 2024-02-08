#ifndef BPLC_APP_h
#define BPLC_APP_H
//------------------------------------------------------------
/**
 * @file BPLC_APP.h
 * @author MIE * 
 * @version 1.0
 */
//-------------------------------------------------------------

//-------------------------------------------------------------
//INCLUDES
//-------------------------------------------------------------
#include "Arduino.h"
#include "SpecialFunctions.h"
//BPLC
#include "BPLC_ERRORS.h"
#include "BPLC_LOG.h"
//Hardware
#include "BPLC_extensionCardManager.h"
#include "HAL_MCU11.h"
#include "OLED_DISPLAY.h" 
//Network
#include "BertaNetNode.h"
#include "BertaPorts.h"
#include "BertaNetwork.h"
//-------------------------------------------------------------
//HARDWARE SPEZIFISCHE TYPES
//-------------------------------------------------------------
typedef enum
{
    vDIP_1,
    vDIP_2,
    vDIP_3,
    vDIP_4,
    vDIP_5,
    vDIP_6,
    vDIP_7,
    vDIP_8,

    vDIP_COUNT,
}e_V_DIP_t;

typedef enum
{
    APP_MODE__STOP,    
    APP_MODE__START,
    APP_MODE__SAFE_STATE,
    APP_MODE__RUN_WITH_CONFIG_1,
    APP_MODE__RUN_WITH_CONFIG_2,
    APP_MODE__RUN_WITH_CONFIG_3,

    APP_MODE__COUNT,
}e_APP_MODE_t;


//-------------------------------------------------------------
//BPLC_APP KLASSE
//-------------------------------------------------------------
#define HARDWARE_ERROR_BUFFER_SIZE 10
#define RUNTIME_ERRORS_MAX 3
#define RUNNTIME 1000

class BPLC_APP:BPLC_LOG, ERROR_OUT
{
    public:
    //Setup des BPLC Systems
            BPLC_APP                ();
    void    begin                   (const e_MCU_CARD_TYPE_t MCU_TYPE, const uint8_t DEVICE_ADDRESS = 0);      
    void    invertEncoder           ();
    void    addExtensionCard        (const e_EXTENSION_CARD_TYPE_t CARD_TYPE, const uint8_t CARD_COUNT);
    //Network
    void    mapPortToNetwork        (portInterface_APP* P_PORT);

    void    mapExtensionCardChannel (IO_Interface* P_IO_OBJECT, const e_EXTENSION_CARD_TYPE_t CARD, const uint8_t CHANNEL);

    //Rountine aufruf
    void    tick                    ();    

    //Buzzer aus Applikation heraus nutzen
    void    beep                    (const uint8_t BEEPS, const int BEEP_INTERVAL);    

    //Dip Controll
    void    setVDip                 (const e_V_DIP_t DIP_NUM, const int16_t VALUE);
    int16_t getVDip                 (const e_V_DIP_t DIP_NUM);

    
    private:
   
    BPLC_extensionCardHandler extensionCardHandler;  

    //APP_APP 
    void            setupApplication();
    //Device Mode
    e_APP_MODE_t    getDeviceMode   ();    
    void            setDeviceMode   (const e_APP_MODE_t MODE);

    struct
    {
        e_APP_MODE_t   deviceMode;
        int16_t        virtualDipSwitch[vDIP_COUNT]; 

        struct 
        {
            bool f_beepOnEncoderInput;
            bool f_useBuzzer;
            bool f_initDone;
        }deviceSettings; 
    }APP_APP;       
    
    //APP_HMI
    void setupHMI();
    void handleDisplay();
    void editDeviceMode();
    void hardwareErrorOut();
    void displaySettings();
    void handle_vDip();

    struct
    {        
        int16_t     temp_ParameterStorage;         
    }APP_HMI;    
  
    //Externer aufruf, wenn HAL Objekt ein Error meldet
    void            setupSafety();
    void            tickSafety();
    void            setSystemError(const e_BPLC_ERROR_t ERROR_CODE);
    bool            thereIsAnSystemError();
    e_BPLC_ERROR_t  getFirstSystemErrorCode();

    struct  
    {   
        struct 
        {
            Timeout         to_runnntime;
            uint8_t         runtimeExeeded;
        }runntimeControl;  
        
        e_BPLC_ERROR_t  errorCode[HARDWARE_ERROR_BUFFER_SIZE];       
        ERROR_OUT       errorOut;   //Textausgabe der ErrorCodes
    }APP_SAFETY;

    //APP_HAL
    void setupHardware();
    void tickHardware();
    void handleMCUCard();
    void handleDIN11Cards();
    void handleDO11Cards();
    void handleAIN11Cards();
    void handleMOT11Cards();
    void handleREL11Cards(); 
      

    struct 
    {
        //Hal objecte zu allen möglichen Erweiterungskarten
        HAL_MCU11_revA  MCU11revA_HAL;    
        HAL_MCU11_revB  MCU11revB_HAL;        
        OLED_MCU11      oled;     
        
        rotaryEncoder   ENCODER; 
        Output          BUZZER;
        Output          LD1_DEVICE_STATE;
        Output          LD2_COMMUNICATION_STATE;
        Output          LD3_ERROR_OUT;   
        Output          OEN;   
        uint8_t         INT_count;

        
        struct
        {
            e_MCU_CARD_TYPE_t   MCU_TYPE = MCU_CARD__NO_MCU_DEFINED;
         }hardwareConfig;
    }APP_HAL;


    //BPLC_COM Netzwerk(BertaNetPorts)
    void setupNetwork();
    void tickNetwork();

    struct 
    {        
        MasterNode*     p_masterNode;
        SlaveNode*      P_slaveNode;   
        uint8_t         deviceAddress = 0;       
        Timeout         to_communicationError; 
    }APP_COM;  
};
#endif