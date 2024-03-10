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
#include <Preferences.h>
//BPLC
#include "BPLC_errorHandler.h"
#include "BPLC_controlInterface.h"
//Hardware
#include "BPLC_ecManager.h"
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
    APP_MODE__RUN,
    APP_MODE__RUN_WITHOUT_SAFETY,
    APP_MODE__RUN_WITHOUT_EC_CARDS,
    APP_MODE__RUN_WITHOUT_COM,

    APP_MODE__COUNT,
}e_APP_MODE_t;


//-------------------------------------------------------------
//BPLC_APP KLASSE
//-------------------------------------------------------------
#define HARDWARE_ERROR_BUFFER_SIZE 10
#define RUNTIME_ERRORS_MAX 3
#define RUNNTIME 1000

class BPLC_APP: BPLC_logPrint
{
    public:
    //Setup des BPLC Systems
            BPLC_APP                ();
    void    begin                   ();      
    //Network PORT
    void    mapPortToNetwork        (portInterface_APP* P_PORT);
    //IO PORT
    void    mapIoObjectToExtensionCardChannel(IO_Interface* P_IO_OBJECT, const e_BPLC_CARD_TYPE_t CARD, const e_EC_ADDR_t ADDR, const uint8_t CHANNEL);
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
    void            readDeviceSettings  ();
    //Device Mode
    e_APP_MODE_t    getDeviceMode   ();    
    void            setDeviceMode   (const e_APP_MODE_t MODE);

    struct
    {
        e_APP_MODE_t   deviceMode;
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
                    bool f_beepOnEncoderInput;
                    bool f_useBuzzer;
                    bool f_encoderInverted;   
                }application;   

                struct 
                {
                    uint8_t deviceAddress;  
                }communication;               

                struct 
                {            
                    e_BPLC_CARD_TYPE_t  mcuCard;
                    bool                oledAvailable;
                    bool                ain11revACards [I2C_ADDRESS_AIN11__COUNT];
                    bool                din11revACards [I2C_ADDRESS_AIN11__COUNT];                
                    bool                do11revACards  [I2C_ADDRESS_AIN11__COUNT];
                    bool                rel11revACards [I2C_ADDRESS_AIN11__COUNT];
                    bool                mot11revAcards [I2C_ADDRESS_AIN11__COUNT];
                    bool                tmp11revACards [I2C_ADDRESS_AIN11__COUNT];
                    bool                ppo11revACards [I2C_ADDRESS_AIN11__COUNT];
                    bool                nano11revACards[I2C_ADDRESS_NANO11__COUNT];
                    bool                fuse12revACards[I2C_ADDRESS_FUSE12__COUNT];
                }hardware;

            }device;               

            uint8_t flashData[sizeof(device)];
        }settings; 

    }APP_APP;       
    
    //APP_HMI
    void setupHMI           ();
    void handleDisplay      ();
    void editDeviceMode     ();
    void hardwareErrorOut   ();
    void displaySettings    ();
    void handle_vDip        ();

    struct
    {        
        int16_t     temp_ParameterStorage;         
    }APP_HMI;    
  

    //Externer aufruf, wenn HAL Objekt ein Error meldet
    BPLC_moduleErrorHandler systemErrorManager;

    void            setupSafety             ();
    void            tickSafety              ();
    void            scanForUnkonwnI2CDevices();
   
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
        //Hal objecte zu allen möglichen Erweiterungskarten     
        OLED_MCU11      oled;     
        
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