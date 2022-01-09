#include "WiFi32s.hpp"

WiFi32s::WiFi32s(/* args */ Controller *cntrl_)
{
    cntrl = cntrl_;
}

WiFi32s::~WiFi32s() {}

bool WiFi32s::init(int apHidden_, const char *apSSID_, const char *apPWD_, int apChannel_, int apMaxConnection_, int staSet_,
                   const char *staSSID_, const char *staPWD_, int staStaticIp_, const char *staIP_, const char *staSubnet_,
                   const char *staGateway_, const char *staDNS_)
{
    // sdCard = sdCard_;
    bool result = false;

    staEnabled = staSet_;
    staStaticIP = staStaticIp_;

    /* WiFi mode to Access Point and to Station */
    WiFi.mode(WIFI_AP_STA);
    /* Begin Access Point */
    if (!WiFi.softAP(apSSID_, apPWD_, apChannel_, apHidden_, apMaxConnection_))
    {
        printf("Failed to configure Access Point!\n");
        // sdCard->writeLogFile("Failed to configure Access Point");
        mainAppError = cntrl->getSdCard()->writeLogFile("Failed to configure Access Point.");
    }
    else
    {
        apIPString = WiFi.softAPIP().toString();
        printf("ESP32 AP created. IP: %s\n", apIPString.c_str());
        mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 AP created. SSID: " + String(apSSID_) + ", IP: " + WiFi.softAPIP().toString());
        result = true;
    }

    /* Begin Station WiFi */
    if (staEnabled == 1)
    {
        bool staticNetworkData = false;
        /* Configures static IP address */
        if (staStaticIP == 1)
        {
            staIP = new IPAddress();
            staSubnet = new IPAddress();
            staGateway = new IPAddress();
            staPrimaryDNS = new IPAddress();

            /* Set local Static IP address */
            if (stringToIPAdress(staIP_, staIP))
            {
                /* Set local subnet */
                if (stringToIPAdress(staSubnet_, staSubnet))
                {
                    /* Set local gateway */
                    if (stringToIPAdress(staGateway_, staGateway))
                    {
                        /* Set primary DNS */
                        if (strlen(staDNS_) != 0)
                        {
                            staticNetworkData = stringToIPAdress(staDNS_, staPrimaryDNS);
                        }
                        else
                        {
                            staticNetworkData = stringToIPAdress(WIFI_NO_DNS_DATA, staPrimaryDNS);
                        }
                    }
                }
            }

            if (staticNetworkData)
            {
                // Configures static IP address
                if (!WiFi.config(*staIP, *staGateway, *staSubnet, *staPrimaryDNS))
                {
                    printf("STA failed to configure static network data!\n");
                    mainAppError = cntrl->getSdCard()->writeLogFile("STA failed to configure static network data");
                    delete staIP;
                    delete staGateway;
                    delete staSubnet;
                    delete staPrimaryDNS;
                }
            }
        }

        WiFi.begin(staSSID_, staPWD_);
        /* Wait for connection */
        uint8_t i = 0;
        while (WiFi.status() != WL_CONNECTED && i++ < 20)
        {
            delay(500);
            printf(".");
        }
        printf("\n");

        if (i == 21)
        {
            printf("ESP32 could not connect to: %s !\n", staSSID_);
            mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 could not connect to: " + String(staSSID_));
        }
        else
        {
            staIPString = WiFi.localIP().toString();
            printf("ESP32 connected to: %s, IP: %s\n", staSSID_, staIPString.c_str());
            mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 connected to: " + String(staSSID_) + ", IP: " + staIPString);
            result = true;
        }
    }

    htmlFileMemoryAllocated = false;

    return result;
}

bool WiFi32s::stringToIPAdress(const char *data_, IPAddress *address_)
{
    if (!address_->fromString(data_))
        return false;

    return true;
}

