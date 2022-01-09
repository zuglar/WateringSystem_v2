#include "Controller.hpp"

Controller::Controller(/* args */) {}

Controller::~Controller() {}

bool Controller::controllerDS3231RTCInit() {
    /* Create dS3231RTC object and init it */
    ds3231rtc = new DS3231RTC();
    if (!ds3231rtc->init())
        return false;
    return true;
}

bool Controller::controllerDigitalOutputInit() {
    /* Create Red LED object and configure it */
    redLED = new DigitalOutput(RED_LED);
    if (!redLED->init()) {
        return false;
    }
    /* Create Green LED object and configure it */
    greenLED = new DigitalOutput(GREEN_LED);
    if (!greenLED->init()) {
        return false;
    }
    /* Create power sensors channel 1 object and configure it */
    powerSensorsCH1 = new DigitalOutput(POWER_SENORS_1_5_CH1);
    if (!powerSensorsCH1->init()) {
        return false;
    }
    /* Create power sensors channel 2 object and configure it */
    powerSensorsCH2 = new DigitalOutput(POWER_SENORS_6_10_CH2);
    if (!powerSensorsCH2->init()) {
        return false;
    }
    /* Create SHCP object and configure it */
    sh_cp = new DigitalOutput(SHCP);
    if (!sh_cp->init()) {
        return false;
    }
    /* Create STCP object and configure it */
    st_cp = new DigitalOutput(STCP);
    if (!st_cp->init()) {
        return false;
    }
    /* Create DS object and configure it */
    ds = new DigitalOutput(DS);
    if (!ds->init()) {
        return false;
    }
    return true;
}

DigitalOutput *Controller::getGreenLED() const {
    return greenLED;
}

DigitalOutput *Controller::getRedLED() const {
    return redLED;
}

bool Controller::controllerSDCardInit() {
    /* Create SDCard object and init it*/
    sdCard = new SDCard();
    if (!sdCard->init()) {
        return false;
    }

    /* soilSensorMaxDrynessValue = sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY).toInt();
    soilSensorMaxWetnessValue = sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY).toInt();

    printf("...soilSensorMaxDrynessValue: %d - soilSensorMaxWetnessValue: %d\n", soilSensorMaxDrynessValue, soilSensorMaxWetnessValue); */
    return true;
}

bool Controller::controllerAht20Bmp280Init() {
    aht20Bmp280 = new Aht20Bmp280();
    if (!aht20Bmp280->init()) {
        mainAppError = sdCard->writeLogFile("AHT20 and BMP280 init failed.");
        return false;
    }

    mainAppError = sdCard->writeLogFile("AHT20 and BMP280 init OK.");
    return true;
}

bool Controller::controllerAnalogInputsInit() {
    /* Create analogInputs object and configure it*/
    analogInputs = new AnalogInput();
    if (!analogInputs->init()) {
        printf("Analog Inputs Error!\n");
        mainAppError = sdCard->writeLogFile("Analog Inputs init failed.");
        return false;
    }
    /* Analog Inputs store in array. Index 0 = Rain sensor group, Index 1-5 are wetness sensor groups*/
    /* analogInputArraySize = sizeof analogInputs->objectOfAnalogInput / sizeof analogInputs->objectOfAnalogInput[0]; */
    // printf("Analog Input Array size: %d\n", analogInputArraySize);
    mainAppError = sdCard->writeLogFile("Analog Inputs OK.");
    printf("Analog Inputs OK.\n");
    return true;
}

bool Controller::analogSensorsThresholdTValues() {
    if (!sdCard->getValueFromIni(THRESHOLDVALUES_SECTION, THRESHOLD_LIMIT_KEY, thresholdValues)) {
        printf("ERROR - Controller-Cannot store analog inputs threshold values!\n");
        mainAppError = sdCard->writeLogFile("ERROR - Controller-Cannot store analog inputs threshold values!");
        return false;
    }

    /* thresholdSensorsValueString = "";
    for (int i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++)
    {
        thresholdSensorsValueString += String(thresholdAnalogSensorsArray[i]) + ";";
    } */
    printf("Analog Inputs threshold values: %s\n", thresholdValues.c_str());
    mainAppError = sdCard->writeLogFile("Analog Inputs threshold values: " + thresholdValues);
    return true;

    /* return sdCard->saveThresholdValuesToArray(thresholdAnalogsensorsArray); */
}

