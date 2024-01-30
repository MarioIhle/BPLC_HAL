#include "BPLC_LOG.h"

BPLC_LOG::BPLC_LOG()
{}

BPLC_LOG::~BPLC_LOG()
{}

void BPLC_LOG::printLog(String TEXT)
{
    Serial.println("##############LOG##############");
    Serial.println(TEXT);
}