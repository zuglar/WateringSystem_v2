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
    /* Create SDCard object and init it */
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

bool Controller::analogSensorsThresholdValues() {
    String *thresholdValues = new String();
    sdCard->getValueFromIni(THRESHOLDVALUES_SECTION, THRESHOLD_KEY, *thresholdValues);
    if (*thresholdValues == EMPTY_STRING) {
        printf("ERROR - Threshold values are empty!\n");
        mainAppError = sdCard->writeLogFile("ERROR - Threshold values are empty!");
        delete thresholdValues;
        return false;
    }

    // sdCard->getValueFromIni(THRESHOLDVALUES_SECTION, THRESHOLD_KEY, thresholdValues);
    printf("Analog Inputs threshold values: %s\n", thresholdValues->c_str());
    mainAppError = sdCard->writeLogFile("Analog Inputs threshold values: " + *thresholdValues);

    // Store threshold values to int array
    char *copy = strdup(thresholdValues->c_str());
    char *found;
    int i = 0;
    while ((found = strsep(&copy, DELIMITER)) != NULL) {
        // Serial.println(found);
        thresholdAnalogSensorsArray[i] = strtoul(found, NULL, 10);
        i++;
    }
    delete thresholdValues;
    return true;
}

int Controller::valueToPercentage(int analogInputValue_) {
    if (analogInputValue_ <= maxWetness)
        return 100;

    if (analogInputValue_ > maxDryness)
        return 1;

    return map(analogInputValue_, maxWetness, maxDryness, 100, 0);
}

bool Controller::getSystemGlobalValues() {
    String value;
    // Stores max dryness
    sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY, value);
    maxDryness = value.toInt();
    printf("Max dryness value of soil: %d.\n", maxDryness);
    mainAppError = sdCard->writeLogFile("Max dryness value of soil: " + value);
    value = String();
    // Stores max wetness
    sdCard->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY, value);
    maxWetness = value.toInt();
    printf("Max wetness value of soil: %d.\n", maxWetness);
    mainAppError = sdCard->writeLogFile("Max wetness value of soil: " + value);
    value = String();
    // Stores interval time
    sdCard->getValueFromIni(TIME_INTERVAL_CHECKING_SECTION, INTERVAL_KEY, value);
    systemRefreshInterval = (uint32_t)value.toInt();
    printf("Refresh interval value of checking the sensors: %d.\n", systemRefreshInterval);
    mainAppError = sdCard->writeLogFile("Refresh interval value of checking the sensors: " + value);

    if (maxDryness > 4095 || maxWetness < 1 || systemRefreshInterval < INTERVAL_MIN_VALUE)
        return false;

    return true;
}

void Controller::valvesTurnOffOn(uint8_t valves) {
    for (int i = 0; i < SN74HC595_STEPS; i++) {
        st_cp->setLevel(HIGH);
        shiftOut(ds->getDigiGpioNum(), sh_cp->getDigiGpioNum(), MSBFIRST, valves);
        st_cp->setLevel(LOW);
    }
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
    String apHidden;
    sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_HIDDEN_KEY, apHidden);
    String apSSID;
    sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_SSID_KEY, apSSID);
    String apPwd;
    sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_PWD_KEY, apPwd);
    String apChannel;
    sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_CHANNEL_KEY, apChannel);
    String apMaxConnection;
    sdCard->getValueFromIni(WIFI_AP_SECTION, WIFI_MAX_CONNECTION_KEY, apMaxConnection);

    String staEnabled;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STASET_KEY, staEnabled);
    String staSSID;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SSID_KEY, staSSID);
    String staPwd;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_PWD_KEY, staPwd);
    String staStaticIpEnabled;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, staStaticIpEnabled);
    String staIP;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_IP_KEY, staIP);
    String staSubnet;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, staSubnet);
    String staGateway;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, staGateway);
    String staDns;
    sdCard->getValueFromIni(WIFI_STA_SECTION, WIFI_DNS_KEY, staDns);
    String ddnsEnabled;
    sdCard->getValueFromIni(WIFI_DDNS_SECTION, WIFI_DDNSSET_KEY, ddnsEnabled);
    String ddnsProvider;
    sdCard->getValueFromIni(WIFI_DDNS_SECTION, WIFI_DDNSPROVIDER_KEY, ddnsProvider);
    String ddnsHost;
    sdCard->getValueFromIni(WIFI_DDNS_SECTION, WIFI_DDNSHOST_KEY, ddnsHost);
    String ddnsUserName;
    sdCard->getValueFromIni(WIFI_DDNS_SECTION, WIFI_DDNSUSERNAME_KEY, ddnsUserName);
    String ddnsUserPwd;
    sdCard->getValueFromIni(WIFI_DDNS_SECTION, WIFI_DDNSPASSWORD_KEY, ddnsUserPwd);
    
    wifi32s = new WiFi32s(this);

    if (staEnabled.toInt()) {
        if (!wifi32s->init(apHidden.toInt(), apSSID.c_str(), apPwd.c_str(), apChannel.toInt(), apMaxConnection.toInt(),
                           staEnabled.toInt(), staSSID.c_str(), staPwd.c_str(), staStaticIpEnabled.toInt(), staIP.c_str(),
                           staSubnet.c_str(), staGateway.c_str(), staDns.c_str(), ddnsEnabled.toInt(), ddnsProvider.c_str(),
                           ddnsHost.c_str(), ddnsUserName.c_str(), ddnsUserPwd.c_str())) {
            return false;
        }
    } else {
        if (!wifi32s->init(apHidden.toInt(), apSSID.c_str(), apPwd.c_str(), apChannel.toInt(), apMaxConnection.toInt(),
                           0, NULL, NULL, NULL, NULL, NULL, NULL, NULL, ddnsEnabled.toInt(), ddnsProvider.c_str(),
                           ddnsHost.c_str(), ddnsUserName.c_str(), ddnsUserPwd.c_str())) {
            return false;
        }
    }

    wifi32s->startWebHtm();
    printf("Web Htm Started.\n");
    mainAppError = sdCard->writeLogFile("Web Htm Started.");

    wifi32s->startFTPServer();
    return true;
}

