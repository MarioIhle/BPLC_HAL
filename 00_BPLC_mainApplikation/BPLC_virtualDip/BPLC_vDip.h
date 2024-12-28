#ifndef BPLC_vDip_h
#define BPLC_vDip_h

#include "Arduino.h"

class BPLC_vDip
{     
    public:
                BPLC_vDip   (String NAME){this->name = NAME; this->value = 0;}
    void        begin       (String NAME){this->name = NAME;}
    void        setValue    (const int64_t VALUE){this->value = VALUE;}
    void        setState    (const bool STATE);
    bool        getState    ();
    int64_t     getValue    (){return this->value;}
    String      getName     (){return this->name;} 
      
    BPLC_vDip*  getNext     (){return this->p_next;}
    void        setNext     (BPLC_vDip* NEXT){this->p_next = NEXT;}


    private:

    String      name;
    int64_t     value;
    BPLC_vDip*  p_next;
};


class BPLC_vDipManager
{
    public:
                BPLC_vDipManager    (){};
    BPLC_vDip*  createNewVirtualDip (String NAME);
    //void        deleteVDip          (String NAME);  //nicht implementiert
    uint8_t     getVDipCount        (){return this->vDipCount;}
    BPLC_vDip*  getFirstVDip        (){return this->p_first;}    


    private:

    BPLC_vDip*  searchVDip          (String NAME);
    void        addNewVDipToList    (BPLC_vDip* P_NEW_DIP);
    
    BPLC_vDip*  p_first;
    uint8_t     vDipCount;

};


#endif