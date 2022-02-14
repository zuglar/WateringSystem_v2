#include "WiFi32s.hpp"

volatile bool updateNewSettingsProccess;

size_t onStaticDownLoad(uint8_t *buffer, size_t maxLen, size_t index) {
    asyncTcpWdt = true;
    static char *filename = WIFI_HTM_FILE;
    static File chunkfile;
    static size_t dnlLen = 2048;     // limit to buffer size
    uint32_t countbytes = 0;  // bytes to be returned
    if (index == 0) {
        printf("onStaticDownLoad: START filename: %s index: %6i maxLen: %5i\n", filename, index, maxLen);
        chunkfile = SD.open(filename);
    }
    if(chunkfile) {
        printf("chunkfile opened\n");
    }
    chunkfile.seek(index);
    countbytes = chunkfile.read(buffer, min(dnlLen, maxLen));

    if (countbytes == 0) {
        chunkfile.close();
        printf("chunkfile closed\n");
    }
    printf("onStaticDownLoad: f: %s index: %6u maxLen: %5u countb: %5u \n", filename, index, maxLen, countbytes);

    return countbytes;
}

WiFi32s::WiFi32s(/* args */ Controller *cntrl_) {
    cntrl = cntrl_;
}

WiFi32s::~WiFi32s() {}

bool WiFi32s::init(int apHidden_, const char *apSSID_, const char *apPWD_, int apChannel_, int apMaxConnection_, int staSet_,
                   const char *staSSID_, const char *staPWD_, int staStaticIp_, const char *staIP_, const char *staSubnet_,
                   const char *staGateway_, const char *staDNS_) {
    bool result = false;

    staEnabled = staSet_;
    staStaticIP = staStaticIp_;

    // WiFi mode to Access Point and to Station
    WiFi.mode(WIFI_AP_STA);
    // Begin Access Point
    if (!WiFi.softAP(apSSID_, apPWD_, apChannel_, apHidden_, apMaxConnection_)) {
        printf("Failed to configure Access Point!\n");
        // sdCard->writeLogFile("Failed to configure Access Point");
        mainAppError = cntrl->getSdCard()->writeLogFile("Failed to configure Access Point.");
    } else {
        apIPString = WiFi.softAPIP().toString();
        printf("ESP32 AP created. IP: %s\n", apIPString.c_str());
        mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 AP created. SSID: " + String(apSSID_) + ", IP: " + WiFi.softAPIP().toString());
        result = true;
    }

    // Configures static IP address
    if (staStaticIP == 1) {
        bool staticNetworkData = false;
        staIP = new IPAddress();
        staSubnet = new IPAddress();
        staGateway = new IPAddress();
        staPrimaryDNS = new IPAddress();

        // Set local Static IP address
        if (stringToIPAdress(staIP_, staIP)) {
            // Set local subnet
            if (stringToIPAdress(staSubnet_, staSubnet)) {
                // Set local gateway
                if (stringToIPAdress(staGateway_, staGateway)) {
                    // Set primary DNS
                    if (strlen(staDNS_) != 0) {
                        staticNetworkData = stringToIPAdress(staDNS_, staPrimaryDNS);
                    } else {
                        staticNetworkData = stringToIPAdress(WIFI_NO_DNS_DATA, staPrimaryDNS);
                    }
                }
            }
        }

        if (staticNetworkData) {
            // Configures static IP address
            if (!WiFi.config(*staIP, *staGateway, *staSubnet, *staPrimaryDNS)) {
                printf("STA failed to configure static network data!\n");
                mainAppError = cntrl->getSdCard()->writeLogFile("STA failed to configure static network data.");
                delete staIP;
                delete staGateway;
                delete staSubnet;
                delete staPrimaryDNS;
            } else {
                printf("STA static network data has been configured.\n");
                mainAppError = cntrl->getSdCard()->writeLogFile("STA static network data has been configured.");
                result = true;
            }
        }
    }

    // Begin Station WiFi
    if (staEnabled == 1) {
        WiFi.begin(staSSID_, staPWD_);
        // Wait for connection
        uint8_t i = 0;
        while (WiFi.status() != WL_CONNECTED && i++ < 20) {
            delay(500);
            printf(".");
        }
        printf("\n");

        if (i == 21) {
            printf("ESP32 could not connect to: %s !\n", staSSID_);
            mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 could not connect to: " + String(staSSID_));
        } else {
            staIPString = WiFi.localIP().toString();
            printf("ESP32 connected to: %s, IP: %s\n", staSSID_, staIPString.c_str());
            mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 connected to: " + String(staSSID_) + ", IP: " + staIPString);
            result = true;
        }
    }

    htmlFileMemoryAllocated = false;

    return result;
}

