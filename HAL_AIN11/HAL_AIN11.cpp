#include "HAL_AIN11.h"

HAL_AIN11::HAL_AIN11()
{
    memset(&this->ports, 0, sizeof(this->ports));
}

void HAL_AIN11::begin(const e_AIN11_ADDRESS_t I2C_ADDRESS, const uint16_t READ_INTERVAL)
{    
    this->deviceAddress = I2C_ADDRESS;

    //Debug Error ausgabe
    Serial.println("##############################");  
    Serial.println("setup AIN11 ");

    Serial.print("CARD: ");
    switch(this->deviceAddress)
    {
        case AIN11_CARD_1_ADDRESS:
            Serial.println("1");
        break;
        case AIN11_CARD_2_ADDRESS:
            Serial.println("2");
        break;
        case AIN11_CARD_3_ADDRESS:
            Serial.println("3");
        break;
        case AIN11_CARD_4_ADDRESS:
            Serial.println("4");
        break;
    }
    //Tatsächliche I2C Addresse ausgeben
    Serial.print("address: 0x"); Serial.println(this->deviceAddress, HEX);

    this->selfCheck.begin(this->deviceAddress);
    if(this->selfCheck.checkI2CConnection())
    {
        Serial.println("I2C connection ok!");
    }
    else
    {
        Serial.println("I2C connection failed!");
        this->errorCode = AIN11_ERROR__I2C_CONNECTION_FAILED;        
    }

    //Applikationsparameter initialisieren         
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
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
    }
}

void HAL_AIN11::mapObjectToNextFreePort(AnalogInput* P_OBJECT)
{  
    for(uint8_t PORT = 0; PORT < AIN11_PORT__COUNT; PORT++)
    {
        if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
        {
            this->ports.p_object[PORT] = P_OBJECT;
            this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT && PORT == AIN11_PORT__4)
        {
            this->errorCode = AIN11_ERROR__PORT_OVERFLOW;
        }
    }
}

void HAL_AIN11::mapObjectToSpecificPort(AnalogInput* P_OBJECT, const uint8_t PORT)
{
    if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE)
    {
        this->ports.p_object[PORT] = P_OBJECT;
        this->ports.used[PORT]     = PORT_USEAGE__MAPPED_TO_OBJECT;
    }
    else 
    {
        this->errorCode = AIN11_ERROR__PORT_ALREADY_DEFINED;
    }
}
//Applikation
void HAL_AIN11::tick()
{   
    //I2C Verbindung zyklisch prüfen
    if(!this->selfCheck.requestHeartbeat())
    {
        this->errorCode = AIN11_ERROR__I2C_CONNECTION_FAILED;
    }
    //Prüfen ob überhaupt ein Port in benutzung
    for(uint8_t PORT = 0; PORT < AIN11_PORT__COUNT; PORT++)
    {            
        if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
        {
            break;
        }
        else if(this->ports.used[PORT] == PORT_USEAGE__NOT_IN_USE && PORT == (AIN11_PORT__COUNT - 1))
        {//letzter Port und immernoch keiner in nutzung
            this->errorCode = AIN11_ERROR__NO_PORT_IN_USE;
        }
    }
  
    if(this->errorCode == BPLC_ERROR__NO_ERROR)
    {
        if(this->to_read.check())
        {
            for(uint8_t PORT = 0; PORT < AIN11_PORT__COUNT; PORT++)
            {            
                if(this->ports.used[PORT] == PORT_USEAGE__MAPPED_TO_OBJECT)
                {
                    const int16_t   RAW_ADC_VALUE = this->ADC.readADC_SingleEnded(this->ports.PIN[PORT]);
                    const float     VALUE_IN_VOLT = this->ADC.computeVolts(RAW_ADC_VALUE);

                    if(RAW_ADC_VALUE >= 0)
                    {
                        this->ports.p_object[PORT]->setPortValue(RAW_ADC_VALUE);
                        this->ports.p_object[PORT]->setValueInVolt(VALUE_IN_VOLT); 
                    }     
                    else
                    {
                        this->ports.p_object[PORT]->setPortValue(0);
                        this->ports.p_object[PORT]->setValueInVolt(0);
                    }               

                }                
            }
            this->to_read.reset(); 
        }
    }
}

e_BPLC_ERROR_t HAL_AIN11::getError()
{
    return this->errorCode;
}