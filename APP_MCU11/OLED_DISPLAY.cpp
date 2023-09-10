#include "OLED_DISPLAY.h"

//---------------------------------------------------
//OLED DISPLAY CONSTRUCTOR
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
//---------------------------------------------------
//MENÜ STANDART TEXTE
const String EXIT       = "exit";
const String LOCK       = "lock";
const String LAST_TEXT  = "last";

String HEADLINE_TEXT [menu_count][10] =
{
  {{"MAIN MENU"}, {"DEVICE MODE"}, {"ERRORS"}, {"SETUP"}, {"VDIP"}, {LOCK}, {LAST_TEXT}},
  {{"MODE"}, {EXIT}, {LAST_TEXT}},
  {{"ERROR 1"}, {"ERROR 2"}, {"ERROR 3"}, {"ERROR 4"}, {"ERROR 5"}, {"ERROR 6"}, {"ERROR 7"}, {"ERROR 8"},{EXIT}, {LAST_TEXT}},
  {{"SETTING1"}, {"SETTING2"}, {EXIT}, {LAST_TEXT}},
  {{"DIP1"}, {"DIP2"}, {"DIP3"},{"DIP4"},{"DIP5"},{"DIP6"},{"DIP7"},{"DIP8"},{EXIT}, {LAST_TEXT}} 
};
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
  delay(500); // Pause for 2 seconds
  oled.clearDisplay();
  oled.print("booting...");
  oled.display(); 
  delay(800);
  oled.clearDisplay();

  memset(&this->display,0, sizeof(s_display_t));
  memset(&this->menu,0, sizeof(s_menu_t));
  memset(&this->screenSaverParameter,0, sizeof(s_screenSaverParameter_t));
  memset(&this->deviceSettings,0, sizeof(s_deviceSettingsParameter_t));

  //TODO: SETTINGS AUS EEPROM LESEN!!
  this->deviceSettings.screenSaverIsEnbaled = true;
  this->deviceSettings.sleepTime = 60000;
  this->screenSaverParameter.to_sleep.setInterval(this->deviceSettings.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->to_parmeter.setInterval(500);

  this->menu.activeMenu = menu_mainMenu;
}

//---------------------------------------------------
//MAIN ROUTINE
void OLED_MCU11::tick()
{  
  #ifdef DEBUG
  Serial.print(", ACTIVE MENU:"); Serial.print(this->menu.activeMenu);
  Serial.print(", ACTIVE TEXT: "); Serial.print(this->menu.activeText);
  Serial.print(", PARAM VALUE: "); Serial.print(this->paramValue);
  #endif

  //Display clearen, wenn neues Menü angezeigt werden soll oder bei Bildschirmschoner
  if(this->menu.activeMenu != this->menu.previousActiveMenu)
  {
    oled.clearDisplay();

    this->menu.previousActiveMenu   = this->menu.activeMenu;    
    this->menu.activeText           = 0;     
    this->paramValue                = 0;
    this->display.cursorPos         = 128;
    this->screenSaverParameter.to_sleep.reset();
    this->TEXT_OUTPUT[0]  = " ";
    this->TEXT_OUTPUT[1]  = " ";
  }  

  //Bildschirmschoner nach Timeout aktivieren
  if(this->screenSaverParameter.to_sleep.check() && this->deviceSettings.screenSaverIsEnbaled)
  {   
    this->menu.activeMenu= menu_screenSaver;  
  }

  this->showHeadlineText();

  //Daten spezifisch anzeigen, Zeile 2
  switch(this->menu.activeMenu)
  {
    case menu_mainMenu:      
      this->showMainMenu();
    break;

    case menu_deviceMode:
      this->showDeviceMode();
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

    case menu_screenSaver:
      this->showScreenSaver();    
    break; 
  }   
}

//---------------------------------------------------
//Steuerung des Menü
void OLED_MCU11::showNextTextOfThisMenu()
{
  this->menu.activeText++;
  this->screenSaverParameter.to_sleep.reset();
}

void OLED_MCU11::showPrevioursTextOfThisMenu()
{
  this->menu.activeText--;
  this->screenSaverParameter.to_sleep.reset();
}

