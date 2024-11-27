#include "OLED_DISPLAY.h"

OLED_STANDART_MENU::OLED_STANDART_MENU() {}
void OLED_STANDART_MENU::begin()
{
  this->errorCode = BPLC_ERROR__NO_ERROR;
  this->blinkState.setupBlink(1, 1000, 1000);
  this->to_sleep.setInterval(60000);

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
     
    this->state = OLED_STATE__SHOW_PAGE;    
  }  
  else
  {
    this->state = OLED_STATE__ERROR;
  }
}
void OLED_STANDART_MENU::tick()
{ 
  switch (this->state)
  {
    case OLED_STATE__SCREENSAVER:
      this->showScreenSaver();
    break;

    case OLED_STATE__SHOW_PAGE:      
      if (this->to_sleep.check())
      {
        this->oled.clearDisplay();
        this->state = OLED_STATE__SCREENSAVER;    
      }
      this->showPage();
    break;

    default:
    case OLED_STATE__ERROR:
    break;
  }
}
void OLED_STANDART_MENU::setPage(const s_oledStandartMenuPage_t PAGE)
{
  if(PAGE != this->menuPage)
  {
    this->menuPage      = PAGE;
    this->f_refreshPage = true;
  }  
}
void OLED_STANDART_MENU::showPage()
{
  //Blinken erzeugen
  if(this->blinkState.tickBlink() && (!this->f_blinkState))
  {
    this->f_refreshPage = true;
    this->f_blinkState  = true;
  }
  else if((!this->blinkState.tickBlink()) && this->f_blinkState)
  {
    this->f_refreshPage = true;
    this->f_blinkState  = false;
  }

  //Neuen Text ausgeben
  if(this->f_refreshPage)
  {   
    this->oled.clearDisplay();       
    
    for(uint8_t row = 0; row < 2; row++)
    {      
      //Nur Text ausgeben wenn blinken == TRUE || kein Blinkn erwümscht
      if(this->f_blinkState || (!BLINK_TEXT))
      {    
        this->showMenuText(this->menuPage.line[row], row);
      } 
    } 
    this->f_refreshPage;
  }
}
void OLED_STANDART_MENU::showText(const String TEXT, const bool ROW)
{
  this->oled.setTextSize(2);
  // Länge des auszugebenden Textes berechnen
  int16_t textLength = TEXT.length() * -24;

  if (textLength > 100)
  {
    this->cursorPos--;
  }
  else
  {
    this->cursorPos = 0;
  }

  if ((this->cursorPos - 64) <= textLength)
  {
    this->cursorPos = 0;
  }  

  this->oled.setCursor(this->cursorPos, ROW);
  this->oled.print(TEXT);         
  
  // Display aktaliesieren
  this->oled.display();
}
void OLED_STANDART_MENU::resetScreenSaver()
{
  this->to_sleep.reset();
  this->state = OLED_STATE__SHOW_PAGE;
}