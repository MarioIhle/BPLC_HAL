#include "OLED_DISPLAY.h"

//---------------------------------------------------
//OLED DISPLAY CONSTRUCTOR
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//---------------------------------------------------
//MENÜ TEXTE
const String EXIT = "exit";
const String LOCK = "lock";

String mainMenuTexts        [5] = {{"DEVICE MODE"}, {"ERRORS"}, {"SETUP"}, {"VDIP"}, {LOCK}};
String deviceMode           [5] = {{"STOP"}, {"STARTUP"}, {"RUNNING"}, {"ERROR"},{EXIT}};
String errorCodesTexts      [3] = {{"ERROR1"}, {"ERROR2"}, {EXIT}};
String deviceSettingsTexts  [3] = {{"SETTING1"}, {"SETTING2"}, {EXIT}};
String dipSwitchTexts       [9] = {{"DIP1"}, {"DIP2"}, {"DIP3"},{"DIP4"},{"DIP5"},{"DIP6"},{"DIP7"},{"DIP8"},{EXIT}};

//---------------------------------------------------
//CONSTRUCTOR
OLED_MCU11::OLED_MCU11(){} 

//---------------------------------------------------
//INIT
void OLED_MCU11::begin()
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
  this->deviceSettings.screenSaverIsEnbaled = true;
  this->deviceSettings.sleepTime = 30000;
  this->screenSaverParameter.to_sleep.setInterval(this->deviceSettings.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->menu.activeMenu     = menu_mainMenu;
  this->display.mode        = mode_unlocked;

}

//---------------------------------------------------
//MAIN ROUTINE
void OLED_MCU11::tick()
{  
  Serial.print("menu: "); Serial.print(this->menu.activeMenu);
  Serial.print(", text: "); Serial.println(this->menu.activeText);

  //Bildschirmschoner nach Timeout aktivieren
  if(this->screenSaverParameter.to_sleep.check() && this->deviceSettings.screenSaverIsEnbaled)
  {
    this->screenSaverParameter.modeBeforeScreenSaver  = this->display.mode;
    this->display.mode                                = mode_screenSaver;  
  }

  //Display wechsel
  if(this->menu.activeMenu != this->menu.previousActiveMenu || this->display.mode != this->display.previousMode)
  {
    this->menu.previousActiveMenu   = this->menu.activeMenu;
    this->display.previousMode      = this->display.mode;   
    this->menu.activeText           = 0; 
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
      switch(this->menu.activeMenu)
      {
        case menu_mainMenu:      
          this->showmainMenu(); 
        break;

        case menu_errorCodes:
          this->showErrorCodes();
        break;

        case menu_settings:
          this->showSettings();
        break;    

        case menu_dipSwitch:
          this->showDipswitches();
        break;

     }
    break;
  }
  //Display output
  oled.display();
}

//---------------------------------------------------
//Steuerung des Menü
void OLED_MCU11::accept()
{  
  //Gerät aufwecken
  if(this->display.mode == mode_screenSaver)
  {
    this->display.mode      = mode_unlocked;
    this->menu.activeMenu   = menu_mainMenu;
    this->screenSaverParameter.to_sleep.reset();
  }
  else
  {
    this->f_accept = true;
  }
}

void OLED_MCU11::cursorUp()
{
  this->f_encoderUp = true;

  //Gerät aufwecken
  if(this->display.mode == mode_screenSaver)
  {
    this->display.mode      = mode_unlocked;
    this->menu.activeMenu   = menu_mainMenu;
    this->screenSaverParameter.to_sleep.reset();
  }
  Serial.println("Call up");
}

void OLED_MCU11::cursorDown()
{
  this->f_encoderDown = true;

  //Gerät aufwecken
  if(this->display.mode == mode_screenSaver)
  {
    this->display.mode      = mode_unlocked;
    this->menu.activeMenu   = menu_mainMenu;
    this->screenSaverParameter.to_sleep.reset();
  }

  Serial.println("Call down");
}

void OLED_MCU11::setDeviceModeToShow(const uint8_t MODE)
{
  this->deviceMode = MODE;
}
//---------------------------------------------------
//Text ausgeben
void OLED_MCU11::showMenuText(const String TEXT)
{   
  //Länge des auszugebenden Textes berechnen
  int16_t textLength = TEXT.length() * -24; 
  if(textLength > 120)
  {
    this->display.cursorPos--;
  }
  else
  {
    this->display.cursorPos = 0;
  }
 
  if((this->display.cursorPos - 64) <= textLength)
  {
    this->display.cursorPos = 0;
  }
  oled.clearDisplay();
  oled.setTextSize(2);  
  oled.setCursor(this->display.cursorPos,0);
  oled.print(TEXT);
}

uint8_t OLED_MCU11::getMenuText(const uint8_t LAST_AVAILABLE_TEXT, const int ACTIVE_TEXT)
{
  int text = ACTIVE_TEXT;  
  if(text > LAST_AVAILABLE_TEXT)
  {
    text = LAST_AVAILABLE_TEXT;
  }
  else if(text < 0 )
  {
    text = 0;
  }
  return text;
}

//---------------------------------------------------
//SCREENSAVER
void OLED_MCU11::screenSaver()
{ 
  //Nichts anzeigen, vielleicht ein Logo?  
}

