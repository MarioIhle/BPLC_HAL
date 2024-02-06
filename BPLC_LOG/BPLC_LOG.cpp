#include "BPLC_LOG.h"

BPLC_LOG::BPLC_LOG()
{}

void BPLC_LOG::printLog(String TEXT)
{
    Serial.println("----------------LOG----------------");
    Serial.print("TIMESTAMP: " + String(millis())+", ");
    Serial.println(TEXT);
}

void BPLC_LOG::printError(String TEXT)
{
    Serial.println("----------------ERROR----------------");
    Serial.print("TIMESTAMP: " + String(millis())+ ", ");
    Serial.println(TEXT);
}

void BPLC_LOG::printResetReason(String MODUL, String FUNKTION, String REASON)
{
    //Kurz Zeit geben vor reset
    delay(500);
    Serial.println("----------------RESET REASON----------------");
    Serial.print("TIMESTAMP: " + String(millis())+ ", ");
    Serial.println(MODUL + ", " + FUNKTION + ", " + REASON);
}