void WiFi32s::startWebHtm()
{
    // https://stackoverflow.com/questions/59575326/passing-a-function-as-a-parameter-within-a-class
    server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                cntrl->controllerGetAht20Bmp280Data();
                openHtm(INDEX_HTM_FILE);
                handleRequest(request); });

    server.on("/wifi.htm", HTTP_GET, [this](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                openHtm(WIFI_HTM_FILE);
                handleRequest(request); });

    server.on("/admin.htm", HTTP_GET, [this](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                openHtm(ADMIN_HTM_FILE);
                handleRequest(request); });

    server.on("/rules.htm", HTTP_GET, [this](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                cntrl->controllerGetKeysValuesRules();
                openHtm(RULES_HTM_FILE);
                handleRequest(request); });

    // Send a GET request to <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2>
    server.on("/update", HTTP_GET, [&](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                 /* GET values of weather, state of valves, state of wetness of soil, state of rain sensors on <ESP_IP>/update?update=1 */
                if (request->hasParam("update"))
                {
                    if (request->getParam("update")->value().compareTo("1") == 0)
                    {
                        cntrl->controllerGetAht20Bmp280Data();
                    }
                    else
                    {
                        sendResponseToClient(request, 202, NOTFOUND_HTM_FILE);
                    }
                }
                else
                {
                    sendResponseToClient(request, 202, NOTFOUND_HTM_FILE);
                }
                openHtm(INDEX_HTM_FILE);
                handleRequest(request); });

    server.on("/restart", HTTP_POST, [this](AsyncWebServerRequest *request)
              {
                logWebTraffic(request);
                /* POST value to restart ESP on <ESP_IP>/update?output=<inputMessage1>&state=<inputMessage2> */
                if (request->hasParam("restart", true))
                {
                    if (request->getParam("restart", true)->value().compareTo("1") == 0)
                    {
                        mainAppError = cntrl->getSdCard()->writeLogFile("The system restarts.");
                        ESP.restart();
                    }
                    else
                    {
                        sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
                    }
                }
                else
                {
                    mainAppError = cntrl->getSdCard()->writeLogFile("The system restart failed.");
                    sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
                } });

    server.on("/", HTTP_POST, [this](AsyncWebServerRequest *request)
              {
                  logWebTraffic(request);
                  /* List all parameters (Compatibility) */
                  int args = request->args();
                  /* printf("...args: %d\n", args);
                  for (int i = 0; i < args; i++)
                  {
                      printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
                  } */

                  if(ON_STA_FILTER(request))
                  {
                      String redirectUrl = "http://" + staIPString + NOTFOUND_HTM_FILE;
                      printf("Invalid request from client. Redirect to: %s .\n", redirectUrl.c_str());
                      mainAppError = cntrl->getSdCard()->writeLogFile("Invalid request from client. Redirect to:  " + redirectUrl);
                      request->redirect(redirectUrl);
                  }

                  if (request->hasParam("adm_pwd", true))
                  {
                      /* Compare given admin password with pasword which is stored in 24C32 EEPROM of DS3231RTC */
                      if (request->getParam("adm_pwd", true)->value().compareTo(cntrl->getDs3231rtc()->getAdminPwd()) != 0)
                      {
                          mainAppError = cntrl->getSdCard()->writeLogFile("Invalid admin password: " + request->getParam("adm_pwd", true)->value());
                          sendResponseToClient(request, 202, ERROR_HTM_FILE);
                      }
                      else
                      {
                          if (request->hasParam("page", true))
                          {
                              if (request->getParam("page", true)->value().compareTo(PAGE_WIFI) == 0)
                              {
                                  if (!saveWifiSettings(request))
                                  {
                                      sendResponseToClient(request, 202, ERROR_HTM_FILE);
                                  }
                                  else
                                  {
                                      sendResponseToClient(request, 202, CORRECT_HTM_FILE);
                                  }
                              }
                              else if (request->getParam("page", true)->value().compareTo(PAGE_ADMIN) == 0)
                              {
                                  if(request->hasParam("new_pwd_1"), true)
                                  {
                                      /* Save admin password in to 24C32 EEPROM of DS3231RTC */
                                      if (!cntrl->getDs3231rtc()->setAdminPwd(request->getParam("new_pwd_1", true)->value()))
                                      {
                                        mainAppError = cntrl->getSdCard()->writeLogFile("New admin password: " + 
                                                request->getParam("new_pwd_1", true)->value() + " has not been saved.");
                                        sendResponseToClient(request, 202, ERROR_HTM_FILE);
                                      }
                                      else
                                      {
                                        mainAppError = cntrl->getSdCard()->writeLogFile("New admin password: " + 
                                                request->getParam("new_pwd_1", true)->value() + " has been saved.");
                                        sendResponseToClient(request, 202, CORRECT_HTM_FILE);
                                      }   
                                  }
                                  else
                                  {
                                      sendResponseToClient(request, 202, ERROR_HTM_FILE);
                                  }
                              }
                              else
                              {
                                  sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
                              }
                          }
                          else
                          {
                              sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
                          }
                      }
                    }
                    else
                    {
                        sendResponseToClient(request, 404, NOTFOUND_HTM_FILE);
                    } });

    server.onNotFound([this](AsyncWebServerRequest *request)
                      { 
                          /* request->send(404, "text/html", "The content you are looking for was not found."); */ 
                          sendResponseToClient(request, 404, NOTFOUND_HTM_FILE); });

    server.serveStatic("/", SD, "/");

    server.begin();
}

