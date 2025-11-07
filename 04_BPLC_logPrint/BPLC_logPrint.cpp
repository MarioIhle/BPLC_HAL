#include "BPLC_logPrint.h"

BPLC_logPrint::BPLC_logPrint()
{}
//ESP: __FILENAME__, __LINE__
void BPLC_logPrint::printLog(String TEXT, String FILE, const uint16_t LINE) 
{  
    Serial.println("");
    Serial.println("----------------LOG----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    Serial.println("--> " + TEXT);
    Serial.flush();
}
//ESP: __FILENAME__, __LINE__
void BPLC_logPrint::printErrorSet(const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE)
{
    Serial.println("");
    Serial.println("---------------SET-ERROR----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    ERROR_errorText text;
    Serial.println("-->ERROR CODE: " + String(ERROR_CODE) + ", " + String(text.getErrorCodeText(ERROR_CODE)));
    Serial.flush();
}
//ESP: __FILENAME__, __LINE__
void BPLC_logPrint::printErrorReset(const e_BPLC_ERROR_t ERROR_CODE,  String FILE, const uint16_t LINE)
{
    Serial.println("");
    Serial.println("---------------RESET-ERROR----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    ERROR_errorText text;
    Serial.println("-->ERROR CODE: " + String(ERROR_CODE) + ", " + String(text.getErrorCodeText(ERROR_CODE)));
    Serial.flush();
}
//ESP: __FILENAME__, __LINE__
void BPLC_logPrint::printResetReason(String TEXT, String FILE, const uint16_t LINE)
{    
    Serial.println("");
    Serial.println("----------------RESET REASON----------------");
    Serial.println("OS Time: " + String(millis()) + ", FILE: " + FILE +", LINE: " + String(LINE));
    Serial.println("--> " + TEXT);
    Serial.flush();
    delay(2500);
}
//ESP: __FILENAME__, __LINE__
void BPLC_logPrint::printRamUsage()
{
#ifndef ARDUINO_AVR_NANO
    Serial.print("HEAP_SIZE: ");      Serial.print(ESP.getHeapSize()); 
    Serial.print(", FREE_HEAP: ");    Serial.print(ESP.getFreeHeap()); 
    Serial.print(", SKETCH_SIZE: ");  Serial.print(ESP.getSketchSize()); 
    Serial.print(", FLASH_SIZE: ");   Serial.print(ESP.getFlashChipSize()); 
    Serial.print(", SPIRAM_SIZE: ");  Serial.println(ESP.getFreePsram()); 
#endif
}
void BPLC_logPrint::printExtensionCardDebugOutput(String EXTENSIONCARD_TYPE, String ADDR, String CHANNEL, String VALUE)
{
    Serial.println("");
    Serial.println("----------------EC DEBUG OUTPUT----------------");
    Serial.println("OS Time: " + String(millis()));
    Serial.println("--> " + EXTENSIONCARD_TYPE + " ADDR:" + ADDR +  ", CHANNEL" + CHANNEL + " CHANGED TO: " + VALUE);
    Serial.flush();
}