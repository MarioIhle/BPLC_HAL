#include "OLED_DISPLAY.h"
//---------------------------------------------------
//MENÜ STANDART TEXTE
const String EXIT       = "EXIT";
const String LOCK       = "LOCK";
const String LAST_TEXT  = "LAST";

//---------------------------------------------------
//CONSTRUCTOR
OLED_MCU11::OLED_MCU11()
{} 

//---------------------------------------------------
//INIT
void OLED_MCU11::begin(s_menu_t* P_MENUS, const uint8_t MENU_COUNT)
{  
  this->oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  Wire.beginTransmission(0x3C);
  const bool DEVICE_FOUND = (bool)(Wire.endTransmission() == 0);
    	   
  if(DEVICE_FOUND)
  { 
    Serial.println("Oled vorhanden");
  }
  else
  {
    Serial.println("Oled nicht vorhanden");
    return;
  }  
  
  this->oled.setTextSize(2); 
  this->oled.setTextColor(SSD1306_WHITE);
  this->oled.display();  
  this->oled.clearDisplay();
  this->oled.print("booting...");
  this->oled.display(); 
  this->oled.clearDisplay();

  memset(&this->display,0, sizeof(s_display_t));
  memset(&this->menu,0, sizeof(s_menu_t));
  memset(&this->screenSaverParameter,0, sizeof(s_screenSaverParameter_t));
  memset(&this->deviceSettings,0, sizeof(s_deviceSettingsParameter_t));

  this->menu.P_MENUS = P_MENUS;
  this->deviceSettings.screenSaverIsEnbaled = true;
  this->deviceSettings.sleepTime = 60000;
  this->screenSaverParameter.to_sleep.setInterval(this->deviceSettings.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->to_textBlink.setInterval(500);

  this->menu.activeMenu = OLED_MENU__MAIN;
}

//---------------------------------------------------
//MAIN ROUTINE
void OLED_MCU11::tick()
{  
  #ifdef DEBUG_OLED_DISPLAY
  Serial.print(", ACTIVE MENU_DEFINITION:"); Serial.print(this->menu.activeMenu);
  Serial.print(", ACTIVE TEXT: "); Serial.print(this->menu.activeText);
  Serial.print(", PARAM VALUE: "); Serial.print(this->paramValue);
  #endif

  //Display clearen, wenn neues Menü angezeigt werden soll oder bei Bildschirmschoner
  if(this->menu.activeMenu != this->menu.previousActiveMenu)
  {
    this->oled.clearDisplay();

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
    this->menu.activeMenu= OLED_MENU__SCREENSAVER;  
  }

  this->showText();
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

e_OLED_MENU_t OLED_MCU11::getActiveMenu()
{
  return this->menu.activeMenu;
}

uint8_t OLED_MCU11::getActiveTextOfThisMenu()
{
  return this->menu.activeText;
}

void OLED_MCU11::enterMenu(const e_OLED_MENU_t MENU_DEFINITION)
{
  this->menu.activeMenu = MENU_DEFINITION;
}

//---------------------------------------------------
//Text ausgeben
void OLED_MCU11::setTextToShow(const String TEXT, const bool ROW)
{
  const bool TEXT_CHANGED = (bool)(this->TEXT_OUTPUT[ROW] != TEXT);

  if(TEXT_CHANGED)
  {
     this->display.TEXT_OUTPUT[ROW] = TEXT;
     this->display.f_refresh;
  } 
}

void OLED_MCU11::blinkText(const bool ROW, const unsigned long INTERVAL = 1000)
{
  if(ROW == 0)
  {
    this->display.f_blinkRow_0 = true;
  }
  else if(ROW == 1)
  {
    this->display.f_blinkRow_1 = true;
  }
  this->display.to_textBlink.setInterval(INTERVAL);
}

void OLED_MCU11::showText()
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
  int16_t textLength = NEW_TEXT.length(); 

  if(textLength > 100)
  {
    this->display.cursorPos--;
  }
  else
  {
    this->display.cursorPos = 0;
  }
 
  if((this->display.cursorPos - 100) <= textLength)
  {
    this->display.cursorPos = 0;
  }  
    
  const bool EDIT_PARAMETER = (bool)(this->to_textBlink.check() && f_blinkRow_1 == true);

  if(EDIT_PARAMETER || this->f_refresh)
  {    
    //NeuenText speichern
    this->TEXT_OUTPUT[ROW] = NEW_TEXT;
    
    //Display leeren
    this->oled.clearDisplay();
    this->oled.setTextSize(2);
    //Erste Zeile ausgebem
    this->oled.setCursor(this->display.cursorPos, 0);
    this->oled.print(this->TEXT_OUTPUT[0]);     

    //Zweite Zeile ausgeben
    if(this->f_blinkRow_1 == true)
    {
      //Bei bearbeiten von Parameter diesen Blinken lassen
      if(this->f_parameterBlink == true)
      {       
        this->oled.setCursor(this->display.cursorPos, 32);
        this->oled.print(this->TEXT_OUTPUT[1]);            
      }  
      else
      {
        //Nichts in Zeile 2 anzeigen
      }   
    } 
    else
    {     
      this->oled.setCursor(this->display.cursorPos, 32);
      this->oled.print(this->TEXT_OUTPUT[1]);       
    }   
    //Display update
    this->oled.display();    
  }

  //Blinken erzeugen
  if(this->to_textBlink.check())
  {
    this->f_parameterBlink = !this->f_parameterBlink;
    this->to_textBlink.reset();
  }

  this->f_refresh = false;

  #ifdef DEBUG_OLED_DISPLAY
  Serial.print(", OLED OUTPUT ROW "); Serial.print(ROW); Serial.print(": "); Serial.print(NEW_TEXT);
  #endif
}

void OLED_MCU11::showScreenSaver()
{ 
  //Nichts anzeigen, vielleicht ein Logo?  
}