int Controller::valueToPercentage(int analogInputValue_) {
    if (analogInputValue_ <= maxWetness)
        return 100;

    if (analogInputValue_ > maxDryness)
        return 1;

    return map(analogInputValue_, maxWetness, maxDryness, 100, 0);
}

bool Controller::getSystemGlobalValues() {
    String *value = new String();
    if (!sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY, *value)) {
        printf("Cannot get max dryness value of soil from %s.\n", WS_INI_FILE);
        mainAppError = sdCard->writeLogFile("Cannot get max dryness value of soil from " + String(WS_INI_FILE));
        delete value;
        return false;
    }
    maxDryness = value->toInt();
    printf("Max dryness value of soil: %d.\n", maxDryness);
    mainAppError = sdCard->writeLogFile("Max dryness value of soil: " + *value);
    /* Remove stored data */
    value->remove(0, value->length());

    if (!sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY, *value)) {
        printf("Cannot get max wetness value of soil from %s.\n", WS_INI_FILE);
        mainAppError = sdCard->writeLogFile("Cannot get max wetness value of soil from " + String(WS_INI_FILE));
        delete value;
        return false;
    }
    maxWetness = value->toInt();
    printf("Max wetness value of soil: %d.\n", maxWetness);
    mainAppError = sdCard->writeLogFile("Max wetness value of soil: " + *value);
    /* Remove stored data */
    value->remove(0, value->length());

    if (!sdCard->getValueFromIni(SENSORS_VALUE_INTERVAL_SECTION, INTERVAL_KEY, *value)) {
        printf("Cannot get value of interval of checking sensors from %s.\n", WS_INI_FILE);
        mainAppError = sdCard->writeLogFile("Cannot get value of interval of checking sensors from " + String(WS_INI_FILE));
        delete value;
        return false;
    }
    refreshSensorsInterval = value->toInt();
    printf("Refresh interval value of checking the sensors: %d.\n", refreshSensorsInterval);
    mainAppError = sdCard->writeLogFile("Refresh interval value of checking the sensors: " + *value);

    delete value;

    return true;
}

void Controller::controllerReadAnalogInputPinValue(const gpio_num_t powerChannel_) {
    printf("Controler Power Channel: %d switched on.\n", powerChannel_);
    mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(powerChannel_) + " switched on.");
    analogInputs->storeAnalogInputPinValue(powerChannel_, measuredValueAnalogSensorsArray);
}

void Controller::setActiveValves() {
    valvesNumber = 0;
    measuredSensorsValueString = "";

    for (int i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++) {
        // printf("...Sensor: %d - threshold: %d - measured: %d - ", i + 1, thresholdAnalogSensorsArray[i], measuredValueAnalogSensorsArray[i]);
        /* If threshold value is 0 then the measured value of sensor will not be used, percentage is 0% */

        if (thresholdAnalogSensorsArray[i] == 0) {
            measuredSensorsValueString += String(SENSOR_NOT_IN_USE) + ";";
            // printf("soil wetness: %s%% - ", SENSOR_NOT_IN_USE);
        } else {
            /* Convert measured values to percentage */
            int value = valueToPercentage(measuredValueAnalogSensorsArray[i]);
            if (i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
                measuredSensorsValueString += String(value) + ";";
                // printf("soil wetness: %d%% - ", value);
                /* Store number which valves will be turned off or on */
                if (thresholdAnalogSensorsArray[i] > value && i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
                    valvesNumber += pow(2, i);
                    // printf("valveOnOff: %d", valvesNumber);
                }
            } else {
                // printf("rain sensor: %d%% - ", value);
                if (thresholdAnalogSensorsArray[i] > value && i >= (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY) && i < ANALOG_DATA_ARRAY_SIZE) {
                    measuredSensorsValueString += RAIN_SENSOR_NOT_RAINS;
                    measuredSensorsValueString += ";";
                } else {
                    measuredSensorsValueString += RAIN_SENSOR_RAINS;
                    measuredSensorsValueString += ";";
                }
            }
        }
        // printf("\n");
    }
    printf("Analog Inputs measured values: %s\n", measuredSensorsValueString.c_str());
    mainAppError = sdCard->writeLogFile("Analog Inputs measured values: " + measuredSensorsValueString);

    valvesBinaryString = String(valvesNumber, BIN);

    while (valvesBinaryString.length() != VALVES_BINARY_STRING_LENGHT) {
        valvesBinaryString = "0" + valvesBinaryString;
    }

    printf("Active valves binary mode: %s\n", valvesBinaryString.c_str());
    mainAppError = sdCard->writeLogFile("Active valves binary mode: " + valvesBinaryString);
}

