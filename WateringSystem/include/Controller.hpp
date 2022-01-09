/* Controller header */
#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include "DefsVarsFuncs.hpp"
#include "DigitalOutput.hpp"
#include "DS3231RTC.hpp"
#include "SDCard.hpp"
#include "Aht20Bmp280.hpp"
#include "AnalogInput.hpp"
#include "WiFi32s.hpp"

class WiFi32s;

class Controller
{
private:
    /* data */
    /* DigitalOutput objects */
    DigitalOutput *redLED;
    DigitalOutput *greenLED;
    DigitalOutput *powerSensorsCH1;
    DigitalOutput *powerSensorsCH2;
    DigitalOutput *sh_cp, *st_cp, *ds;
    /* DS3231 object */
    DS3231RTC *ds3231rtc;
    /* SDCard object */
    SDCard *sdCard;
    /* Aht20Bmp280 object */
    Aht20Bmp280 *aht20Bmp280;
    /* Analog Input object */
    AnalogInput *analogInputs;
    
    /* Private property - To store activ valves to integer*/
    int valvesNumber;
    /* Private method - Convert analog input value to percentage */
    int valueToPercentage(int analogInputValue_);

public:
    /* Constructor */
    Controller(/* args */);
    /* Destructor */
    ~Controller();
    /* Public method - Configure Digital Outputs */
    bool controllerDigitalOutputInit();
    /* Public method - Configure DS3231RTC */
    bool controllerDS3231RTCInit();
    /* Public method - Configure SDCard */
    bool controllerSDCardInit();
    /* Public method - Configure Aht20Bmp280 */
    bool controllerAht20Bmp280Init();
    /* Public method - Configure Analog Inputs */
    bool controllerAnalogInputsInit();
    /* Public method - Save Analog Sensors threshold value to array */
    bool analogSensorsThresholdTValues();
    /* Public method - Read values of analog inputs and store in an array */
    void controllerReadAnalogInputPinValue(const gpio_num_t powerChannel_);
    /* Public method - To store/get max dryness and wetness values of soil and refresh interval of system from ws.ini file */
    bool getSystemGlobalValues();
    /* Public method - Set value of valvesNumber. Calculate which valves will be turn On or Off */
    void setActiveValves();
    /* Public method - Turn Off/On watering valves */
    void valvesTurnOffOn();
    /* Public method - get data from Aht20Bmp280 sensors*/
    bool controllerGetAht20Bmp280Data();
    /* Public method - Configure and init WiFi32s */
    bool controllerWiFi32sInit();
    /* Public method - Get keys and values of watering rules */
    void controllerGetKeysValuesRules();

    /* Getter Green Led Digital Output */
    DigitalOutput *getGreenLED() const;
    /* Getter Red Led Digital Output */
    DigitalOutput *getRedLED() const;
    /* Getter - Power Sensors Channel 1 Digital Output */
    DigitalOutput *getPowerSensorsCH1() const;
    /* Getter - Power Sensors Channel 2 Digital Output */
    DigitalOutput *getPowerSensorsCH2() const;
    /* Getter - sdCard object */
    SDCard *getSdCard() const;
    /* Getter - ds3231rtc object */
    DS3231RTC *getDs3231rtc() const;

    /* Public property - To store Analog Sensors threshold values from ws.ini file to string */
    String thresholdValues;
    /* Public property - To store measured values of Analog Sensors to String */
    String measuredSensorsValueString;
    /* Public property - To store binary value of active valves to String */
    String valvesBinaryString;
    /* Public property - Array to store measured values of rain and wetness sensors */
    int measuredValueAnalogSensorsArray[ANALOG_DATA_ARRAY_SIZE];
    /* Public property - Array to store threshold values of rain and wetness sensors */
    /////////////////////////////////////////////////
    int thresholdAnalogSensorsArray[ANALOG_DATA_ARRAY_SIZE];
    /* Public property - To store value of temperature measured by Aht20Bmp280 sensor */
    float temperature;
    /* Public property - To store value of humidity measured by Aht20Bmp280 sensor */
    float relativeHumidity;
    /* Public property - To store value of air pressure measured by Aht20Bmp280 sensor */
    float airPressure;
    /* Public property - To store/get value of refresh interval of sensors from ini file */
    int refreshSensorsInterval;
    /* Public property - To store/get value of max dryness of soil from ini file */
    int maxDryness;
    /* Public property - To store/get value of max wetness of soil from ini file */
    int maxWetness;


    /* Public property - To store keys of watering rules  from ini file */
    String keysOfWateringRules;
    /* Public property - To store values of keys of watering rules from ini file */
    String valuesOfKeysOfRules;
    


    /* WiFi32s object */
    WiFi32s *wifi32s;
};

#endif /* __CONTROLLER_HPP__ */