bool Controller::controllerPrepareWatering() {
    // Gets number of keys
    keysNum = sdCard->getNumKeysInSection(WATERING_RULES_SECTION);

    // Allocate memory for ruleNames
    ruleNames = (char **)malloc(keysNum * sizeof(char *));
    for (uint8_t i = 0; i < keysNum; i++) {
        ruleNames[i] = (char *)malloc(RULE_NAME_BUFFER * sizeof(char));
    }

    if (!sdCard->getKeysArray(WATERING_RULES_SECTION, ruleNames)) {
        return false;
    }

    // Variable to store rule value
    String ruleValue;
    // Gets unixDateTime now  from DS3231
    uint32_t unixDateTimeNow = getDs3231rtc()->getUnixTimeNow();
    // Gets unixDate from DS3231-unixtime
    uint32_t unixDateNow = unixDateTimeNow - (unixDateTimeNow % 86400);
    // Variable to store active rule name
    char *activeRuleName = NULL;

    // Variable to store start time
    uint32_t startTime = 0;
    // Variable to store end time
    uint32_t endTime = 0;
    // Variable to store next start time
    uint32_t nextStartTime = 0;

    char *true_ = "1";

    valvesDecValue = 0;
    checkSoilWetness = false;
    checkRainSensor = false;
    checkTemperature = false;
    activeRuleExists = false;
    wateringDurationTime = 0;
    wateringTimer1Interrupt = 0;

    // printf("Unix Date Now: %d\n", unixDateNow);

    for (uint8_t i = 0; i < keysNum; i++) {
        ruleValue = EMPTY_STRING;
        sdCard->getValueFromIni(WATERING_RULES_SECTION, ruleNames[i], ruleValue);
        // printf("Rule name: %s - Value: %s\n", ruleNames[i], ruleValue.c_str());

        char *copy = strdup(ruleValue.c_str());
        char *found;
        uint8_t j = 0;

        while ((found = strsep(&copy, DELIMITER)) != NULL) {
            if (j == 0) {  // Checks start date
                if (unixDateNow < (uint32_t)(strtoul(found, NULL, 10)))
                    break;
            } else if (j == 1) {  // Checks end date
                if (unixDateNow > (uint32_t)(strtoul(found, NULL, 10)))
                    break;
            } else if (j == 2) {  // Checks start time
                uint32_t value = (uint32_t)strtoul(found, NULL, 10);
                value = value + unixDateNow;
                if (value <= unixDateTimeNow) {
                    startTime = value;
                    // printf("**************** %s - startTime: %d\n", ruleNames[i], startTime);
                } else {
                    if (nextStartTime == 0) {
                        nextStartTime = value;
                    } else {
                        if (nextStartTime > value) {
                            nextStartTime = value;
                        }
                    }
                    // printf("**************** %s - nextStartTime: %d\n", ruleNames[i], nextStartTime);
                    break;
                }
            } else if (j == 3) {  // Checks end time
                uint32_t value = (uint32_t)(strtoul(found, NULL, 10));
                value = value + unixDateNow;
                if (value >= unixDateTimeNow) {
                    endTime = value;
                    // printf("**************** %s - endTime: %d\n", ruleNames[i], endTime);
                } else if (startTime <= unixDateTimeNow && value <= unixDateTimeNow) {
                    if (nextStartTime == 0) {
                        nextStartTime = startTime + AFTER_MIDNIGHT;
                    } else {
                        if (nextStartTime > startTime + AFTER_MIDNIGHT) {
                            nextStartTime = startTime + AFTER_MIDNIGHT;
                        }
                    }
                    // printf("****************NEXT DAY: %s - nextStartTime: %d\n", ruleNames[i], nextStartTime);
                    break;
                }
            } else if (j == 4) {  // Store decimal value of active valves
                valvesDecValue = (uint8_t)strtol(found, NULL, 10);
            } else if (j == 5) {  // Store - bool - check soil wetness before watering
                checkSoilWetness = found && (strcmp(found, true_) == 0);
            } else if (j == 6) {  // Store - bool - check rain sensor - if rains then stops watering
                checkRainSensor = found && (strcmp(found, true_) == 0);
            } else if (j == 7) {  // Store - bool - check temperature during watering - if temperature is above or below the given values stops watering
                checkTemperature = found && (strcmp(found, true_) == 0);
            } else if (j == 8) {  // Store - low temperaure - if temperature is below the given value stops watering
                lowTemperature = (int8_t)atoi(found);
            } else if (j == 9) {  // Store - high temperaure - if temperature is above the given value stops watering
                highTemperature = (int8_t)atoi(found);
                activeRuleName = strdup(ruleNames[i]);
                activeRuleExists = true;
                break;
            }
            j++;
        }
    }

    // Freeing alocated memory for ruleNames
    for (uint8_t i = 0; i < keysNum; i++)
        free(ruleNames[i]);

    free(ruleNames);


    if (activeRuleExists) {
        wateringDurationTime = endTime - unixDateTimeNow;

        printf(
            "ACTIVE RULE: %s, Start time: %02d:%02d:%02d, End time: %02d:%02d:%02d, Valves: %d, Check soil wetness: %d, Check the rain: %d,"
            "Check temperatue: %d, Low temperature: %d, High temperature: %d, Duration time %d seconds\n",
            activeRuleName, ((startTime % 86400) / 3600), ((startTime % 3600) / 60), (startTime % 60), ((endTime % 86400) / 3600),
            ((endTime % 3600) / 60), (endTime % 60), valvesDecValue, checkSoilWetness, checkRainSensor, checkTemperature, lowTemperature,
            highTemperature, wateringDurationTime);

        mainAppError = sdCard->writeLogFile("ACTIVE RULE: " + String(activeRuleName) + ", Start time: " + String(((startTime % 86400) / 3600)) + ":" +
                                            String(((startTime % 3600) / 60)) + ":" + String(startTime % 60) + ", End time: " + String(((endTime % 86400) / 3600)) + ":" +
                                            String(((endTime % 3600) / 60)) + ":" + String(endTime % 60) + ", Valves: " + String(valvesDecValue) + ", Check soil wetness: " +
                                            String(checkSoilWetness) + ", Check the rain: " + String(checkRainSensor) + ", Check temperatue: " + String(checkTemperature) +
                                            ", Low temperature: " + String(lowTemperature) + ", High temperature: " + String(highTemperature) + ", Duration time: " +
                                            String(wateringDurationTime) + " seconds.");
        controllerCheckWateringRules();
    } else {
        wateringDurationTime = nextStartTime - unixDateTimeNow;
        printf("NO ACTIVE RULE - Next watering rule in: %d days, Time: %02d:%02d:%02d. Total seconds: %d\n", (wateringDurationTime / 86400),
               ((wateringDurationTime % 86400) / 3600), ((wateringDurationTime % 3600) / 60), (wateringDurationTime % 60), wateringDurationTime);

        mainAppError = sdCard->writeLogFile("NO ACTIVE RULE - Next watering rule in: " + String((wateringDurationTime / 86400)) + " days, Time: " +
                                            String(((wateringDurationTime % 86400) / 3600)) + ":" + String((wateringDurationTime % 3600) / 60) + ":" +
                                            String(wateringDurationTime % 60) + ". Total seconds: " + String(wateringDurationTime));

        newValvesDecValue = ALL_VALVES_OFF;
        valvesTurnOffOn(newValvesDecValue);
    }
    return true;
}