//---------------------------------------------------
//MENU AUSGABE
void OLED_MCU11::showmainMenu()
{
  //Encoder auswetung
  if(this->f_encoderUp)
  {
    this->menu.activeText++;
  }
  else if(f_encoderDown)
  {
    this->menu.activeText--;
  }
  this->f_encoderUp = false;
  this->f_encoderDown = false;

  const uint8_t LAST_AVAILABLE_TEXT       = sizeof(mainMenuTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT               = this->getMenuText(LAST_AVAILABLE_TEXT, this->menu.activeText); 
  const String  TEXT                      = mainMenuTexts[ACTIVE_TEXT];  
  this->menu.activeText                   = ACTIVE_TEXT; 
  this->showMenuText(TEXT);
  

  //Bestätigung 
  if(this->f_accept == true)
  {
    if(TEXT == LOCK)
    {
      this->display.mode = mode_screenSaver;
    }
    else
    {
      this->menu.activeMenu = (e_oledMenu_t)this->menu.activeText;  
    }     
    this->f_accept = false;
  }
}

void OLED_MCU11::showErrorCodes()
{
  //Encoder auswetung
  if(this->f_encoderUp)
  {
    this->menu.activeText++;
  }
  else if(f_encoderDown)
  {
    this->menu.activeText--;
  }
  this->f_encoderUp = false;
  this->f_encoderDown = false;

  const uint8_t LAST_TEXT       = sizeof(errorCodesTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText); 
  const String  TEXT            = errorCodesTexts[ACTIVE_TEXT];  

  this->showMenuText(TEXT);
  this->menu.activeText         = ACTIVE_TEXT; 

  //Bestätigung 
  if(this->f_accept == true)
  {
    if(TEXT == EXIT)
    {
      this->menu.activeMenu = menu_mainMenu;
    }
    else
    {
      //enter Parameter;  
    }   
    this->f_accept = false;  
  }
}

void OLED_MCU11::showSettings()
{
  //Encoder auswetung
  if(this->f_encoderUp)
  {
    this->menu.activeText++;    
  }
  else if(f_encoderDown)
  {
    this->menu.activeText--;
  }
  this->f_encoderUp = false;
  this->f_encoderDown = false;

  const uint8_t LAST_TEXT       = sizeof(deviceSettingsTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText); 
  const String  TEXT            = deviceSettingsTexts[ACTIVE_TEXT];  

  this->showMenuText(TEXT);
  this->menu.activeText         = ACTIVE_TEXT; 

  //Bestätigung 
  if(this->f_accept == true)
  {
    if(TEXT == EXIT)
    {
      this->menu.activeMenu = menu_mainMenu;
    }
    else
    {
      //Enter Parameter;
    }     
    this->f_accept = false;
  }
}

void OLED_MCU11::showDipswitches()
{
  const uint8_t LAST_TEXT       = sizeof(dipSwitchTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText);
  const String  DIP_VALUE       = String(this->digitalDipSwitch[ACTIVE_TEXT], DEC);

  String  TEXT                  = String(dipSwitchTexts[ACTIVE_TEXT] + ": " + DIP_VALUE); 
  this->menu.activeText         = ACTIVE_TEXT; 

  if(dipSwitchTexts[ACTIVE_TEXT] == EXIT)
  {
    TEXT = dipSwitchTexts[ACTIVE_TEXT];
  }    
  
  this->showMenuText(TEXT);      

  //Encoder auswetung
  if(this->f_encoderUp)
  {
    this->menu.activeText++;    
  }
  else if(f_encoderDown)
  {
    this->menu.activeText--;
  }
  this->f_encoderUp = false;
  this->f_encoderDown = false;

  //Bestätigung 
  if(this->f_accept == true)
  {
    if(TEXT == EXIT)
    {
      this->menu.activeMenu = menu_mainMenu;
    }
    else
    {
      this->digitalDipSwitch[this->menu.activeText] = !this->digitalDipSwitch[this->menu.activeText];
    }     
    this->f_accept = false;
  }
}

void OLED_MCU11::showDeviceMode()
{
  const uint8_t LAST_TEXT       = sizeof(dipSwitchTexts)/sizeof(String) - 1;   
  const uint8_t ACTIVE_TEXT     = this->getMenuText(LAST_TEXT, this->menu.activeText);
  const String  DIP_VALUE       = String(this->digitalDipSwitch[ACTIVE_TEXT], DEC);

  String  TEXT                  = String(dipSwitchTexts[ACTIVE_TEXT] + ": " + DIP_VALUE); 
  this->menu.activeText         = ACTIVE_TEXT; 

  if(dipSwitchTexts[ACTIVE_TEXT] == EXIT)
  {
    TEXT = dipSwitchTexts[ACTIVE_TEXT];
  }    
  
  this->showMenuText(TEXT);      

  //Encoder auswetung
  if(this->f_encoderUp)
  {
    this->menu.activeText++;    
  }
  else if(f_encoderDown)
  {
    this->menu.activeText--;
  }
  this->f_encoderUp = false;
  this->f_encoderDown = false;

  //Bestätigung 
  if(this->f_accept == true)
  {
    if(TEXT == EXIT)
    {
      this->menu.activeMenu = menu_mainMenu;
    }
    else
    {
      this->digitalDipSwitch[this->menu.activeText] = !this->digitalDipSwitch[this->menu.activeText];
    }     
    this->f_accept = false;
  }
}