#include "Controller.hpp"

Controller::Controller(/* args */) {}

Controller::~Controller() {}

bool Controller::controllerDS3231RTCInit()
{
    /* Create dS3231RTC object and init it */
    ds3231rtc = new DS3231RTC();
    if (!ds3231rtc->init())
        return false;
    return true;
}

bool Controller::controllerDigitalOutputInit()
{
    /* Create Red LED object and configure it */
    redLED = new DigitalOutput(RED_LED);
    if (!redLED->init())
    {
        return false;
    }
    /* Create Green LED object and configure it */
    greenLED = new DigitalOutput(GREEN_LED);
    if (!greenLED->init())
    {
        return false;
    }
    /* Create power sensors channel 1 object and configure it */
    powerSensorsCH1 = new DigitalOutput(POWER_SENORS_1_5_CH1);
    if (!powerSensorsCH1->init())
    {
        return false;
    }
    /* Create power sensors channel 2 object and configure it */
    powerSensorsCH2 = new DigitalOutput(POWER_SENORS_6_10_CH2);
    if (!powerSensorsCH2->init())
    {
        return false;
    }
    /* Create SHCP object and configure it */
    sh_cp = new DigitalOutput(SHCP);
    if (!sh_cp->init())
    {
        return false;
    }
    /* Create STCP object and configure it */
    st_cp = new DigitalOutput(STCP);
    if (!st_cp->init())
    {
        return false;
    }
    /* Create DS object and configure it */
    ds = new DigitalOutput(DS);
    if (!ds->init())
    {
        return false;
    }
    return true;
}

DigitalOutput *Controller::getGreenLED() const
{
    return greenLED;
}

DigitalOutput *Controller::getRedLED() const
{
    return redLED;
}

bool Controller::controllerSDCardInit()
{
    /* Create SDCard object and init it*/
    sdCard = new SDCard();
    if (!sdCard->init())
    {
        return false;
    }

    /* soilSensorMaxDrynessValue = sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY).toInt();
    soilSensorMaxWetnessValue = sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY).toInt();

    printf("...soilSensorMaxDrynessValue: %d - soilSensorMaxWetnessValue: %d\n", soilSensorMaxDrynessValue, soilSensorMaxWetnessValue); */
    return true;
}

bool Controller::controllerAht20Bmp280Init()
{
    aht20Bmp280 = new Aht20Bmp280();
    if (!aht20Bmp280->init())
    {
        mainInitError = sdCard->writeLogFile("AHT20 and BMP280 init failed.");
        return false;
    }

    mainInitError = sdCard->writeLogFile("AHT20 and BMP280 init OK.");
    return true;
}

bool Controller::controllerAnalogInputsInit()
{
    /* Create analogInputs object and configure it*/
    analogInputs = new AnalogInput();
    if (!analogInputs->init())
    {
        printf("Analog Inputs Error!\n");
        mainInitError = sdCard->writeLogFile("Analog Inputs init failed.");
        return false;
    }
    /* Analog Inputs store in array. Index 0 = Rain sensor group, Index 1-5 are wetness sensor groups*/
    /* analogInputArraySize = sizeof analogInputs->objectOfAnalogInput / sizeof analogInputs->objectOfAnalogInput[0]; */
    // printf("Analog Input Array size: %d\n", analogInputArraySize);
    mainInitError = sdCard->writeLogFile("Analog Inputs OK.");
    printf("AnalogInputs_OK.\n");
    return true;
}

bool Controller::analogSensorsThresholdToArray()
{
    if (!sdCard->saveThresholdValuesToArray(thresholdAnalogSensorsArray))
    {
        printf("Controller-Cannot store analog inputs threshold values to array Error!\n");
        mainInitError = sdCard->writeLogFile("Controller-Cannot store analog inputs threshold values to array Error!");
        return false;
    }

    thresholdSensorsValueString = "";
    for (int i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++)
    {
        thresholdSensorsValueString += String(thresholdAnalogSensorsArray[i]) + ";";
    }
    printf("Analog inputs threshold values: %s\n", thresholdSensorsValueString.c_str());
    mainInitError = sdCard->writeLogFile("Analog inputs threshold values: " + thresholdSensorsValueString);
    return true;

    /* return sdCard->saveThresholdValuesToArray(thresholdAnalogsensorsArray); */
}

int Controller::valueToPercentage(int analogInputValue_)
{
    if (analogInputValue_ <= soilSensorMaxWetnessValue)
        return 100;

    if (analogInputValue_ > soilSensorMaxDrynessValue)
        return 1;

    return map(analogInputValue_, soilSensorMaxWetnessValue, soilSensorMaxDrynessValue, 100, 0);
}