void Controller::valvesTurnOffOn() {
    /* valvesBinaryString = String(valvesNumber, BIN);

    while (valvesBinaryString.length() != VALVES_BINARY_STRING_LENGHT)
    {
        valvesBinaryString = "0" + valvesBinaryString;
    } */

    for (int i = 0; i < SN74HC595_STEPS; i++) {
        st_cp->setLevel(HIGH);
        shiftOut(ds->getDigiGpioNum(), sh_cp->getDigiGpioNum(), MSBFIRST, valvesNumber);
        st_cp->setLevel(LOW);
    }

    /* printf("Active valves binary mode: %s\n", valvesBinaryString.c_str());
    mainAppError = sdCard->writeLogFile("Active valves binary mode: " + valvesBinaryString); */
}

DigitalOutput *Controller::getPowerSensorsCH1() const {
    return powerSensorsCH1;
}

DigitalOutput *Controller::getPowerSensorsCH2() const {
    return powerSensorsCH2;
}

DS3231RTC *Controller::getDs3231rtc() const {
    return ds3231rtc;
}

SDCard *Controller::getSdCard() const {
    return sdCard;
}

bool Controller::controllerGetAht20Bmp280Data() {
    /* printf("Date/Time: %s\n", ds3231rtc->getDateTimeNow().c_str()); */
    temperature = 0.0;
    relativeHumidity = 0.0;
    airPressure = 0.0;
    if (!aht20Bmp280->getAht20Bmp280Data(temperature, relativeHumidity, airPressure)) {
        printf("Error occurred while reading temperature, humidity  and air pressure values from Aht20Bmp280 sensor.\n");
        mainAppError = sdCard->writeLogFile("Error occurred while reading temperature, humidity  and air pressure values from Aht20Bmp280 sensor.");
        temperature = 0.0;
        relativeHumidity = 0.0;
        airPressure = 0.0;
        return false;
    }
    printf("Aht20Bmp280 data - Temp: %.2f, Hum: %.2f, Press: %.2f\n", temperature, relativeHumidity, airPressure);
    mainAppError = sdCard->writeLogFile("Aht20Bmp280 data - Temp: " + String(temperature, 2) + ", Hum: " + String(relativeHumidity, 2) + ", Press: " + String(airPressure, 2));
    return true;
}

