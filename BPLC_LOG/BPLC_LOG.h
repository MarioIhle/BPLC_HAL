#ifndef BPLC_LOG_h
#define BPLC_LOG_h
#include "Arduino.h"




class BPLC_LOG
{
    public:
    BPLC_LOG();
    void printLog(String TEXT);
    void printError(String TEXT);

    private:

};
#endif