void Controller::controllerCheckWateringRules() {
    newValvesDecValue = valvesDecValue;

    if (!checkSoilWetness && !checkRainSensor && !checkTemperature) {
        valvesTurnOffOn(newValvesDecValue);
        return;
    }

    this->controllerGetSensorsValue();

    if (checkRainSensor) {
        if (measuredSensorsValueString.endsWith("1")) {
            newValvesDecValue = ALL_VALVES_OFF;
            valvesTurnOffOn(newValvesDecValue);
            return;
        }
    }

    if (checkTemperature) {
        this->controllerGetAht20Bmp280Data();
        if ((int8_t)this->temperature <= this->lowTemperature || (int8_t)this->temperature >= this->highTemperature) {
            newValvesDecValue = ALL_VALVES_OFF;
            valvesTurnOffOn(newValvesDecValue);
            return;
        }
    }

    if (checkSoilWetness) {
        ///////////////////////////////////
        // return;
        char binResult[9];
        newValvesDecValue = ALL_VALVES_OFF;
        strcpy(binResult, String(valvesDecValue, BIN).c_str());
        // printf("binResult: %s\n", binResult);
        uint8_t value = 0;

        size_t lenght = strlen(binResult);
        for (int i = lenght - 1, j = 0; i >= 0; i--, j++) {
            // Serial.print(binResult[i]);
            if (binResult[i] == '1') {
                value = valueToPercentage(measuredValueAnalogSensorsArray[j]);
                if ((uint8_t)thresholdAnalogSensorsArray[j] > value) {
                    // printf("j: %d - threshold[j]: %d - measured[j] %d - value: %d\n", j, thresholdAnalogSensorsArray[j], measuredValueAnalogSensorsArray[i], value);
                    newValvesDecValue += (uint8_t)(pow(2, j) + 0.5);
                }
            }
        }
        // printf("newValvesDecValue: %d\n", newValvesDecValue);
        valvesTurnOffOn(newValvesDecValue);
        return;
    }

    valvesTurnOffOn(newValvesDecValue);
}

