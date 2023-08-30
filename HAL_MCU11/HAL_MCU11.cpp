#include "HAL_DO11.h"

/---------------------------------------------------
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
OnBoardPavis::OnBoardPavis(){} 
//---------------------------------------------------
//INIT
//---------------------------------------------------
void OnBoardPavis::begin(s_OnBoardPavisParameter_t INIT)
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
  this->lockScreenParameter.to_activeNumberBlink.setInterval(500);
  this->lockScreenParameter.to_activeNumberBlink.reset();
  this->deviceSettings.unlockCodeIsEnabled = true;
  this->deviceSettings.lockTime = 60000;
  this->screenSaverParameter.to_lock.setInterval(this->deviceSettings.lockTime);
  this->screenSaverParameter.to_lock.reset();
  for(uint8_t i= 0; i< UNLOCK_CODE_LENGTH; i++)
  {
    this->deviceSettings.unlockCode[i] = i; //EEPROM.read(i + DATALOCATION);
  }

  this->knob  = INIT.p_knob;
  this->ets   = INIT.p_ETS;
  this->IN1   = INIT.p_IN_1;
  this->IN2   = INIT.p_IN_2;
  this->OUT1  = INIT.p_OUT_1;
  this->OUT2  = INIT.p_OUT_2;
  this->menu.active     = menu_mainMenu;
  this->display.mode    = mode_unlocked;
}
//---------------------------------------------------
//PUBLIC FUNCTIONS
//---------------------------------------------------
//MAIN ROUTINE
void OnBoardPavis::tick()
{
  const bool THERE_IS_SOME_USER_INPUT = (bool)(this->knob->getTurningDirection() != idle ||this->knob->getPushButton.low() != 0);
  if(THERE_IS_SOME_USER_INPUT)
  {
    this->screenSaverParameter.to_sleep.reset();
    this->screenSaverParameter.to_lock.reset();

    //Wenn Bildschrimschoner aktiv,--> Gerät aufwecken 
    if(this->display.mode == mode_screenSaver)
    {
      if(this->screenSaverParameter.deviveIsLocked)
      {
        this->display.mode = mode_lockScreen;
        this->menu.active  = menu_mainMenu;
      }
      else
      {
        this->display.mode = this->screenSaverParameter.modeBeforeScreenSaver;
      }
    }
  }
  //Bildschirmschoner nach Timeout aktivieren
  if(this->screenSaverParameter.to_sleep.check() && this->deviceSettings.screenSaverIsEnbaled)
  {
    this->screenSaverParameter.modeBeforeScreenSaver  = this->display.mode;
    this->display.mode                                = mode_screenSaver;  
  }
  //Gerät sperren nach Timeout
  if(this->screenSaverParameter.to_lock.check() && this->deviceSettings.unlockCodeIsEnabled)
  {    
    this->screenSaverParameter.deviveIsLocked = true;
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
void OnBoardPavis::showMenuText(const String TEXT)
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
uint8_t OnBoardPavis::getMenuText(const uint8_t LAST_TEXT, const uint8_t PREVIOUS_TEXT)
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
uint32_t OnBoardPavis::useKnobAsInput(uint32_t VALUE)
{
  switch(knob->getTurningDirection())
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
void OnBoardPavis::screenSaver()
{ 
  //Nichts anzeigen, vielleicht ein Logo?  
}
//---------------------------------------------------
//LOCKSCREEN
void OnBoardPavis::lockScreen()
{   
  //Kein Codesperre aktiv
  else
  {
    if(this->knob->getPushButton.posFlank())
    {
      this->menu.active = menu_mainMenu;
    }
  }
}
//---------------------------------------------------
//MAIN MENÜ
void OnBoardPavis::mainMenu()
{
  const uint8_t LAST_TEXT       = sizeof(mainMenuTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText); 
  const String  TEXT            = mainMenuTexts[ACTIVE_TEXT];   
  this->showMenuText(TEXT);
  this->menu.activeText         = ACTIVE_TEXT;

  const bool    BUTTON_PRESSED  = (bool)(this->knob->getPushButton.posFlank());

  if(BUTTON_PRESSED)
  {
    this->menu.active = (e_OnBoardPavisMenu_t)this->menu.activeText;
  }
}
//---------------------------------------------------
//ERROR CODE AUSGABE
void OnBoardPavis::showErrorCodes()
{}

