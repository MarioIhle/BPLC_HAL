#include "OLED_DISPLAY.h"
//---------------------------------------------------
// MENÜ STANDART TEXTE
const String EXIT = "EXIT";
const String LOCK = "LOCK";
const String LAST_TEXT = "LAST";

String HEADLINE_TEXT[menu_count][10] =
    {
        {{"MODE"}, {"EDIT MODE"}, {"BPLC ERROR"}, {"vDIP"}, {"SETUP"}, {LOCK}, {LAST_TEXT}},
        {{"EDIT MODE"}, {EXIT}, {LAST_TEXT}},
        {{"BPLC ERROR"}, {EXIT}, {LAST_TEXT}},
        {{"DIP1"}, {"DIP2"}, {"DIP3"}, {"DIP4"}, {"DIP5"}, {"DIP6"}, {"DIP7"}, {"DIP8"}, {EXIT}, {LAST_TEXT}},
        {{"BUZZER"}, {"MOT11_1 TEACHIN"}, {EXIT}, {LAST_TEXT}}};
//---------------------------------------------------
// CONSTRUCTOR
OLED_MCU11::OLED_MCU11() {}

//---------------------------------------------------
// INIT
void OLED_MCU11::begin()
{
  this->errorCode = BPLC_ERROR__NO_ERROR;

  if(!I2C_check::begin(0x3C))
  {
    this->errorCode = BPLC_ERROR__OLED_COMMUNICATION_FAILED;
  }

  if(this->errorCode == BPLC_ERROR__NO_ERROR)
  {
    // Library inititalisieren
    this->oled.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    this->oled.setTextSize(2);
    this->oled.setTextColor(SSD1306_WHITE);
    this->oled.display();
    this->oled.clearDisplay();
    this->oled.print("booting...");
    this->oled.display();
    this->oled.clearDisplay();

    memset(&this->display, 0, sizeof(s_display_t));
    memset(&this->menu, 0, sizeof(s_menu_t));
    memset(&this->screenSaverParameter, 0, sizeof(s_screenSaverParameter_t));
    memset(&this->deviceSettings, 0, sizeof(s_deviceSettingsParameter_t));

    this->deviceSettings.screenSaverIsEnbaled = true;
    this->deviceSettings.sleepTime = 60000;
    this->screenSaverParameter.to_sleep.setInterval(this->deviceSettings.sleepTime);
    this->screenSaverParameter.to_sleep.reset();

    this->to_parmeter.setInterval(500);

    this->menu.activeMenu = menu_mainMenu;    
  }  
}

//---------------------------------------------------
// MAIN ROUTINE
void OLED_MCU11::tick()
{
#ifdef DEBUG_OLED_DISPLAY
  Serial.print(", ACTIVE MENU:");
  Serial.print(this->menu.activeMenu);
  Serial.print(", ACTIVE TEXT: ");
  Serial.print(this->menu.activeText);
  Serial.print(", PARAM VALUE: ");
  Serial.print(this->paramValue);
#endif

  // Display clearen, wenn neues Menü angezeigt werden soll oder bei Bildschirmschoner
  if (this->menu.activeMenu != this->menu.previousActiveMenu)
  {
    this->oled.clearDisplay();

    this->menu.previousActiveMenu = this->menu.activeMenu;
    this->menu.activeText = 0;
    this->paramValue = 0;
    this->display.cursorPos = 128;
    this->screenSaverParameter.to_sleep.reset();
    this->TEXT_OUTPUT[0] = " ";
    this->TEXT_OUTPUT[1] = " ";
  }

  //Nur im Mainmenü Bildschirmschoner zulassen
  if(this->menu.activeMenu != menu_mainMenu)
  {
    this->screenSaverParameter.to_sleep.reset();
  }
  // Bildschirmschoner nach Timeout aktivieren
  if (this->screenSaverParameter.to_sleep.check() && this->deviceSettings.screenSaverIsEnbaled)
  {
    this->menu.activeMenu = menu_screenSaver;
  }

  this->showHeadlineText();

  // Daten spezifisch anzeigen, Zeile 2
  switch (this->menu.activeMenu)
  {
  case menu_mainMenu:
    this->showMainMenu();
    break;

  case menu_deviceMode:
    this->showDeviceMode();
    break;

  case menu_errorCodes:
    this->showHardwareErrorCode();
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
// Steuerung des Menü
void OLED_MCU11::showNextTextOfThisMenu()
{
  this->menu.activeText++;
}

void OLED_MCU11::showPrevioursTextOfThisMenu()
{
  this->menu.activeText--;
}

void OLED_MCU11::enterMenu()
{
  // Im Hauptmenü, subMenü auswählen
  if (this->menu.activeMenu == menu_mainMenu)
  {
    this->menu.activeMenu = (e_oledMenu_t)this->menu.activeText;
    this->menu.activeText = 0;
  }

  // Zurück ins Hauptmenü
  if (HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] == EXIT)
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
// Text ausgeben
void OLED_MCU11::showHeadlineText()
{
  if (HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] == LAST_TEXT)
  {
    this->menu.activeText--;
  }
  if (this->menu.activeText < 0)
  {
    this->menu.activeText = 0;
  }

  this->showMenuText(HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText], 0);
}

