#include "OLED_DISPLAY.h"

OLED_STANDART_MENU::OLED_STANDART_MENU() 
{}
void OLED_STANDART_MENU::begin()
{
  this->to_coolDown.setInterval(2000);
  this->blinkState.setupBlink(1, 5000, 1000);

  if(!I2C_check::begin(0x3C))
  {
    this->setError(BPLC_ERROR__OLED_COMMUNICATION_FAILED, __FILENAME__, __LINE__);
  }

  if(this->getError()->errorCode == BPLC_ERROR__NO_ERROR)
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
  if(this->getError()->errorCode == BPLC_ERROR__NO_ERROR)
  {
    this->showPage();
  }
}
void OLED_STANDART_MENU::setPage(s_oledStandartMenuPage_t PAGE)
{
  for(uint8_t row = 0; row < 2; row++)
  {      
    if(PAGE.line[row].text != this->menuPage.line[row].text)
    {
      this->menuPage      = PAGE;

      if(to_coolDown.checkAndReset())
      {
        this->f_refreshPage = true;
      }      
    }  
  }
}
void OLED_STANDART_MENU::showPage()
{
  const bool BLINK_TEXT_ROW_1 = this->menuPage.line[ROW_1].f_blink;
  const bool BLINK_TEXT_ROW_2 = this->menuPage.line[ROW_2].f_blink;

  //Blinken erzeugen
  if(this->blinkState.tickBlink() && (!this->f_blinkState))
  {
    if(BLINK_TEXT_ROW_1 || BLINK_TEXT_ROW_2)
    {
      this->f_refreshPage = true;
    }    
    this->f_blinkState  = true;
  }
  else if((!this->blinkState.tickBlink()) && this->f_blinkState)
  {
    if(BLINK_TEXT_ROW_1 || BLINK_TEXT_ROW_2)
    {
      this->f_refreshPage = true;
    } 
    this->f_blinkState  = false;
  }

  //Neuen Text ausgeben
  if(this->f_refreshPage)
  {   
    this->oled.clearDisplay();       
    
    for(uint8_t row = 0; row < 2; row++)
    {      
      //Nur Text ausgeben wenn blinken == TRUE || kein Blinkn erwümscht
      if(this->f_blinkState || (!this->menuPage.line[row].f_blink))
      {    
        this->showText(this->menuPage.line[row].text, row);
      } 
    } 
    // Display aktualiesieren
    this->oled.display(); 
    this->f_refreshPage;
  }
}
void OLED_STANDART_MENU::showText(const String TEXT, const uint8_t ROW)
{  
  // Länge des auszugebenden Textes berechnen
  /*
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
  */
  this->oled.setTextSize(2);
  int16_t row = ((SCREEN_HEIGHT/ROW_COUNT)*ROW);
  this->oled.setCursor(0,  row);  
  this->oled.print(TEXT);          
}