String WiFi32s::processor(const String &var)
{
    if(var == "TEMPERATURE")
        return String(cntrl->temperature, 2);
    else if(var == "HUMIDITY")
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
    else if(var == "REFRESH_INTERVAL")
        return String(cntrl->refreshSensorsInterval, 10);
    else if(var == "MAX_WETNESS")
        return String(cntrl->maxWetness, 10);
    else if(var == "MAX_DRYNESS")
        return String(cntrl->maxDryness, 10);

    return String();
}

void WiFi32s::handleRequest(AsyncWebServerRequest *request)
{
    asyncTcpWdt = true;
    request->send_P(200, "text/html", htmFile, [this](const String &var)
                    { return processor(var); });
}

void WiFi32s::openHtm(String htmFileName_)
{
    /* If the memory has had allocated we free it */
    if (htmlFileMemoryAllocated)
    {
        free(htmFile);
        htmlFileMemoryAllocated = false;
    }

    File file = SD.open(htmFileName_);
    if (file)
    {
        size_t fileSize = file.size();
        htmFile = (char *)malloc(fileSize + 1);
        file.readBytes(htmFile, fileSize);
        htmFile[fileSize] = '\0';
        file.close();
        htmlFileMemoryAllocated = true;
    }
}

bool WiFi32s::saveWifiSettings(AsyncWebServerRequest *request_)
{
    /* Save new Access Point Data */
    if (request_->hasParam("ap_save_chb", true))
    {
        if (request_->getParam("ap_save_chb", true)->value().compareTo("on") != 0)
        {
            return false;
        }

        if (!(request_->hasParam("ap_newpwd_1", true)) || !(request_->hasParam("ap_ssid", true)))
        {
            return false;
        }

        if (request_->getParam("ap_newpwd_1", true)->value().length() < 8 ||
            request_->getParam("ap_ssid", true)->value().length() < 4)
        {
            return false;
        }

        if (!cntrl->getSdCard()->storeValueToIni(WIFI_AP_SECTION, WIFI_SSID_KEY, request_->getParam("ap_ssid", true)->value()) ||
            !cntrl->getSdCard()->storeValueToIni(WIFI_AP_SECTION, WIFI_PWD_KEY, request_->getParam("ap_newpwd_1", true)->value()))
        {
            return false;
        }
    }
    /* Save new Station data */
    if (request_->hasParam("sta_chb", true))
    {
        if (request_->getParam("sta_chb", true)->value().compareTo("on") != 0)
        {
            return false;
        }

        if (!(request_->hasParam("sta_newpwd_1", true)) || !(request_->hasParam("sta_ssid", true)))
        {
            return false;
        }

        if (request_->getParam("sta_newpwd_1", true)->value().compareTo("") == 0 ||
            request_->getParam("sta_ssid", true)->value().compareTo("") == 0)
        {
            if (!cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_SSID_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_PWD_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_STASET_KEY, "0"))
            {
                return false;
            }
        }
        else if (request_->getParam("sta_newpwd_1", true)->value().length() < 8 ||
                 request_->getParam("sta_ssid", true)->value().length() < 1)
        {
            return false;
        }
        else
        {
            if (!cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_SSID_KEY, request_->getParam("sta_ssid", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_PWD_KEY, request_->getParam("sta_newpwd_1", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_STASET_KEY, "1"))
            {
                return false;
            }
        }
    }
    /* Save new Station Static IP data */
    if (request_->hasParam("sta_static_chb", true))
    {
        if (request_->getParam("sta_static_chb", true)->value().compareTo("on") != 0)
        {
            return false;
        }

        if (!(request_->hasParam("sta_ip", true)) && !(request_->hasParam("sta_subnet", true)) &&
            !(request_->hasParam("sta_gateway", true)) && !(request_->hasParam("sta_dns", true)))
        {
            return false;
        }

        if (request_->getParam("sta_ip", true)->value().compareTo("") == 0 ||
            request_->getParam("sta_subnet", true)->value().compareTo("") == 0 ||
            request_->getParam("sta_gateway", true)->value().compareTo("") == 0 ||
            request_->getParam("sta_dns", true)->value().compareTo("") == 0)
        {
            if (!cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_IP_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_DNS_KEY, "") ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, "0"))
            {
                return false;
            }
        }
        else
        {
            if (!cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_IP_KEY, request_->getParam("sta_ip", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_SUBNET_KEY, request_->getParam("sta_subnet", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_GATEWAY_KEY, request_->getParam("sta_gateway", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_DNS_KEY, request_->getParam("sta_dns", true)->value()) ||
                !cntrl->getSdCard()->storeValueToIni(WIFI_STA_SECTION, WIFI_STATIC_IP_KEY, "1"))
            {
                return false;
            }
        }
    }

    return true;
}

