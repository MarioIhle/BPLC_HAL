#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11()
{
    this->usedPorts = 0;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1; 
    this->usedPorts = 1;
}
         
HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2)
{   
    this->deviceAddress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1; 
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->usedPorts = 2;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1;
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->p_ports[AI_PORT_3] = P_PORT_3;    
    this->usedPorts = 3;
}

HAL_AIN11::HAL_AIN11(const e_AIN11_ADDRESS_t ADDRESS, AnalogInput* P_PORT_1, AnalogInput* P_PORT_2, AnalogInput* P_PORT_3, AnalogInput* P_PORT_4)
{    
    this->deviceAddress = ADDRESS;

    this->p_ports[AI_PORT_1] = P_PORT_1;
    this->p_ports[AI_PORT_2] = P_PORT_2;
    this->p_ports[AI_PORT_3] = P_PORT_3;
    this->p_ports[AI_PORT_4] = P_PORT_4;    
    this->usedPorts = 4;
}

e_BPLC_ERROR_t HAL_AIN11::begin(const uint16_t READ_INTERVAL)
{    
    e_BPLC_ERROR_t error = BPLC_ERROR__NO_ERROR;
    
    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup AIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case AIN11_CARD_1:
            Serial.println("1");
        break;
        case AIN11_CARD_2:
            Serial.println("2");
        break;
        case AIN11_CARD_3:
            Serial.println("3");
        break;
        case AIN11_CARD_4:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    Serial.print("Ports defined: "); Serial.print(this->usedPorts); Serial.println("/4");
 
    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        error = AIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren         
    if(error == BPLC_ERROR__NO_ERROR)
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

        this->ADC.begin(this->deviceAddress);
        this->to_read.setInterval(READ_INTERVAL); 
   
        this->f_error = false;  
    }
    else
    {
        this->f_error = true;                      
    }

    return error;
}

void HAL_AIN11::tick()
{   
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->f_error = true;
    }
    else
    {
        //this->f_error = false;    //selbrücksetellung des Fehlerzustands, zur zeit nicht genutzt
    }

    if(!this->f_error)
    {
        if(this->to_read.check())
        {
            for(uint8_t PORT = 0; PORT < this->usedPorts; PORT++)
            {            
                const int16_t   RAW_ADC_VALUE = this->ADC.readADC_SingleEnded(this->PINS[PORT]);
                const float     VALUE_IN_VOLT = this->ADC.computeVolts(RAW_ADC_VALUE);

                if(RAW_ADC_VALUE >= 0)
                {
                    this->p_ports[PORT]->setPortValue(RAW_ADC_VALUE);
                    this->p_ports[PORT]->setValueInVolt(VALUE_IN_VOLT); 
                }     
                else
                {
                    this->p_ports[PORT]->setPortValue(0);
                    this->p_ports[PORT]->setValueInVolt(0);
                }               

                #ifdef DEBUG_HAL_AIN11
                Serial.print(", PORT "); Serial.print(PORT + 1); Serial.print(": "); Serial.print(RAW_ADC_VALUE); Serial.print(", IN VOLT: "); Serial.print(VALUE_IN_VOLT);    
                #endif

            }
            this->to_read.reset(); 
        }
    }
}

e_BPLC_ERROR_t HAL_AIN11::getError()
{
    e_BPLC_ERROR_t tempError = BPLC_ERROR__NO_ERROR;

    if(this->f_error)
    {
        tempError = BPLC_ERROR__AIN11_COMMUNICATION_FAILED;
    }
    return tempError;
}