void OLED_MCU11::enterMenu()
{    
  //Im Hauptmenü, subMenü auswählen
  if(this->menu.activeMenu == menu_mainMenu)
  {
    this->menu.activeMenu = (e_oledMenu_t)this->menu.activeText;
    this->menu.activeText = 0;
  }  

  //Zurück ins Hauptmenü
  if(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] == EXIT)
  {    
    this->menu.activeMenu = menu_mainMenu;
    this->menu.activeText = 0;
  }

  this->screenSaverParameter.to_sleep.reset();
}

e_oledMenu_t OLED_MCU11::getActiveMenu()
{
  return this->menu.activeMenu;
}

uint8_t OLED_MCU11::getActiveMenuTextNum()
{
  return this->menu.activeText;
}

void OLED_MCU11::setMenu(const e_oledMenu_t MENU)
{
  this->menu.activeMenu = MENU;
}

bool OLED_MCU11::readyToExitMenu()
{
  const bool READY = (bool)(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] == EXIT);
  return READY;
}

//---------------------------------------------------
//Text ausgeben
void OLED_MCU11::showHeadlineText()
{
  if(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] == LAST_TEXT)
  {
    this->menu.activeText--;
  }
  if(this->menu.activeText < 0)
  {
    this->menu.activeText = 0;
  }

  this->showMenuText(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText], 0);
}   

void OLED_MCU11::showMenuText(const String NEW_TEXT, const bool ROW)
{   
  //Länge des auszugebenden Textes berechnen
  int16_t textLength = NEW_TEXT.length() * -24; 
  if(textLength > 100)
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
  
  oled.setTextSize(2);

  if(this->TEXT_OUTPUT[ROW] != NEW_TEXT || (this->to_parmeter.check() && f_parmParameter == true))
  {    
    //NeuenText speichern
    this->TEXT_OUTPUT[ROW] = NEW_TEXT;
    
    //Display leeren
    oled.clearDisplay();
    //Erste Zeile 
    oled.setCursor(this->display.cursorPos, 0);
    oled.print(this->TEXT_OUTPUT[0]);     

    //Zweite Zeile  
    if(this->f_parmParameter == true)
    {
      //Bei bearbeiten von Parameter diesen Blinken lassen
      if(this->f_parameterBlink == true)
      {       
        oled.setCursor(this->display.cursorPos, 32);
        oled.print(this->TEXT_OUTPUT[1]);            
      }  
      else
      {
        //Nichts in Zeile 2 anzeigen
      }   
    } 
    else
    {     
      oled.setCursor(this->display.cursorPos, 32);
      oled.print(this->TEXT_OUTPUT[1]);       
    }   
    //Display aktaliesieren
    oled.display();    
  }

  //Blinken erzeugen
  if(this->to_parmeter.check())
  {
    this->f_parameterBlink = !this->f_parameterBlink;
    this->to_parmeter.reset();
  }

  #ifdef DEBUG
  Serial.print(", OLED OUTPUT ROW "); Serial.print(ROW); Serial.print(": "); Serial.print(NEW_TEXT);
  #endif
}

int OLED_MCU11::getMenuText(const uint8_t LAST_AVAILABLE_TEXT, const int ACTIVE_TEXT)
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
//MENU AUSGABE
void OLED_MCU11::showScreenSaver()
{ 
  //Nichts anzeigen, vielleicht ein Logo?  
}

void OLED_MCU11::showMainMenu()
{
}

void OLED_MCU11::showErrorCodes()
{
  if(this->menu.activeText < 8)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
}

void OLED_MCU11::showSettings()
{
  
}

void OLED_MCU11::showDipswitches()
{
  
}

String DEVICE_MODE[] = {{"stop"}, {"start"}, {"safe"}, {"running C1"}, {"running C2"}, {"running C3"}};
void OLED_MCU11::showDeviceMode()
{
  this->showMenuText(DEVICE_MODE[this->paramValue], 1);
}

//---------------------------------------------------
//Parametermode
void OLED_MCU11::enterParameter()
{  
  //Nur bei Parameter in Parametriermode
  if(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] != EXIT)
  {
    this->f_parmParameter = true;
  }
}

void OLED_MCU11::setParamValueToShow(const int VALUE)
{
  this->paramValue = VALUE;
}

void OLED_MCU11::exitParameter()
{
  this->f_parmParameter = false;
}

bool OLED_MCU11::parameterEntered()
{
  return this->f_parmParameter;
}