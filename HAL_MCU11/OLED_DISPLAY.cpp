#include "OLED_DISPLAY.h"



//---------------------------------------------------
//CONSTRUCTOR
OLED_MCU11::OLED_MCU11()
{} 

//---------------------------------------------------
//INIT
void OLED_MCU11::begin(RotaryEncoder* P_ENCODER)
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
  this->oled.drawBitmap(0,0, Home, 128, 64, WHITE);
  this->oled.display();
  delay(5000);
  
  this->setTextToShow("booting", OLED_ROW__1);
  this->showText();



  this->p_encoder = P_ENCODER;

  this->screenSaverParameter.screenSaverIsEnbaled = true;
  this->screenSaverParameter.sleepTime = 60000;
  this->screenSaverParameter.to_sleep.setInterval(this->screenSaverParameter.sleepTime); 
  this->screenSaverParameter.to_sleep.reset(); 

  this->to_textBlink.setInterval(500);
  this->f_parameterBlink = 0;
  this->f_refresh = 0;
  this->f_blinkRow[OLED_ROW__1] = false;
  this->f_blinkRow[OLED_ROW__2] = false;
  this->f_blinkRow[OLED_ROW__3] = false;
}

//---------------------------------------------------
//MAIN ROUTINE
void OLED_MCU11::tick()
{  
  if(!this->isSceensaverActive())
  {
    this->showText();  
  }

  //Timeout abgelaufen 
  if(this->screenSaverParameter.to_sleep.check() && this->screenSaverParameter.screenSaverIsEnbaled == true)
  {
    this->showScreensaver();
  }
  //Solange userinput da ist, nicht in Screensaver
  if(this->p_encoder->isButtonPressed() || this->p_encoder->getTurningDirection() != movement_idle)
  {
    this->screenSaverParameter.to_sleep.reset(); 
    this->screenSaverParameter.screenSaverActive = false;
  }
}

//---------------------------------------------------
//Text ausgeben
void OLED_MCU11::setTextToShow(const String TEXT, const e_OLED_ROW_t ROW)
{
  const bool TEXT_CHANGED = (bool)(this->TEXT_OUTPUT[ROW] != TEXT);

  if(TEXT_CHANGED)
  {
    this->TEXT_OUTPUT[ROW] = TEXT;
    this->f_refresh = true;
  } 
}

void OLED_MCU11::clearAllTexts()
{
  for(uint8_t ROW = 0; ROW < OLED_ROW__COUNT; ROW++)
  {
    if(TEXT_OUTPUT[ROW] != "")
    {
      this->TEXT_OUTPUT[ROW] = ""; 
      this->f_refresh = true;
    }      
  }    
}

void OLED_MCU11::startBlinkText(const e_OLED_ROW_t ROW, const unsigned long INTERVAL)
{
  this->f_blinkRow[ROW] = true;
  this->to_textBlink.setInterval(INTERVAL);
}

void OLED_MCU11::stopBlinkText(const e_OLED_ROW_t ROW)
{
  this->f_blinkRow[ROW] = false;
}

void OLED_MCU11::showScreensaver()
{
  if(this->screenSaverParameter.screenSaverActive == false)
  {
    this->oled.clearDisplay();
    this->oled.display(); 
    this->screenSaverParameter.screenSaverActive = true;
  }
}

bool OLED_MCU11::isSceensaverActive()
{
  return this->screenSaverParameter.screenSaverActive;
}

void OLED_MCU11::showText()
{     
  bool REFRESH_DISPLAY = this->f_refresh;
  this->f_refresh = false;

  //Falls nicht sowieso schon display refresh angefordert, blinken prüfen
  if(REFRESH_DISPLAY == false)
  {
    for(uint8_t ROW = 0; ROW < OLED_ROW__COUNT; ROW++)
    {
      if(this->to_textBlink.check() && f_blinkRow[ROW] == true)
      {
        REFRESH_DISPLAY = true;
        break;
      }        
    }
  }
  
  if(REFRESH_DISPLAY)
  {     
    //Display leeren
    this->oled.clearDisplay();
    this->oled.setTextSize(2); 
    this->oled.startscrollleft( 2,  4);   
    //Reihe für Reihe schreiben
    for(uint8_t ROW = 0; ROW < OLED_ROW__COUNT; ROW++)
    {
      if(this->f_blinkRow[ROW] == true)
      {
        //Bei bearbeiten von Parameter diesen Blinken lassen
        if(this->f_parameterBlink == true)
        {    
          this->oled.setCursor(0, (ROW * 21));
          this->oled.print(this->TEXT_OUTPUT[ROW]);          
        }  
        else
        {
          //Nichts anzeigen
        }   
      } 
      else
      {     
        this->oled.setCursor(0, (ROW * 21));
        this->oled.print(this->TEXT_OUTPUT[ROW]);      
      }  
    }  
    //an Hardware schicken  
    this->oled.drawFastHLine(0, 16, 127, SSD1306_WHITE);
    this->oled.display();   
  }

  //Blinken erzeugen
  if(this->to_textBlink.check())
  {
    this->f_parameterBlink = !this->f_parameterBlink;
    this->to_textBlink.reset();
  }  
}