void Controller::controllerGetSensorsValue() {
    measuredSensorsValueString = "";

    getPowerSensorsCH1()->setLevel(HIGH);
    printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_1_5_CH1);
    delay(DELAY_05_SEC);
    // mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(POWER_SENORS_1_5_CH1) + " switched on.");
    analogInputs->storeAnalogInputPinValue(POWER_SENORS_1_5_CH1, measuredValueAnalogSensorsArray);
    getPowerSensorsCH1()->setLevel(LOW);
    getPowerSensorsCH2()->setLevel(HIGH);
    printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_6_10_CH2);
    delay(DELAY_05_SEC);
    // mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(POWER_SENORS_6_10_CH2) + " switched on.");
    analogInputs->storeAnalogInputPinValue(POWER_SENORS_6_10_CH2, measuredValueAnalogSensorsArray);
    getPowerSensorsCH2()->setLevel(LOW);

    uint8_t value = 0;
    for (uint8_t i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++) {
        value = 0;
        // If the wetness threshold value is 0 then the measured value of sensor will not be used, percentage is 0%
        if (thresholdAnalogSensorsArray[i] == 0) {
            if (i < ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)
                measuredSensorsValueString += String(SENSOR_NOT_IN_USE) + ";";
        } else {
            // Convert measured values to percentage
            value = valueToPercentage(measuredValueAnalogSensorsArray[i]);
            // Save wetness sensors
            if (i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
                measuredSensorsValueString += String(value) + ";";
            } else {
                // Save rain sensor
                if (thresholdAnalogSensorsArray[i] > value) {
                    measuredSensorsValueString += RAIN_SENSOR_NOT_RAINS;
                } else {
                    measuredSensorsValueString += RAIN_SENSOR_RAINS;
                }
            }
        }
        // printf("Sensor: %d - Measured: %d - Threshold: %d - Percentage: %d\n", i + 1, measuredValueAnalogSensorsArray[i], thresholdAnalogSensorsArray[i], value);
    }

    printf("measuredSensorsValueString: %s\n", measuredSensorsValueString.c_str());
    mainAppError = sdCard->writeLogFile("Measured values of analog sensors in percentage: " + measuredSensorsValueString);
}

// void Controller::controllerReadAnalogInputValue(const gpio_num_t powerChannel_) {
//     printf("Controler Power Channel: %d switched on.\n", powerChannel_);
//     mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(powerChannel_) + " switched on.");
//     analogInputs->storeAnalogInputPinValue(powerChannel_, measuredValueAnalogSensorsArray);
// }