bool Controller::controllerWiFi32sInit() {
    String *apHidden = new String();
    String *apSSID = new String();
    String *apPwd = new String();
    String *apChannel = new String();
    String *apMaxConnection = new String();

    if (!(sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_HIDDEN_KEY, *apHidden) && sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_SSID_KEY, *apSSID) &&
          sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_PWD_KEY, *apPwd) && sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_CHANNEL_KEY, *apChannel) &&
          sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_MAX_CONNECTION_KEY, *apMaxConnection))) {
        printf("ERROR - Cannot import Access Point data from ws.ini file. Exits from WiFi configuration.\n");
        mainAppError = sdCard->writeLogFile("ERROR - Cannot import Access Point data from ws.ini file. Exits from WiFi configuration.");
        delete apHidden;
        delete apSSID;
        delete apPwd;
        delete apChannel;
        delete apMaxConnection;
        return false;
    }

    printf("Access Point values have been imported from ws.ini file..\n");
    mainAppError = sdCard->writeLogFile("Access Point values have been imported from ws.ini file.");

    String *staEnabled = new String();
    String *staSSID = new String();
    String *staPwd = new String();
    String *staStaticIpEnabled = new String();
    String *staIP = new String();
    String *staSubnet = new String();
    String *staGateway = new String();
    String *staDns = new String();
    bool staConfiguration = false;

    if (!(sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STASET_KEY, *staEnabled) && sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SSID_KEY, *staSSID) &&
          sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_PWD_KEY, *staPwd) && sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, *staStaticIpEnabled) &&
          sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_IP_KEY, *staIP) && sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, *staSubnet) &&
          sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, *staGateway) && sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_DNS_KEY, *staDns))) {
        printf("ERROR - Cannot import Station / Router values from ws.ini file.\n");
        mainAppError = sdCard->writeLogFile("ERROR - Cannot import Station / Router values from ws.ini file.");
        delete staEnabled;
        delete staSSID;
        delete staPwd;
        delete staStaticIpEnabled;
        delete staIP;
        delete staSubnet;
        delete staGateway;
        delete staDns;
    } else {
        printf("Station / Router values have been imported from ws.ini file..\n");
        mainAppError = sdCard->writeLogFile("Station / Router values have been imported from ws.ini file.");
        staConfiguration = true;
    }

    wifi32s = new WiFi32s(this);

    if (staConfiguration) {
        if (!wifi32s->init(apHidden->toInt(), apSSID->c_str(), apPwd->c_str(), apChannel->toInt(), apMaxConnection->toInt(),
                           staEnabled->toInt(), staSSID->c_str(), staPwd->c_str(), staStaticIpEnabled->toInt(), staIP->c_str(),
                           staSubnet->c_str(), staGateway->c_str(), staDns->c_str())) {
            delete apHidden;
            delete apSSID;
            delete apPwd;
            delete apChannel;
            delete apMaxConnection;
            delete staEnabled;
            delete staSSID;
            delete staPwd;
            delete staStaticIpEnabled;
            delete staIP;
            delete staSubnet;
            delete staGateway;
            delete staDns;
            return false;
        }
    } else {
        if (!wifi32s->init(apHidden->toInt(), apSSID->c_str(), apPwd->c_str(), apChannel->toInt(), apMaxConnection->toInt(),
                           0, NULL, NULL, NULL, NULL, NULL, NULL, NULL)) {
            delete apHidden;
            delete apSSID;
            delete apPwd;
            delete apChannel;
            delete apMaxConnection;
            return false;
        }
    }
    /*
        if (!wifi32s->init(sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_HIDDEN_KEY).toInt(),
                           sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_SSID_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_PWD_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_CHANNEL_KEY).toInt(),
                           sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_MAX_CONNECTION_KEY).toInt(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STASET_KEY).toInt(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SSID_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_PWD_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY).toInt(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_IP_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY).c_str(),
                           sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_DNS_KEY).c_str())) {
            return false;
        }
     */
    wifi32s->startWebHtm();
    printf("Web Htm Started.\n");
    mainAppError = sdCard->writeLogFile("Web Htm Started.");

    wifi32s->startFTPServer();
    return true;
}

void Controller::controllerGetKeysValuesRules() {
    keysOfWateringRules = EMPTY_STRING;
    valuesOfKeysOfRules = EMPTY_STRING;
    sdCard->getKeysValuesFromSection(RULES_SECTION, keysOfWateringRules, valuesOfKeysOfRules);
    printf("keysOfWatteringRules: %s\n", keysOfWateringRules.c_str());
    printf("valuesOfKeysOfRules: %s\n", valuesOfKeysOfRules.c_str());
}