bool WiFi32s::stringToIPAdress(const char *data_, IPAddress *address_) {
    if (!address_->fromString(data_))
        return false;

    return true;
}

void WiFi32s::startWebHtm() {
    // Opens index.htm file

    // server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
    //     asyncTcpWdt = true;
    //     logWebTraffic(request, EMPTY_STRING);
    //     sendResponseToClient(request, 200, INDEX_HTM_FILE);
    // });

    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        logWebTraffic(request, EMPTY_STRING);
        request->sendChunked("text/html", onStaticDownLoad);
    });
    // Sends data to show values of weather, of wetness of soil and of state of valves and rain sensor
    server.on("/getWeather", HTTP_GET, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        logWebTraffic(request, EMPTY_STRING);
        cntrl->controllerGetAht20Bmp280Data();
        cntrl->controllerGetSensorsValue();
        // Simple option to create json
        // AsyncResponseStream *response = request->beginResponseStream("application/json");
        // DynamicJsonDocument json(1024);
        // json["temp"] = "\"" + String(cntrl->temperature, 2) + "\"";
        // json["hum"] = "\"" + String(cntrl->relativeHumidity, 2) + "\"";
        // json["atm"] = "\"" + String((cntrl->airPressure / 1000), 2) + "\"";
        // json["valves"] = 79;
        // json["sensors"] = "100;50;20;10;30;60;70;80;1";
        // serializeJson(json, *response);

        AsyncJsonResponse *response = new AsyncJsonResponse();
        // response->addHeader("Access-Control-Allow-Origin", "*");
        // response->addHeader("Access-Control-Max-Age", "600");
        // response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        // response->addHeader("Access-Control-Allow-Headers", "*");
        JsonObject root = response->getRoot();
        root["temp"] = String(cntrl->temperature, 2);
        root["hum"] = String(cntrl->relativeHumidity, 2);
        root["atm"] = String((cntrl->airPressure / 1000), 2);
        root["valves"] = cntrl->newValvesDecValue;
        root["sensors"] = cntrl->measuredSensorsValueString;

        jsonOutput = String();
        serializeJson(root, jsonOutput);
        logWebTraffic(request, jsonOutput);

        response->setLength();
        request->send(response);
    });
    // Opens wifi.htm file
    server.on("/wifi.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        logWebTraffic(request, EMPTY_STRING);
        sendResponseToClient(request, 200, WIFI_HTM_FILE);
    });
    // Sends data to show values of wifi settings
    server.on("/getWiFi", HTTP_GET, [this](AsyncWebServerRequest *request) {
        logWebTraffic(request, EMPTY_STRING);
        String dynDns = String();
        AsyncJsonResponse *response = new AsyncJsonResponse();
        // response->addHeader("Access-Control-Allow-Origin", "*");
        // response->addHeader("Access-Control-Max-Age", "600");
        // response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        // response->addHeader("Access-Control-Allow-Headers", "*");
        JsonObject root = response->getRoot();
        DynamicJsonDocument wifiArrayDoc(1024);
        JsonArray wifiArray = wifiArrayDoc.to<JsonArray>();
        wifiArray.add(WiFi.softAPSSID());
        wifiArray.add(WiFi.softAPIP().toString());
        // wifiArray.add(String(staStaticIP));
        wifiArray.add(WiFi.localIP().toString());
        wifiArray.add(WiFi.subnetMask().toString());
        wifiArray.add(WiFi.gatewayIP().toString());
        wifiArray.add(WiFi.dnsIP().toString());
        // wifiArray.add(String(staEnabled));
        wifiArray.add(WiFi.SSID());

        root["wifi"] = wifiArrayDoc;
        // char buffer[5000];
        // serializeJson(root, buffer);
        // printf("BUFFER: %s\n", buffer);
        jsonOutput = String();
        serializeJson(root, jsonOutput);
        logWebTraffic(request, jsonOutput);
        response->setLength();
        request->send(response);
    });
    // Opens rules.htm file
    server.on("/rules.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        logWebTraffic(request, EMPTY_STRING);
        sendResponseToClient(request, 200, RULES_HTM_FILE);
    });
    // Sends data to show values of global settings in rule.htm
    server.on("/getGlobalData", HTTP_GET, [this](AsyncWebServerRequest *request) {
        AsyncJsonResponse *response = new AsyncJsonResponse();
        // response->addHeader("Access-Control-Allow-Origin", "*");
        // response->addHeader("Access-Control-Max-Age", "600");
        // response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        // response->addHeader("Access-Control-Allow-Headers", "*");
        JsonObject root = response->getRoot();

        String threshold;
        String wetness;
        String dryness;
        String interval;
        uint8_t numOfKeys = cntrl->getSdCard()->getNumKeysInSection(WATERING_RULES_SECTION);

        logWebTraffic(request, "NUM_OF_KEYS = " + String(numOfKeys));

        DynamicJsonDocument ruleArrayDoc(1024);
        JsonArray ruleArray = ruleArrayDoc.to<JsonArray>();

        if (numOfKeys == 0) {
            logWebTraffic(request, "Couldn't find rule name. Base rule with name FirstRule created.");
            uint32_t unixDateNow = cntrl->getDs3231rtc()->getUnixTimeNow();

            unixDateNow = unixDateNow - (unixDateNow % 86400);

            char firstRuleValue[RULE_VALUE_BUFFER] = "";
            String firstvalue = String(unixDateNow) + ";" + String(unixDateNow + 15778458) + ";0;0;0;0;0;0;-40;50";
            strncpy(firstRuleValue, firstvalue.c_str(), RULE_VALUE_BUFFER);
            cntrl->getSdCard()->saveValueToIni(WATERING_RULES_SECTION, "FirstRule", firstRuleValue);
            logWebTraffic(request, "Base rule value: " + firstvalue + " added to FirstRule.");
            numOfKeys = 1;
        }

        char **keysArray;
        keysArray = (char **)malloc(numOfKeys * sizeof(char *));  // Allocate row pointers
        for (int i = 0; i < numOfKeys; i++)
            keysArray[i] = (char *)malloc(11 * sizeof(char));  // Allocateeach row separately

        cntrl->getSdCard()->getKeysArray(WATERING_RULES_SECTION, keysArray);

        for (int i = 0; i < numOfKeys; i++) {
            ruleArray.add(keysArray[i]);
        }

        cntrl->getSdCard()->getValueFromIni(THRESHOLDVALUES_SECTION, THRESHOLD_KEY, threshold);
        cntrl->getSdCard()->getValueFromIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY, wetness);
        cntrl->getSdCard()->getValueFromIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY, dryness);
        cntrl->getSdCard()->getValueFromIni(TIME_INTERVAL_CHECKING_SECTION, INTERVAL_KEY, interval);

        root["rulename"] = ruleArrayDoc;
        root["threshold"] = threshold;
        root["wetness"] = wetness;
        root["dryness"] = dryness;
        root["interval"] = interval;

        // char buffer[5000];
        // serializeJson(root, buffer);
        // printf("BUFFER: %s\n", buffer);

        jsonOutput = String();
        serializeJson(root, jsonOutput);
        logWebTraffic(request, jsonOutput);
        response->setLength();

        for (int i = 0; i < numOfKeys; i++)
            free(keysArray[i]);

        free(keysArray);

        request->send(response);
    });
    // Sends data to show values of rule settings in rule.htm
    server.on("/getRuleValue", HTTP_GET, [this](AsyncWebServerRequest *request) {
        if (request->hasParam("ruleName", false) && request->getParam("ruleName", false)->value() != "") {
            String rulevalue;
            const String rulename = request->getParam("ruleName", false)->value();
            AsyncJsonResponse *response = new AsyncJsonResponse();
            JsonObject root = response->getRoot();

            cntrl->getSdCard()->getValueFromIni(WATERING_RULES_SECTION, rulename, rulevalue);

            root["rulevalue"] = rulevalue;

            jsonOutput = String();
            serializeJson(root, jsonOutput);
            logWebTraffic(request, jsonOutput);
            response->setLength();
            request->send(response);
        }
    });
    // Opens restart.htm  - Access Point Filtered
    server.on("/restart.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;

        if (ON_AP_FILTER(request)) {
            logWebTraffic(request, EMPTY_STRING);
            sendResponseToClient(request, 200, RESTART_HTM_FILE);
        }
    });
    // Restarts the system - Access Point Filtered
    server.on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        if (ON_AP_FILTER(request)) {
            if (request->hasParam("restart", true) && request->getParam("restart", true)->value().compareTo("1") == 0) {
                logWebTraffic(request, "The system restarts.");
                printf("The system restarts.\n");
                sendResponseToClient(request, 200, RESTART_HTM_FILE);
                ESP.restart();
            }
        } else {
            logWebTraffic(request, "The system has not been restarted.");
            printf("The system has not been restarted.\n");
            sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
        }
    });
    // Opens admin.htm - Access Point Filtered
    server.on("/admin.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
        logWebTraffic(request, ADMIN_HTM_FILE);
        asyncTcpWdt = true;
        // sendResponseToClient(request, 200, ADMIN_HTM_FILE);
        if (ON_AP_FILTER(request)) {
            sendResponseToClient(request, 200, ADMIN_HTM_FILE);
        } else {
            logWebTraffic(request, "Request for page: " + String(ADMIN_HTM_FILE) + " is not allowed.");
            printf("Request for page: %s is not allowed.\n", ADMIN_HTM_FILE);
            sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
        }
    });
    //  Update new wifi, rule, global settings and admin password - Access Point Filtered
    server.on("/update", HTTP_POST, [this](AsyncWebServerRequest *request) {
        asyncTcpWdt = true;
        updateNewSettingsProccess = true;
        int result = 0;
        logWebTraffic(request, EMPTY_STRING);

        /* List all parameters (Compatibility) */
        int args = request->args();
        printf("...args: %d\n", args);
        for (int i = 0; i < args; i++) {
            printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
        }

        if (ON_AP_FILTER(request)) {
            String ds3231Pwd = cntrl->getDs3231rtc()->getAdminPwd();
            String admPwd = "";
            if (request->hasParam("adm_pwd", true)) {
                admPwd = request->getParam("adm_pwd", true)->value();
            }

            // Compare given admin password with pasword which is stored in 24C32 EEPROM of DS3231RTC
            if (admPwd.compareTo(ds3231Pwd) != 0) {
                logWebTraffic(request, "Invalid admin password: " + request->getParam("adm_pwd", true)->value());
                result = 2;
            } else {
                if ((request->hasParam("page", true)) && (request->getParam("page", true)->value().compareTo(PAGE_WIFI) == 0)) {
                    if (!saveWifiSettings(request)) {
                        logWebTraffic(request, "New WiFi setting has not been saved!");
                    } else {
                        logWebTraffic(request, "New WiFi setting has been saved!");
                        result = 1;
                    }
                } else if ((request->hasParam("page", true)) && (request->getParam("page", true)->value().compareTo(PAGE_ADMIN) == 0)) {
                    if (request->hasParam("new_pwd_1"), true) {
                        printf("************* SAVING PWD\n");
                        // Save admin password in to 24C32 EEPROM of DS3231RTC
                        if (!cntrl->getDs3231rtc()->setAdminPwd(request->getParam("new_pwd_1", true)->value())) {
                            printf("************* PWD NOT SAVED\n");
                            logWebTraffic(request, "New admin password: " + request->getParam("new_pwd_1", true)->value() + " has not been saved.");
                        } else {
                            logWebTraffic(request, "New admin password: " + request->getParam("new_pwd_1", true)->value() + " has been saved.");
                            printf("************* PWD SAVED\n");
                            result = 1;
                        }
                    }
                } else if ((request->hasParam("page", true)) && (request->getParam("page", true)->value().compareTo(PAGE_RULES) == 0)) {
                    if (!saveDelRuleSettings(request)) {
                        logWebTraffic(request, "New Rule setting has not been saved!");
                    } else {
                        logWebTraffic(request, "New Rule setting has been saved!");
                        cntrl->controllerPrepareWatering();
                        result = 1;
                    }
                } else if ((request->hasParam("page", true)) && (request->getParam("page", true)->value().compareTo(PAGE_GLOBAL) == 0)) {
                    if (!saveGlobalSettings(request)) {
                        logWebTraffic(request, "New Global setting has not been saved!");
                    } else {
                        logWebTraffic(request, "New Global setting has been saved!");
                        result = 1;
                    }
                }
            }
        } else {
            result = 2;
        }

        updateNewSettingsProccess = false;

        AsyncJsonResponse *response = new AsyncJsonResponse();
        // response->addHeader("Access-Control-Allow-Origin", "*");
        // response->addHeader("Access-Control-Max-Age", "600");
        // response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
        // response->addHeader("Access-Control-Allow-Headers", "*");
        JsonObject root = response->getRoot();
        root["result"] = result;
        jsonOutput = String();
        serializeJson(root, jsonOutput);
        logWebTraffic(request, jsonOutput);
        response->setLength();
        request->send(response);
    });
    // request->send(404, "text/html", "The content you are looking for was not found.");
    server.onNotFound([this](AsyncWebServerRequest *request) {
        sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
    });

    server.serveStatic("/", SD, "/");

    DefaultHeaders::Instance().addHeader("Access-Control-Allow-Origin", "*");

    server.begin();
}

