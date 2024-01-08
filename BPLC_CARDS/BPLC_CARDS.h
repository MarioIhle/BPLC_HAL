#ifndef BPLC_CARDS_h
#define BPLC_CARDS_h

//--------------------------------------------------------------------
//Version und Revision
//--------------------------------------------------------------------
typedef enum
{
    MCU_CARD__NO_MCU_DEFINED,
    MCU_CARD__MCU11revA,
    MCU_CARD__MCU11revB,

}e_MCU_CARD_TYPE_t;

typedef enum
{
    EXTENSION_CARD__NO_CARD_DEFINED,
    EXTENSION_CARD__DIN11revA,
    EXTENSION_CARD__DO11revA,
    EXTENSION_CARD__AIN11revA,
    EXTENSION_CARD__REL11revA,
    EXTENSION_CARD__MOT11revA,
    EXTENSION_CARD__NANO11revA,
    EXTENSION_CARD__FUSE11revA,
    EXTENSION_CARD__FUSE12revA,

    EXTENSION_CARD__TYPE_COUNT,

}e_EXTENSION_CARD_TYPE_t;
#endif