void OLED_MCU11::showMenuText(const String NEW_TEXT, const bool ROW)
{
  // Länge des auszugebenden Textes berechnen
  int16_t textLength = NEW_TEXT.length() * -24;
  if (textLength > 100)
  {
    this->display.cursorPos--;
  }
  else
  {
    this->display.cursorPos = 0;
  }

  if ((this->display.cursorPos - 64) <= textLength)
  {
    this->display.cursorPos = 0;
  }

  this->oled.setTextSize(2);

  if (this->TEXT_OUTPUT[ROW] != NEW_TEXT || (this->to_parmeter.check() && f_parmParameter == true) || this->f_refresh)
  {
    // NeuenText speichern
    this->TEXT_OUTPUT[ROW] = NEW_TEXT;

    // Display leeren
    this->oled.clearDisplay();
    // Erste Zeile
    this->oled.setCursor(this->display.cursorPos, 0);
    this->oled.print(this->TEXT_OUTPUT[0]);

    // Zweite Zeile
    if (this->f_parmParameter == true)
    {
      // Bei bearbeiten von Parameter diesen Blinken lassen
      if (this->f_parameterBlink == true)
      {
        this->oled.setCursor(this->display.cursorPos, 32);
        this->oled.print(this->TEXT_OUTPUT[1]);
      }
      else
      {
        // Nichts in Zeile 2 anzeigen
      }
    }
    else
    {
      this->oled.setCursor(this->display.cursorPos, 32);
      this->oled.print(this->TEXT_OUTPUT[1]);
    }
    // Display aktaliesieren
    this->oled.display();
  }

  // Blinken erzeugen
  if (this->to_parmeter.check())
  {
    this->f_parameterBlink = !this->f_parameterBlink;
    this->to_parmeter.reset();
  }

  this->f_refresh = false;

#ifdef DEBUG_OLED_DISPLAY
  Serial.print(", OLED OUTPUT ROW ");
  Serial.print(ROW);
  Serial.print(": ");
  Serial.print(NEW_TEXT);
#endif
}

uint8_t OLED_MCU11::getMenuText(const uint8_t LAST_AVAILABLE_TEXT, const uint8_t ACTIVE_TEXT)
{
  uint8_t text = ACTIVE_TEXT;
  if (text > LAST_AVAILABLE_TEXT)
  {
    text = LAST_AVAILABLE_TEXT;
  }
  else if (text < 0)
  {
    text = 0;
  }
  return text;
}

//---------------------------------------------------
// MENU AUSGABE
// Texte 2. Zeile
String DEVICE_MODE[] = {{"init"}, {"stop"}, {"running"}, {"safestate"}};

void OLED_MCU11::showScreenSaver()
{
  // Nichts anzeigen, vielleicht ein Logo?
}

void OLED_MCU11::showMainMenu()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(DEVICE_MODE[this->paramValue], 1);
  }
  else
  {
    this->showMenuText("", 1);
  }
}

void OLED_MCU11::showHardwareErrorCode()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
  else
  {
    this->showMenuText("", 1);
  }  
}

void OLED_MCU11::showSettings()
{
  if (this->menu.activeText == 0)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
  else
  {
    this->showMenuText("", 1);
  }  
}

void OLED_MCU11::showDipswitches()
{
  if (this->menu.activeText < 8)
  {
    this->showMenuText(String(this->paramValue, DEC), 1);
  }
}

void OLED_MCU11::showDeviceMode()
{
  this->showMenuText(DEVICE_MODE[this->paramValue], 1);
}

//---------------------------------------------------
// Parametermode
void OLED_MCU11::enterParameter()
{
  // Nur bei Parameter in Parametriermode
  if (HEADLINE_TEXT[this->menu.activeMenu][this->menu.activeText] != EXIT)
  {
    this->f_parmParameter = true;
  }
}

void OLED_MCU11::setParamValueToShow(const uint8_t VALUE)
{
  this->paramValue = VALUE;
}

void OLED_MCU11::exitParameter()
{
  this->f_parmParameter = false;
  this->f_refresh = true;
}

bool OLED_MCU11::parameterEntered()
{
  return this->f_parmParameter;
}

e_BPLC_ERROR_t  OLED_MCU11::getError()
{
  return this->errorCode;
}