void WiFi32s::openHtm(String htmFileName_) {
    // If the memory has had allocated we free it
    if (htmlFileMemoryAllocated) {
        free(htmFile);
        htmlFileMemoryAllocated = false;
    }
    // Opens htm file which is stored on SD Card
    File file = SD.open(htmFileName_);
    if (file) {
        size_t fileSize = file.size();
        htmFile = (char *)malloc(fileSize + 1);
        file.readBytes(htmFile, fileSize);
        htmFile[fileSize] = '\0';
        file.close();
        htmlFileMemoryAllocated = true;
    }
}

bool WiFi32s::saveWifiSettings(AsyncWebServerRequest *request_) {
    // Save new Access Point Data
    if (request_->hasParam("ap_save_chb", true) && request_->getParam("ap_save_chb", true)->value().compareTo("on") == 0) {
        if ((request_->hasParam("ap_ssid", true) && request_->getParam("ap_ssid", true)->value().length() >= 4) &&
            (request_->hasParam("ap_newpwd_1", true) && request_->getParam("ap_newpwd_1", true)->value().length() >= 8)) {
            if (!cntrl->getSdCard()->saveValueToIni(WIFI_AP_SECTION, WIFI_SSID_KEY, request_->getParam("ap_ssid", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_AP_SECTION, WIFI_PWD_KEY, request_->getParam("ap_newpwd_1", true)->value())) {
                return false;
            }
        }
    }
    // Save new System Static/Dinamic IP data
    if (request_->hasParam("sta_static_chb", true) && request_->getParam("sta_static_chb", true)->value().compareTo("on") == 0) {
        if (!(request_->hasParam("sta_ip", true)) && !(request_->hasParam("sta_subnet", true)) &&
            !(request_->hasParam("sta_gateway", true)) && !(request_->hasParam("sta_dns", true))) {
            return false;
        }

        if (request_->getParam("sta_ip", true)->value().compareTo("") == 0) {
            if (!cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_IP_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_DNS_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, "0")) {
                return false;
            }
        } else {
            if (!cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_IP_KEY, request_->getParam("sta_ip", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, request_->getParam("sta_subnet", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, request_->getParam("sta_gateway", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_DNS_KEY, request_->getParam("sta_dns", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, "1")) {
                return false;
            }
        }
    }
    // Save new Station data
    if (request_->hasParam("sta_chb", true) && request_->getParam("sta_chb", true)->value().compareTo("on") == 0) {
        if (!(request_->hasParam("sta_ssid", true)) || !(request_->hasParam("sta_newpwd_1", true))) {
            return false;
        }

        if (request_->getParam("sta_ssid", true)->value().compareTo("") == 0) {
            if (!cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_SSID_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_PWD_KEY, "") ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_STASET_KEY, "0")) {
                return false;
            }
        } else if (request_->getParam("sta_newpwd_1", true)->value().length() < 8 ||
                   request_->getParam("sta_ssid", true)->value().length() < 4) {
            return false;
        } else {
            if (!cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_SSID_KEY, request_->getParam("sta_ssid", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_PWD_KEY, request_->getParam("sta_newpwd_1", true)->value()) ||
                !cntrl->getSdCard()->saveValueToIni(WIFI_STA_SECTION, WIFI_STASET_KEY, "1")) {
                return false;
            }
        }
    }

    return true;
}
// Save / Delete rule
bool WiFi32s::saveDelRuleSettings(AsyncWebServerRequest *request_) {
    if ((request_->hasParam("rule_name", true)) && (request_->getParam("rule_name", true)->value() != EMPTY_STRING)) {
        if ((request_->hasParam("save_delete", true)) && (request_->getParam("save_delete", true)->value().toInt()) == 0) {
            // Delete rule
            if (!cntrl->getSdCard()->deleteKey(WATERING_RULES_SECTION, request_->getParam("rule_name", true)->value())) {
                logWebTraffic(request_, "Rule: " + request_->getParam("rule_name", true)->value() + " has not been deleted.");
                return false;
            }
            logWebTraffic(request_, "Rule: " + request_->getParam("rule_name", true)->value() + " has been deleted.");
        } else if ((request_->hasParam("save_delete", true)) && (request_->getParam("save_delete", true)->value().toInt()) == 1) {
            // Save new value
            if (!cntrl->getSdCard()->saveValueToIni(WATERING_RULES_SECTION, request_->getParam("rule_name", true)->value(),
                                                    request_->getParam("new_rule_values", true)->value())) {
                logWebTraffic(request_, "Rule: " + request_->getParam("rule_name", true)->value() + " - value:" +
                                            request_->getParam("new_rule_values", true)->value() + " has not been saved.");
                return false;
            }
            logWebTraffic(request_, "Rule: " + request_->getParam("rule_name", true)->value() + " - value:" +
                                        request_->getParam("new_rule_values", true)->value() + " has been saved.");
        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

bool WiFi32s::saveGlobalSettings(AsyncWebServerRequest *request_) {
    // Save new thresholds
    if ((request_->hasParam("new_thresholds", true)) && (request_->getParam("new_thresholds", true)->value() != EMPTY_STRING)) {
        if (!cntrl->getSdCard()->saveValueToIni(THRESHOLDVALUES_SECTION, THRESHOLD_KEY, request_->getParam("new_thresholds", true)->value())) {
            logWebTraffic(request_, "New thresholds: " + request_->getParam("new_thresholds", true)->value() + " has not been saved.");
            return false;
        }
        logWebTraffic(request_, "New thresholds: " + request_->getParam("new_thresholds", true)->value() + " has been saved.");
    }
    // Save refresh_interval
    if ((request_->hasParam("refresh_interval", true)) && (request_->getParam("refresh_interval", true)->value() != EMPTY_STRING)) {
        if (!cntrl->getSdCard()->saveValueToIni(TIME_INTERVAL_CHECKING_SECTION, INTERVAL_KEY, request_->getParam("refresh_interval", true)->value())) {
            logWebTraffic(request_, "New refresh time: " + request_->getParam("refresh_interval", true)->value() + " has not been saved.");
            return false;
        }
        logWebTraffic(request_, "New  refresh time: " + request_->getParam("refresh_interval", true)->value() + " has been saved.");
    }
    // Save wetness_sensitivity
    if ((request_->hasParam("wetness_sensitivity", true)) && (request_->getParam("wetness_sensitivity", true)->value() != EMPTY_STRING)) {
        if (!cntrl->getSdCard()->saveValueToIni(WETNESS_DRYNESS_SECTION, WETNESS_KEY, request_->getParam("wetness_sensitivity", true)->value())) {
            logWebTraffic(request_, "New wetness sensytivity: " + request_->getParam("wetness_sensitivity", true)->value() + " has not been saved.");
            return false;
        }
        logWebTraffic(request_, "New wetness sensytivity: " + request_->getParam("wetness_sensitivity", true)->value() + " has been saved.");
    }
    // Save dryness_sensitivity
    if ((request_->hasParam("dryness_sensitivity", true)) && (request_->getParam("dryness_sensitivity", true)->value() != EMPTY_STRING)) {
        if (!cntrl->getSdCard()->saveValueToIni(WETNESS_DRYNESS_SECTION, DRYNESS_KEY, request_->getParam("dryness_sensitivity", true)->value())) {
            logWebTraffic(request_, "New dryness sensytivity: " + request_->getParam("dryness_sensitivity", true)->value() + " has not been saved.");
            return false;
        }
        logWebTraffic(request_, "New dryness sensytivity: " + request_->getParam("dryness_sensitivity", true)->value() + " has been saved.");
    }

    return true;
}

bool WiFi32s::startFTPServer() {
    ftp = new FTPServer();
    ftp->addUser(FTP_USER, FTP_PASSWORD);
    ftp->addFilesystem("SD", &SD);

    if (!ftp->begin()) {
        printf("ESP32 FTP server starting error!\n");
        logWebTraffic(nullptr, "ESP32 FTP server starting error!");
        return false;
    }

    printf("ESP32 FTP server has been started successfully.\n");
    logWebTraffic(nullptr, "FTP server has been started successfully.");
    return true;
}

void WiFi32s::logWebTraffic(AsyncWebServerRequest *request, const String message) {
    if (request != nullptr) {
        printf("Opened HOST: %s PATH: %s\n", request->host().c_str(), request->url().c_str());
        printf("Client remote IP: %s, remote port: %d\n", client.remoteIP().toString().c_str(), client.remotePort());
        mainAppError = cntrl->getSdCard()->writeLogFile("Opened HOST: " + request->host() + " PATH: " + request->url());
        mainAppError = cntrl->getSdCard()->writeLogFile("Client remote IP: " + client.remoteIP().toString() + " remote port: " + String(client.remotePort()));
    }

    if (message != EMPTY_STRING) {
        mainAppError = cntrl->getSdCard()->writeLogFile(message);
    }
}

void WiFi32s::sendResponseToClient(AsyncWebServerRequest *request_, int hhtpCode_, String htmFileName_) {
    openHtm(htmFileName_);
    request_->send(hhtpCode_, "text/html", htmFile);
}



// void WiFi32s::setCrossOrigin(AsyncResponseStream *response) {
//     response->addHeader("Access-Control-Allow-Origin", "*");
//     response->addHeader("Access-Control-Max-Age", "600");
//     response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
//     response->addHeader("Access-Control-Allow-Headers", "*");
// }

/*
// Code if we use PLACEHOLDER in html file
void WiFi32s::handleRequest(AsyncWebServerRequest *request) {
    asyncTcpWdt = true;
    request->send_P(200, "text/html", htmFile, [this](const String &var) { return processor(var); });
}
 */
/*
// Code if we use PLACEHOLDER in html file
String WiFi32s::processor(const String &var) {
    if (var == "TEMPERATURE")
        return String(cntrl->temperature, 2);
    else if (var == "HUMIDITY")
        return String(cntrl->relativeHumidity, 2);
    else if (var == "ATM_PRESSURE")
        return String((cntrl->airPressure / 1000), 2);
    else if (var == "VALVES_STATE")
        return cntrl->valvesBinaryString;
    else if (var == "WETNESS_AND_RAINS")
        return cntrl->measuredSensorsValueString;
    else if (var == "WIFI_SETTINGS")
        return WiFi.softAPSSID() + ";" + WiFi.softAPIP().toString() + ";" + WiFi.SSID() + ";" + WiFi.localIP().toString() + ";" +
               WiFi.subnetMask().toString() + ";" + WiFi.gatewayIP().toString() + ";" + WiFi.dnsIP().toString();
    else if (var == "THRESHOLD_VALUES")
        return cntrl->thresholdValues;
    else if (var == "RULES_NAME")
        return cntrl->keysOfWateringRules;
    else if (var == "RULES_VALUE")
        return cntrl->valuesOfKeysOfRules;
    else if (var == "REFRESH_INTERVAL")
        return String(cntrl->refreshSensorsInterval, 10);
    else if (var == "MAX_WETNESS")
        return String(cntrl->maxWetness, 10);
    else if (var == "MAX_DRYNESS")
        return String(cntrl->maxDryness, 10);

    return String();
}
 */

// For testing
// server.on("/json.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
//     asyncTcpWdt = true;
//     logWebTraffic(request);
//     /* List all parameters (Compatibility) */
//    //  int args = request->args();
//     printf("...args: %d\n", args);
//     for (int i = 0; i < args; i++)
//     {
//         printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
//     }
//     openHtm("/json.htm");
//     request->send(202, "text/html", htmFile);

//     AsyncResponseStream *response = request->beginResponseStream("application/json");
//     DynamicJsonDocument json(1024);
//     json["status"] = "ok";
//     json["ssid"] = WiFi.SSID();
//     json["ip"] = WiFi.localIP().toString();
//     serializeJson(json, *response);
//     response->addHeader("Access-Control-Allow-Origin", "*");
//     response->addHeader("Access-Control-Max-Age", "600");
//     response->addHeader("Access-Control-Allow-Methods", "PUT,POST,GET,OPTIONS");
//     response->addHeader("Access-Control-Allow-Headers", "*");
//     request->send(response); });

// CHUNKED
// AsyncWebServerResponse *response = request->beginChunkedResponse("text/plain", [](uint8_t *buffer, size_t maxLen, size_t index) -> size_t {
//     // Write up to "maxLen" bytes into "buffer" and return the amount written.
//     // index equals the amount of bytes that have been already sent
//     // You will be asked for more data until 0 is returned
//     // Keep in mind that you can not delay or yield waiting for more data!
//     return mySource.read(buffer, maxLen);
// });
// response->addHeader("Server", "ESP Async Web Server");
// request->send(response);
// server.on("/wifi.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
//     asyncTcpWdt = true;
//     logWebTraffic(request, EMPTY_STRING);
//     request->sendChunked("text/html", onStaticDownLoad);
// });

/*
    // Code if we use PLACEHOLDER in html file -- IN HTML FILE YOU CANNOT USE '%' character
    // https://stackoverflow.com/questions/59575326/passing-a-function-as-a-parameter-within-a-class
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
                logWebTraffic(request);
                cntrl->controllerGetAht20Bmp280Data();
                openHtm(INDEX_HTM_FILE);
                handleRequest(request); });

    server.on("/wifi.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
            logWebTraffic(request);
            openHtm(WIFI_HTM_FILE);
            handleRequest(request); });

    server.on("/rules.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
            logWebTraffic(request);
            cntrl->controllerGetKeysValuesRules();
            openHtm(RULES_HTM_FILE);
            handleRequest(request); });

    server.on("/admin.htm", HTTP_GET, [this](AsyncWebServerRequest *request) {
            logWebTraffic(request);
            openHtm(ADMIN_HTM_FILE);
            handleRequest(request); });
*/

// part of chunk code
// size_t onStaticDownLoad(uint8_t *buffer, size_t maxLen, size_t index) {
//     asyncTcpWdt = true;
//     static char *filename = WIFI_HTM_FILE;
//     static File chunkfile;
//     static size_t dnlLen = 2048;     // limit to buffer size
//     uint32_t countbytes = 0;  // bytes to be returned
//     if (index == 0) {
//         printf("onStaticDownLoad: START filename: %s index: %6i maxLen: %5i\n", filename, index, maxLen);
//         chunkfile = SD.open(filename);
//     }
//     if(chunkfile) {
//         printf("chunkfile opened\n");
//     }
//     chunkfile.seek(index);
//     countbytes = chunkfile.read(buffer, min(dnlLen, maxLen));

//     if (countbytes == 0) {
//         chunkfile.close();
//         printf("chunkfile closed\n");
//     }
//     printf("onStaticDownLoad: f: %s index: %6u maxLen: %5u countb: %5u \n", filename, index, maxLen, countbytes);

//     return countbytes;
// }