bool Controller::getSoilMaxDrynessWetnessValues()
{
    String maxDryness(sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY));
    String maxWetness(sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY));

    if (maxDryness.equals(EMPTY_STRING) || maxWetness.equals(EMPTY_STRING))
    {
        printf("Cannot get max dryness and wetness values of soil from %s.", WS_INI_FILE);
        mainInitError = sdCard->writeLogFile("Cannot get max dryness and wetness values of soil from " + String(WS_INI_FILE));
        return false;
    }
    printf("Get Max dryness: %s and wetness: %s values of soil.", maxDryness.c_str(), maxWetness.c_str());
    mainInitError = sdCard->writeLogFile("Get Max dryness: " + maxDryness + " and wetness: " + maxWetness + " values of soil.");
    soilSensorMaxDrynessValue = maxDryness.toInt();
    soilSensorMaxWetnessValue = maxWetness.toInt();
    return true;
}

void Controller::controllerReadAnalogInputPinValue(const gpio_num_t powerChannel_)
{
    printf("...Controler Power Channel: %d...!!!\n", powerChannel_);
    analogInputs->storeAnalogInputPinValue(powerChannel_, measuredValueAnalogSensorsArray);
}

void Controller::setActiveValves()
{
    valvesNumber = 0;
    measuredSensorsValueString = "";

    for (int i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++)
    {
        printf("...Sensor: %d - threshold: %d - measured: %d - ", i + 1, thresholdAnalogSensorsArray[i], measuredValueAnalogSensorsArray[i]);
        /* If threshold value is 0 then the measured value of sensor will not be used, percentage is 0% */

        if (thresholdAnalogSensorsArray[i] == 0)
        {
            measuredSensorsValueString += String(SENSOR_NOT_IN_USE) + ";";
            printf("soil wetness: %s%% - ", SENSOR_NOT_IN_USE);
        }
        else
        {
            /* Convert measured values to percentage */
            int value = valueToPercentage(measuredValueAnalogSensorsArray[i]);
            if (i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY))
            {
                measuredSensorsValueString += String(value) + ";";
                printf("soil wetness: %d%% - ", value);
                /* Store number which valves will be turned off or on */
                if (thresholdAnalogSensorsArray[i] > value && i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY))
                {
                    valvesNumber += pow(2, i);
                    printf("valveOnOff: %d", valvesNumber);
                }
            }
            else
            {
                printf("rain sensor: %d%% - ", value);
                if (thresholdAnalogSensorsArray[i] > value && i >= (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY) && i < ANALOG_DATA_ARRAY_SIZE)
                {
                    measuredSensorsValueString += RAIN_SENSOR_NOT_RAINS;
                    measuredSensorsValueString += ";";
                }
                else
                {
                    measuredSensorsValueString += RAIN_SENSOR_RAINS;
                    measuredSensorsValueString += ";";
                }
            }
        }
        printf("\n");
    }
    printf("...measuredSensorsValueString: %s\n", measuredSensorsValueString.c_str());
}

void Controller::valvesTurnOffOn()
{
    valvesBinaryString = String(valvesNumber, BIN);

    while (valvesBinaryString.length() != VALVES_BINARY_STRING_LENGHT)
    {
        valvesBinaryString = "0" + valvesBinaryString;
    }

    printf("Valves Binary String: %s\n", valvesBinaryString.c_str());

    for (int i = 0; i < SN74HC595_STEPS; i++)
    {
        st_cp->setLevel(HIGH);
        shiftOut(ds->getDigiGpioNum(), sh_cp->getDigiGpioNum(), MSBFIRST, valvesNumber);
        st_cp->setLevel(LOW);
    }
}

DigitalOutput *Controller::getPowerSensorsCH1() const
{
    return powerSensorsCH1;
}

DigitalOutput *Controller::getPowerSensorsCH2() const
{
    return powerSensorsCH2;
}

bool Controller::controllerGetAht20Bmp280Data()
{
    printf("...Date/Time: %s...!!!\n", ds3231rtc->getDateTimeNow().c_str());
    if (!aht20Bmp280->getAht20Bmp280Data(temperature, relativeHumidity, airPressure))
    {
        printf("Error occurred while reading temperature, humidity  and air pressure values from Aht20Bmp280 sensor.\n");
        mainInitError = sdCard->writeLogFile("Error occurred while reading temperature, humidity  and air pressure values from Aht20Bmp280 sensor.");
        temperature = 0.0;
        relativeHumidity = 0.0;
        airPressure = 0.0;
        return false;
    }
    printf("Aht20Bmp280 data - Temp: %.2f, Hum: %.2f, Press: %.2f\n", temperature, relativeHumidity, airPressure);
    mainInitError = sdCard->writeLogFile("Aht20Bmp280 data - Temp: " + String(temperature, 2) + ", Hum: " 
                    + String(relativeHumidity, 2) + ", Press: " + String(airPressure, 2));
    return true;
}