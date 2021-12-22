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
            printf("ESP32 connected to: %s, IP: %s\n", staSSID_, WiFi.localIP().toString().c_str());
            mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 connected to: " + String(staSSID_) + ", IP: " + WiFi.localIP().toString());
            result = true;
        }
    }

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
                  printf("GET Host: %s\n", request->host().c_str());
                  printf("GET Url %s\n", request->url().c_str());
                  openHtm(INDEX_HTM_FILE);
                  /* asyncTcpWdt = true; */
                  handleRequest(request); });

    server.on("/", HTTP_POST, [this](AsyncWebServerRequest *request)
              {
                  /* List all parameters (Compatibility) */
                  printf("POST Host: %s\n", request->host().c_str());
                  printf("POST Url %s\n", request->url().c_str());
                  openHtm(INDEX_HTM_FILE);
                  int args = request->args();
                  printf("...args: %d\n", args);
                  for (int i = 0; i < args; i++)
                  {
                      printf("ARG[%s]: %s\n", request->argName(i).c_str(), request->arg(i).c_str());
                  }
                  /* asyncTcpWdt = true; */
                  if (request->hasParam("adm_pwd", true))
                  {
                      /* printf("Admin password: %s\n", ds3231RTC->getAdminPwd().c_str());
                      printf("Admin adm_pwd: %s\n", request->getParam("adm_pwd", true)->value().c_str()); */
                      if (request->getParam("adm_pwd", true)->value().compareTo(cntrl->getDs3231rtc()->getAdminPwd()) != 0)
                      {
                          openHtm(ERROR_HTM_FILE);
                          request->send(404, "text/html", htmFile);
                      }
                      else
                      {
                          if (request->hasParam("page", true))
                          {
                              if (request->getParam("page", true)->value().compareTo(PAGE_WIFI) != 0 &&
                                  request->getParam("page", true)->value().compareTo(PAGE_WATERING) != 0)
                              {
                                  openHtm(ERROR_HTM_FILE);
                                  request->send(404, "text/html", htmFile);
                              }

                              if (request->getParam("page", true)->value().compareTo(PAGE_WIFI) == 0)
                              {
                                  if (!saveWifiSettings(request))
                                  {
                                      openHtm(ERROR_HTM_FILE);
                                      request->send(404, "text/html", htmFile);
                                  }
                                  else
                                  {
                                      openHtm(CORRECT_HTM_FILE);
                                      request->send(202, "text/html", htmFile);
                                  }
                              }
                          }
                      }
                  }
                  else
                  {
                      handleRequest(request);
                  } });

    server.serveStatic("/", SD, "/");

    server.begin();
}

String WiFi32s::processor(const String &var)
{
    String result;
    if (var == "WEATHER")
    {
        result += "<tr>\n";
        result += "<td style=\"width: 240px;\">Temperature</td>\n";
        result += "<td style=\"width: 100px;\">" + String(cntrl->temperature, 2) + "</td>\n";
        result += "<td style=\"width: 100px;\"><img src=\"./icns/temp.png\" alt=\"temp\"></td>\n";
        result += "</tr>\n";

        result += "<tr>\n";
        result += "<td style=\"width: 240px;\">Humidity</td>\n";
        result += "<td style=\"width: 100px;\">" + String(cntrl->relativeHumidity, 2) + "</td>\n";
        result += "<td style=\"width: 100px;\"><img src=\"./icns/humidity.png\" alt=\"temp\"></td>\n";
        result += "</tr>\n";

        result += "<tr>\n";
        result += "<td style=\"width: 240px;\">Atm. Pressure</td>\n";
        result += "<td style=\"width: 100px;\">" + String((cntrl->airPressure / 1000), 2) + "</td>\n";
        result += "<td style=\"width: 100px;\"><img src=\"./icns/atmospheric.png\" alt=\"temp\"></td>\n";
        result += "</tr>\n";

        // printf("\n%s\n", result.c_str());
        return result;
    }
    else if (var == "VALVES")
        return cntrl->valvesBinaryString;
    else if (var == "SENSORS_VALUE")
        return cntrl->measuredSensorsValueString;
    else if (var == "THRESHOLD_VALUES")
        return cntrl->thresholdSensorsValueString;
    /* if (var == "TEMP_PH")
        return String(cntrl->temperature, 2);

    if (var == "HUM_PH")
        return String(cntrl->relativeHumidity, 2);

    if (var == "AIR_PH")
        return String((cntrl->airPressure / 1000), 2);

    if (var == "APSSID_PH")
        return WiFi.softAPSSID();

    if (var == "APIP_PH")
        return WiFi.softAPIP().toString();

    if (var == "STASSID_PH")
        return WiFi.SSID();

    if (var == "STAIP_PH")
        return WiFi.localIP().toString();

    if (var == "STASUBNET_PH")
        return WiFi.subnetMask().toString();

    if (var == "STAGATEWAY_PH")
        return WiFi.gatewayIP().toString();

    if (var == "STADNS_PH")
        return WiFi.dnsIP().toString();

    if (var == "VALVES")
        return cntrl->valvesBinaryString;

    if (var == "SENSORS_VALUE")
        return cntrl->measuredSensorsValueString;

    if (var == "THRESHOLD_VALUES")
        return cntrl->thresholdSensorsValueString; */

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
    File file = SD.open(htmFileName_);
    if (file)
    {
        size_t fileSize = file.size();
        htmFile = (char *)malloc(fileSize + 1);
        file.readBytes(htmFile, fileSize);
        htmFile[fileSize] = '\0';
        file.close();
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

        if (!(request_->hasParam("ap_newpwd_1", true)) && !(request_->hasParam("ap_ssid", true)))
        {
            return false;
        }

        if (request_->getParam("ap_newpwd_1", true)->value().compareTo("") == 0 ||
            request_->getParam("ap_ssid", true)->value().compareTo("") == 0)
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

        if (!(request_->hasParam("sta_newpwd_1", true)) && !(request_->hasParam("sta_ssid", true)))
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

bool WiFi32s::startFTPServer()
{
    ftp = new FTPServer();
    ftp->addUser(FTP_USER, FTP_PASSWORD);
    ftp->addFilesystem("SD", &SD);

    if (!SD.exists(WS_INI_FILE))
    {
        printf("SDCard ERROR ini file %s is not exists!\n", WS_INI_FILE);
    }

    printf("SDCard ini file %s is exists!\n", WS_INI_FILE);

    if (!ftp->begin())
    {
        printf("ESP32 FTP server starting error!\n");
        mainAppError = cntrl->getSdCard()->writeLogFile("ESP32 FTP server starting error!");
        return false;
    }
    printf("FTP server has been started successfully\n");
    mainAppError = cntrl->getSdCard()->writeLogFile("FTP server has been started successfully");
    return true;
}

void WiFi32s::getClientData()
{
    WiFiClient thisClient;
    printf("Client timeout: %d\n", thisClient.getTimeout());
    printf("Client available: %d\n", thisClient.available());
    printf("Client connected: %d\n", thisClient.connected());
}
