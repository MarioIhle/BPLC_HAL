#include "BPLC_PLI.h"

BPLC_LOG::BPLC_LOG()
{}
void BPLC_LOG::printLog(String TEXT)
{
    Serial.println("----------------LOG----------------");    
    Serial.println("TIMESTAMP: " + String(millis()));
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
    Serial.println("----------------RESET REASON----------------");
    Serial.print("TIMESTAMP: " + String(millis())+ ", ");
    Serial.println(MODUL + ", " + FUNKTION + ", " + REASON);
    Serial.flush();
    delay(1000);
}