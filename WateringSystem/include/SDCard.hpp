/*  !!! All information are in DataSheets folder !!!
*
*   Connection:
*   MicroSD_M pin  NodeMCU-32S pin
*       GND --------------------------- Power GND
*       Vcc --------------------------- Power 5 V
*       MOSI ------ MOSI-GPIO23
*       MISO ------ MISO-GPIO19
*       SCK  ------ VSPI SCK-GPIO18
*       CS   ------ VSPI SS-GPIO5    
*/
/* SDCard header */
#ifndef __SDCARD_HPP__
#define __SDCARD_HPP__

#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "DefsVarsFuncs.hpp"
#include "DS3231RTC.hpp"
#include "minIni.h"
#include <new>

class SDCard
{
private:
    /* data */
    /* Main wsIni object */
    minIni *wsIni;
    DS3231RTC *ds3231rtc;
    /* Private method - Opening the ws.ini file and check is file opened successfully */
    //bool openingWsIniFile();
    
public:
    /* Default Constructor */
    SDCard();
    /* Default Destructor */
    ~SDCard();                  
    /* Public method - init SDCard - if initialization successful the return value is TRUE else FALSE */
    bool init();
    /* Public method - write log message into log file - If error occurs the return value is TRUE else FALSE */
    bool writeLogFile(String logMessage_);
    /* Public method - remove log files older than four days */
    void removeOldLogFiles();
    /* Public method - Save analog sensors threshold values from ini file to array */
    // bool saveThresholdValuesToArray(int *array_);
    /* Public method - Reads section and key from ini file and stores result in value parameter */
    void getValueFromIni(const String &section_, const String &key_, String &value);
    /* Public method - Saves the text parameter that in the given section and at the given key. 1/true on success, 0/false on failure*/
    bool saveValueToIni(String section_, String key_, String value_);
    /* Public method - Gets number of keys in a section */
    uint8_t getNumKeysInSection(const String &section);
    /* Public method - Stores key values into String array */
    bool getKeysArray(String section_, char **arr);
    /* Public method - Delete key and value in the given section */
    bool deleteKey(const String &section_, const String &key_);

};


#endif /* __SDCARD_HPP__ */