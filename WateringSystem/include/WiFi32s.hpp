/* WiFi32s header */
#ifndef __WIFI32S_HPP__
#define __WIFI32S_HPP__

#include "WiFi.h"
#include "AsyncTCP.h"
#include "ESPAsyncWebServer.h"
#include "ESP-FTP-Server-Lib.h"
#include "FTPFilesystem.h"
#include "Controller.hpp"

class Controller;

class WiFi32s
{
private:
    /* data */
    String apIPString;
    String staIPString;
    IPAddress *staIP;         /* Station Static IP address */
    IPAddress *staSubnet;     /* Station Subnet */
    IPAddress *staGateway;    /* Station Gateway */
    IPAddress *staPrimaryDNS; /* Station Primary DNS */
    /* Private method - Convert string to IPAddress */
    bool stringToIPAdress(const char *data_, IPAddress *address_);
    /* Private method - fills out place holders of web page */
    String processor(const String &var);
    /* Private method - web page send request */
    void handleRequest(AsyncWebServerRequest *request);
    /* Private Controller object */
    Controller *cntrl;
    int staEnabled;
    int staStaticIP;
    /* Private method - convert htm file from SD card to web html file which will be send to client */
    void openHtm(String htmFileName_);

public:
    WiFi32s(/* args */ Controller *cntrl_);
    ~WiFi32s();
    /* Public method - init WiFi32s - if initialization successful the return is TRUE else FALSE */
    bool init(int apHidden_, const char *apSSID_, const char *apPWD_, int apChannel_, int apMaxConnection_, int staSet_,
              const char *staSSID_, const char *staPWD_, int staStaticIp_, const char *staIP_, const char *staSubnet_,
              const char *staGateway_, const char *staDNS_);
    /* AsyncWebServer object */
    AsyncWebServer server = AsyncWebServer(80);
    /* Public method - create htm file and start web server */
    void startWebHtm();
    /* Public method - Start FTP server. Browse SDCard */
    bool startFTPServer();
    /* Public method - save new wifi settings */
    bool saveWifiSettings(AsyncWebServerRequest *request_);
    /* Public property - char array of html file */
    char *htmFile;
    /* Fublic - FTP Object */
    FTPServer *ftp;

    void getClientData();
};

#endif /* __WIFI32S_HPP__ */