void WiFi32s::startFTPServer()
{
    ftp = new FTPServer();
    ftp->addUser(FTP_USER, FTP_PASSWORD);
    ftp->addFilesystem("SD", &SD);

    if (!ftp->begin())
    {
        printf("ESP32 FTP server starting error!\n");
        mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 FTP server starting error!");
    }
    printf("FTP server has been started successfully\n");
    mainAppError = cntrl->getSdCard()->writeLogFile("FTP server has been started successfully");
}

void WiFi32s::logWebTraffic(AsyncWebServerRequest *request)
{
    client = new WiFiClient();
    printf("Opened HOST: %s URL: %s\n", request->host().c_str(), request->url().c_str());
    printf("Client remote IP: %s, remote port: %d\n", client->remoteIP().toString().c_str(), client->remotePort());
    mainAppError = cntrl->getSdCard()->writeLogFile("Opened HOST: " + request->host() + " URL: " + request->url());
    mainAppError = cntrl->getSdCard()->writeLogFile("Client remote IP: " + client->remoteIP().toString() + " remote port: " + String(client->remotePort()));
    delete client;
}

void WiFi32s::sendResponseToClient(AsyncWebServerRequest *request_, int hhtpCode_, String htmFileName_)
{
    openHtm(htmFileName_);
    request_->send(hhtpCode_, "text/html", htmFile);
}
