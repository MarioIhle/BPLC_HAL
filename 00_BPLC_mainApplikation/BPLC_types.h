#ifndef BPLC_TYPES_h
#define BPLC_TYPES_H

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
    BPLC_DEVICE_MODE__STOP,    
    BPLC_DEVICE_MODE__START,
    BPLC_DEVICE_MODE__SAFE_STATE,
    BPLC_DEVICE_MODE__RUN,
    BPLC_DEVICE_MODE__RUN_WITHOUT_SAFETY,
    BPLC_DEVICE_MODE__RUN_WITHOUT_EC_CARDS,
    BPLC_DEVICE_MODE__RUN_WITHOUT_COM,

    BPLC_DEVICE_MODE__COUNT,
}e_BPLC_DEVICE_MODE_t;

#endif