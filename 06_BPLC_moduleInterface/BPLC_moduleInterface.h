#ifndef BPLC_moduleInterface_h
#define BPLC_moduleInterface_h

#include "Arduino.h"

//---------------------------------------------------------------------------------------
//NODE INTERFACE 
//---------------------------------------------------------------------------------------
/*
  Einheitliches Interface für weitere Verrabeitung
*/
class moduleInterface
{
  public:
  //virtual void begin   () = 0;
  //virtual void tick    () = 0;
  virtual void enable  () = 0;
  virtual void disable () = 0;
  virtual bool enabled () = 0;
};
#endif