// void Controller::setActiveValves() {
//     valvesNumber = 0;
//     measuredSensorsValueString = "";

//     for (int i = 0; i < ANALOG_DATA_ARRAY_SIZE; i++) {
//         printf("...Sensor: %d - threshold: %d - measured: %d\n", i + 1, thresholdAnalogSensorsArray[i], measuredValueAnalogSensorsArray[i]);
//         /* If threshold value is 0 then the measured value of sensor will not be used, percentage is 0% */

//         if (thresholdAnalogSensorsArray[i] == 0) {
//             measuredSensorsValueString += String(SENSOR_NOT_IN_USE) + ";";
//             // printf("soil wetness: %s%% - ", SENSOR_NOT_IN_USE);
//         } else {
//             /* Convert measured values to percentage */
//             int value = valueToPercentage(measuredValueAnalogSensorsArray[i]);
//             if (i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
//                 measuredSensorsValueString += String(value);// + ";";
//                 // printf("soil wetness: %d%% - ", value);
//                 /* Store number which valves will be turned off or on */
//                 if (thresholdAnalogSensorsArray[i] > value && i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
//                     valvesNumber += pow(2, i);
//                     // printf("valveOnOff: %d", valvesNumber);
//                 }
//             } else {
//                 // printf("rain sensor: %d%% - ", value);
//                 if (thresholdAnalogSensorsArray[i] > value && i >= (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY) && i < ANALOG_DATA_ARRAY_SIZE) {
//                     measuredSensorsValueString += RAIN_SENSOR_NOT_RAINS;
//                     // measuredSensorsValueString += ";";
//                 } else {
//                     measuredSensorsValueString += RAIN_SENSOR_RAINS;
//                     // measuredSensorsValueString += ";";
//                 }
//             }

//             if (i < (ANALOG_DATA_ARRAY_SIZE - RAIN_SENSORS_QUANTITY)) {
//                 measuredSensorsValueString += ";";
//             }

//         }
//         // printf("\n");
//     }
//     printf("Analog Inputs measured values: %s\n", measuredSensorsValueString.c_str());
//     mainAppError = sdCard->writeLogFile("Analog Inputs measured values: " + measuredSensorsValueString);

//     valvesBinaryString = String(valvesNumber, BIN);

//     while (valvesBinaryString.length() != VALVES_BINARY_STRING_LENGHT) {
//         valvesBinaryString = "0" + valvesBinaryString;
//     }

//     printf("Active valves binary mode: %s\n", valvesBinaryString.c_str());
//     mainAppError = sdCard->writeLogFile("Active valves binary mode: " + valvesBinaryString);
// }

/*
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

    wifi32s->startWebHtm();
    printf("Web Htm Started.\n");
    mainAppError = sdCard->writeLogFile("Web Htm Started.");

    wifi32s->startFTPServer();
    return true;
} */

// unsigned long previousMillis = 0;
// const long interval = 1000;  // 500ms = 0.5s
// // bool start = true;
// bool channelOn = true;
// uint8_t channelNum = 1;
// // gets measured value of wetness and rain sensors / channel
// while (true) {
//     unsigned long currentMillis = millis();

//     if (currentMillis - previousMillis >= interval) {
//         // save the last time you blinked the LED
//         previousMillis = currentMillis;

//         if (channelOn) {
//             if (channelNum == 1) {
//                 getPowerSensorsCH1()->setLevel(HIGH);
//                 printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_1_5_CH1);
//                 // mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(POWER_SENORS_1_5_CH1) + " switched on.");
//                 analogInputs->storeAnalogInputPinValue(POWER_SENORS_1_5_CH1, measuredValueAnalogSensorsArray);
//             } else {
//                 getPowerSensorsCH2()->setLevel(HIGH);
//                 printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_6_10_CH2);
//                 // mainAppError = sdCard->writeLogFile("Controler Power Channel: " + String(POWER_SENORS_6_10_CH2) + " switched on.");
//                 analogInputs->storeAnalogInputPinValue(POWER_SENORS_6_10_CH2, measuredValueAnalogSensorsArray);
//             }
//             channelOn = false;
//         } else {
//             if (channelNum == 1) {
//                 getPowerSensorsCH1()->setLevel(LOW);
//                 channelNum++;
//                 channelOn = true;
//             } else {
//                 getPowerSensorsCH2()->setLevel(LOW);
//                 break;
//             }
//         }
//     }
// }
