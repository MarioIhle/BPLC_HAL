#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11()
{
    this->usedPortCount = 0;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1; 
    this->usedPortCount = 1;
}
         
HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2)
{   
    this->deviceAdress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1; 
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->usedPortCount = 2;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1;
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->p_ports[AI_PORT_3] = P_PORT_3;    
    this->usedPortCount = 3;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3, AnalogInput* P_PORT_4)
{    
    this->deviceAdress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1;
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->p_ports[AI_PORT_3] = P_PORT_3;
    this->p_ports[AI_PORT_4] = P_PORT_4;    
    this->usedPortCount = 4;
}

void HAL_AIN11::begin()
{    
      // The ADC input range (or gain) can be changed via the following
  // functions, but be careful never to exceed VDD +0.3V max, or to
  // exceed the upper and lower limits if you adjust the input range!
  // Setting these values incorrectly may destroy your ADC!
  //                                                                ADS1015  ADS1115
  //                                                                -------  -------
  this->ADC.setGain(GAIN_TWOTHIRDS);    // 2/3x gain +/- 6.144V  1 bit = 3mV      0.1875mV (default)
  // ads.setGain(GAIN_ONE);             // 1x gain   +/- 4.096V  1 bit = 2mV      0.125mV
  // ads.setGain(GAIN_TWO);             // 2x gain   +/- 2.048V  1 bit = 1mV      0.0625mV
  // ads.setGain(GAIN_FOUR);            // 4x gain   +/- 1.024V  1 bit = 0.5mV    0.03125mV
  // ads.setGain(GAIN_EIGHT);           // 8x gain   +/- 0.512V  1 bit = 0.25mV   0.015625mV
  // ads.setGain(GAIN_SIXTEEN);         // 16x gain  +/- 0.256V  1 bit = 0.125mV  0.0078125mV

  this->ADC.begin(this->deviceAdress);
}

void HAL_AIN11::tick()
{   
#ifdef DEBUG_HAL_AIN11
Serial.print("AIN11 "); Serial.print(this->deviceAdress); Serial.println("VALUES:");
#endif
        for(uint8_t PORT = 0; PORT < this->usedPortCount; PORT++)
        {            
            const uint16_t  RAW_ADC_VALUE = this->ADC.readADC_SingleEnded(PORT);
            const float     VALUE_IN_VOLT = this->ADC.computeVolts(RAW_ADC_VALUE);
                            
            this->p_ports[PORT]->setPortValue(RAW_ADC_VALUE);
            this->p_ports[PORT]->setValueInVolt(VALUE_IN_VOLT);   

#ifdef DEBUG_HAL_AIN11
Serial.print(", PORT "); Serial.print(PORT); Serial.print(": "); Serial.print(RAW_ADC_VALUE); Serial.print(", IN VOLT: "); Serial.print(VALUE_IN_VOLT);    
#endif

        } 
#ifdef DEBUG_HAL_AIN11
Serial.println(" ");       
#endif  
}



