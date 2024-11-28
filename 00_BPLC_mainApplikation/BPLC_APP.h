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
#ifndef ARDUINO_AVR_NANO
#include <Preferences.h>

//BPLC
#include "BPLC_types.h"
#include "BPLC_errorHandler.h"
#include "BPLC_controlInterface.h"

//Hardware
#include "04_BPLC_extensionCardManager/BPLC_ecManager.h"

//MCU HMI
#include "00_BPLC_mainApplikation/APP_HMI/APP_HMI.h"

//Network
#include "BertaNetNode.h"
#include "BertaPorts.h"
#include "BertaNetwork.h"
#endif

//-------------------------------------------------------------
//BPLC_APP KLASSE
//-------------------------------------------------------------
#define HARDWARE_ERROR_BUFFER_SIZE 10
#define RUNTIME_ERRORS_MAX 3
#define RUNNTIME 1000

class BPLC_APP: BPLC_logPrint, CRC16Calculator
{
    public:
    //Setup des BPLC Systems
            BPLC_APP                ();
    void    begin                   ();      
    //Network PORT
    void    mapPortToNetwork        (portInterface_APP* P_PORT);
    //IO PORT
    void    mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_EC_TYPE_t CARD, const e_EC_ADDR_t ADDR, const e_EC_CHANNEL_t CHANNEL);
    //Rountine aufruf
    void    tick                    ();    
    //Buzzer aus Applikation heraus nutzen
    void    beep                    (const uint8_t BEEPS, const int BEEP_INTERVAL);    
    //Dip Controll
    void    setVDip                 (const e_V_DIP_t DIP_NUM, const int16_t VALUE);
    int16_t getVDip                 (const e_V_DIP_t DIP_NUM);

    
    private:
    //ControlPanel
    BPLC_controlInterface   hostPc;
    void                    tickControlPanel    ();
    void                    setupControlPanel   ();

    //APP_APP 
    void            setupApplication    ();
    //DeviceSettings
    Preferences     parameterFlash;
    void            setupParameterFlash ();
    void            saveDeviceSettings  ();
    void            loadDeviceSettings  ();
    //Device Mode
    e_BPLC_DEVICE_MODE_t    getDeviceMode   ();    
    void            setDeviceMode   (const e_BPLC_DEVICE_MODE_t MODE);

    struct
    {
        e_BPLC_DEVICE_MODE_t   deviceMode;
        int16_t        virtualDipSwitch[vDIP_COUNT]; 

        struct
        {
            bool f_setupParameterFlash;
            bool f_completeSetupDone;
        }setup;       
        
        union 
        {        
            struct 
            {
                struct 
                {                   
                    bool f_useBuzzer;                       
                }application;   

                struct 
                {
                    bool f_encoderInverted;
                    bool f_beepOnEncoderInput;
                }hmi;

                struct 
                {
                    uint8_t deviceAddress;  
                }communication;               

                struct 
                {            
                    e_EC_TYPE_t  mcuCard;
                    bool                oledAvailable;
                    bool                ain11revACards [AIN11_ADDRESS_COUNT];
                    bool                din11revACards [DIN11_ADDRESS_COUNT];                
                    bool                do11revACards  [DO11_ADDRESS_COUNT];
                    bool                rel11revACards [REL11_ADDRESS_COUNT];
                    bool                mot11revAcards [MOT11_ADDRESS_COUNT];
                    bool                tmp11revACards [TMP11_ADDRESS_COUNT];
                    bool                ppo11revACards [PPO11_ADDRESS_COUNT];
                    bool                nano11revACards[NANO11_ADDRESS_COUNT];
                    bool                fuse12revACards[FUSE12_ADDRESS_COUNT];
                }hardware;

            }device;               

            uint8_t flashData[sizeof(device)];
        }settings; 

    }APP_APP;         
  
    //Externer aufruf, wenn HAL Objekt ein Error meldet
    BPLC_moduleErrorHandler systemErrorManager;

    void setupSafety             ();
    void tickSafety              ();
  
   
    struct  
    {   
        struct 
        {
            Timeout         to_runnntime;
            uint8_t         runtimeExeeded;
        }runntimeControl;          

        Timeout             to_scanI2Cbus;
    }APP_SAFETY;


    //APP_HAL
    BPLC_extensionCardManager   extensionCardManager; 
    void                        setupHardware   ();
    void                        tickHardware    ();
    
    struct 
    {        
        BPLC_HMI*       p_hmi;    
        hmiEncoder      ENCODER; 
        output          BUZZER;
        output          LD1_DEVICE_STATE;
        output          LD2_COMMUNICATION_STATE;
        output          LD3_ERROR_OUT;   
        output          OEN;   
    }APP_HAL;


    //BPLC_COM Netzwerk(BertaNetPorts)
    void setupNetwork   ();
    void tickNetwork    ();

    struct 
    {        
        MasterNode*     p_masterNode;
        SlaveNode*      P_slaveNode;  
        Timeout         to_communicationError; 
    }APP_COM;  
};
#endif