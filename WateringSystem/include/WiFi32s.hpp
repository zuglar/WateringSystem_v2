/* WiFi32s header */
#ifndef __WIFI32S_HPP__
#define __WIFI32S_HPP__

#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
#include <ArduinoJson.h>
#include "Controller.hpp"
#include <AsyncJson.h>
#include <EasyDDNS.h>
#include "HTTPClient.h"

class Controller;

class WiFi32s
{
private:
    /* data */
    IPAddress *staIP;         /* Station Static IP address */
    IPAddress *staSubnet;     /* Station Subnet */
    IPAddress *staGateway;    /* Station Gateway */
    IPAddress *staPrimaryDNS; /* Station Primary DNS */
    
    /* Private method - Convert string to IPAddress */
    bool stringToIPAdress(const char *data_, IPAddress *address_);
/* 
    // Code if we use PLACEHOLDER in html file
    // Private method - fills out place holders of web page
    // String processor(const String &var);
    // Private method - web page send request
    //void handleRequest(AsyncWebServerRequest *request);
 */
    /* Private method - convert htm file from SD card to web html file which will be send to client */
    void openHtm(String htmFileName_);

public:
    WiFi32s(/* args */ Controller *cntrl_);
    ~WiFi32s();
    /* Public method - init WiFi32s - if initialization successful the return is TRUE else FALSE */
    bool init(int apHidden_, const char *apSSID_, const char *apPWD_, int apChannel_, int apMaxConnection_, int staSet_,
              const char *staSSID_, const char *staPWD_, int staStaticIp_, const char *staIP_, const char *staSubnet_,
              const char *staGateway_, const char *staDNS_, int ddns_, const char *ddnsProvider_, const char *ddnsHost_,
              const char *ddnsUserName_, const char *ddnsPassword_);
    /* AsyncWebServer object */
    AsyncWebServer server = AsyncWebServer(80);
    /* Public method - create htm file and start web server */
    void startWebHtm();
    /* Public method - Start FTP server. Browse SDCard */
    bool startFTPServer();
    /* Public method - save wifi settings */
    bool saveWifiSettings(AsyncWebServerRequest *request_);
    /* Public method - save / delete rule settings */
    bool saveDelRuleSettings(AsyncWebServerRequest *request_);
    /* Public method - save global settings */
    bool saveGlobalSettings(AsyncWebServerRequest *request_);
    /* Public property - char array of html file */
    char *htmFile;
    /* Public - FTP Object */
    FTPServer *ftp;
    /* Public Controller object */
    Controller *cntrl;
    /* Public property - Checks memory allocation for htmFile */
    bool htmlFileMemoryAllocated;
    /* Public method - Saves activities in log file on SD Card */
    void logWebTraffic(AsyncWebServerRequest *request, const String message);
    /* Public property - get client data for logging and checking for saving server data */
    WiFiClient client;
    /* Public property - send response to client */
    void sendResponseToClient(AsyncWebServerRequest *request_, int hhtpCode_, String htmFileName_);


    void notFound(AsyncWebServerRequest *request);

    String jsonOutput;

    String apIPString;
    String staIPString;
    int staEnabled;
    int staStaticIP;
    String ddnsProvider;
    String ddnsHost;
    
/* 
    // https://www.mischianti.org/2020/07/15/how-to-create-a-rest-server-on-esp8266-or-esp32-cors-request-option-and-get-part-4/
    // void setCrossOrigin(AsyncResponseStream *response);
 */
};

#endif /* __WIFI32S_HPP__ */