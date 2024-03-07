#include "BPLC_logPrint.h"

BPLC_logPrint::BPLC_logPrint()
{}
void BPLC_logPrint::printLog(String TEXT, String FILE, const uint16_t LINE)
{  
    Serial.println("----------------LOG----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    Serial.println("--> " + TEXT);
    Serial.flush();
}
void BPLC_logPrint::printError(const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE)
{
    Serial.println("----------------ERROR----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    Serial.println("-->ERROR CODE: " + String(ERROR_CODE) + ", " + String(this->getErrorCodeText(ERROR_CODE)));
    Serial.flush();
}
void BPLC_logPrint::printResetReason(String TEXT, String FILE, const uint16_t LINE)
{    
    Serial.println("----------------RESET REASON----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    Serial.println("--> " + TEXT);
    Serial.flush();
    delay(1000);
}