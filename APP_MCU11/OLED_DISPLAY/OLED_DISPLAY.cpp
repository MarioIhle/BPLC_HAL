#include "OLED_DISPLAY.h"

//---------------------------------------------------
//OLED DISPLAY CONSTRUCTOR
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//---------------------------------------------------
//MENÜ TEXTE
const String EXIT = "exit";
const String LOCK = "lock";
String mainMenuTexts        [7] = {{"Fehlerstände"}, {"Diagnose"}, {"DIP"}, {"Settings"}, {LOCK}};
String errorCodesTexts      [3] = {{"1"}, {"2"}, {}};
String diagnoseTexts        [6] = {{"Aus"}, {"Sonder"}, {"Einlass"}, {"Auslass"}, {"Wechselseitig"}, {EXIT}};
String doorCommandsTexts    [7] = {{"EFE"}, {"EFA"}, {"DFE"}, {"DFA"}, {"FREI"}, {"LOCK"}, {EXIT}};
String techaInTexts         [3] = {{"Grundstellung Einstellen"}, {"Einlernvorgang starten"}, {EXIT}};
String onBoardIosTexts      [5] = {{"Input 1"}, {"Input 2"},{"Output 1"}, {"Output 2"}, {EXIT}};
String deviceSettingsTexts  [3] = {{"Bildschrimschoner"}, {"Sicherheit"}, {EXIT}};

//---------------------------------------------------
//PQS_STICK CONSTRUCTOR
MCU11::MCU11(){} 
//---------------------------------------------------
//INIT
//---------------------------------------------------
void MCU11::begin()
{
  if (!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); 
  }  
  oled.setTextSize(2); // Draw 2X-scale text
  oled.setTextColor(SSD1306_WHITE);
  oled.display();  
  delay(2000); // Pause for 2 seconds
  oled.clearDisplay();
  oled.print("booting...");
  oled.display(); 
  delay(1500);
  oled.clearDisplay();

  memset(&this->display,0, sizeof(s_display_t));
  memset(&this->menu,0, sizeof(s_menuParameter_t));
  memset(&this->screenSaverParameter,0, sizeof(s_screenSaverParameter_t));
  memset(&this->deviceSettings,0, sizeof(s_deviceSettingsParameter_t));

  //TODO: SETTINGS AUS EEPROM LESEN!!
  this->deviceSettings.screenSaverIsEnbaled = false;
  this->deviceSettings.sleepTime = 30000;
  this->screenSaverParameter.to_sleep.setInterval(this->deviceSettings.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->menu.active     = menu_mainMenu;
  this->display.mode    = mode_unlocked;


  this->knob.begin(this->pins.encoder.A, this->pins.encoder.B, this->pins.encoder.Z);
}
//---------------------------------------------------
//PUBLIC FUNCTIONS
//---------------------------------------------------
//MAIN ROUTINE
void MCU11::tick()
{
  const bool THERE_IS_SOME_USER_INPUT = (bool)(this->knob.getTurningDirection() != idle);

  if(THERE_IS_SOME_USER_INPUT)
  {
    this->screenSaverParameter.to_sleep.reset();
 
    //Wenn Bildschrimschoner aktiv,--> Gerät aufwecken 
    if(this->display.mode == mode_screenSaver)
    {
      this->display.mode = this->screenSaverParameter.modeBeforeScreenSaver;      
    }
  }
  //Bildschirmschoner nach Timeout aktivieren
  if(this->screenSaverParameter.to_sleep.check() && this->deviceSettings.screenSaverIsEnbaled)
  {
    this->screenSaverParameter.modeBeforeScreenSaver  = this->display.mode;
    this->display.mode                                = mode_screenSaver;  
  }
  //Display wechsel
  if(this->menu.active != this->menu.previousActive || this->display.mode != this->display.previousMode)
  {
    this->menu.previousActive   = this->menu.active;
    this->display.previousMode  = this->display.mode;    
    oled.clearDisplay();
    this->display.cursorPos = 128;
  }  

  //Display page selector
  switch(this->display.mode)
  {
    case mode_screenSaver:
      this->screenSaver();    
    break; 

    case mode_unlocked:
      //Menünavigation
      switch(this->menu.active)
      {
        case menu_mainMenu:      
          this->mainMenu(); 
        break;

        case menu_errorCodes:
          this->showErrorCodes();
        break;

        case menu_diagnose:
        break;    
     }
    break;
  }
  //Display output
  oled.display();
}
//---------------------------------------------------
//PRIVATE FUNCTIONS
//---------------------------------------------------
void MCU11::showMenuText(const String TEXT)
{   
  //Länge des auszugebenden Textes berechnen
  int16_t textLength = TEXT.length() * -24; 
  this->display.cursorPos--;
  if((this->display.cursorPos - 64) <= textLength)
  {
    this->display.cursorPos = 0;
  }
  oled.clearDisplay();
  oled.setTextSize(4);  
  oled.setCursor(this->display.cursorPos,0);
  oled.print(TEXT);
}
//---------------------------------------------------
//SCROLL MENU TEXTS
uint8_t MCU11::getMenuText(const uint8_t LAST_TEXT, const uint8_t PREVIOUS_TEXT)
{
  //Mit dem Encoder durch das Menü scrollen
  uint8_t TEXT = (uint8_t)useKnobAsInput((uint32_t)PREVIOUS_TEXT);
  //Bereichsbegrenzung 
  if(TEXT < 0)
  {
    TEXT = 0;
  }
  if(TEXT >= LAST_TEXT)
  {
    TEXT = LAST_TEXT;
  }
  return TEXT;
}
//---------------------------------------------------
//ENCODER VERARBEITUNG
uint32_t MCU11::useKnobAsInput(uint32_t VALUE)
{
  switch(knob.getTurningDirection())
  {    
    case idle:         
    break;
     
    case left:
      VALUE--;
    break;

    case right:
      VALUE++;
    break;
  }
  return VALUE;
}
//---------------------------------------------------
//SCREENSAVER
void MCU11::screenSaver()
{ 
  //Nichts anzeigen, vielleicht ein Logo?  
}
//---------------------------------------------------
//MAIN MENÜ
void MCU11::mainMenu()
{
  const uint8_t LAST_TEXT       = sizeof(mainMenuTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText); 
  const String  TEXT            = mainMenuTexts[ACTIVE_TEXT];   
  this->showMenuText(TEXT);
  this->menu.activeText         = ACTIVE_TEXT;

  const bool    BUTTON_PRESSED  = this->knob.buttonPressed();

  if(BUTTON_PRESSED)
  {
    this->menu.active = (e_oledMenu_t)this->menu.activeText;
  }
}
//---------------------------------------------------
//ERROR CODE AUSGABE
void MCU11::showErrorCodes()
{}

