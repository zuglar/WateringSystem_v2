#include "SDCard.hpp"

SDCard::SDCard() {}

SDCard::~SDCard() {}

bool SDCard::init()
{
    if (!SD.begin(SD_CS_PIN, SPI, SDCARD_FREQUENY, SDCARD_MOINTPOINT, SDCARD_MAX_FILES, false))
    {
        printf("SD begin error!\n");
        return false;
    }
    
    if (!SD.exists(LOG_DIR_NAME))
    {
        if (!ds3231rtc->isRtcRunning())
        {
            printf("SDCard RTC NOT RUNNING!\n");
            return false;
        }

        if (!SD.mkdir(LOG_DIR_NAME))
        {
            printf("SDCard ERROR Create directory %s !\n", LOG_DIR_NAME);
            return false;
        }
    }

    if (!SD.exists(WS_INI_FILE))
    {
        printf("SDCard ERROR ini file %s is not exists!\n", WS_INI_FILE);
        return false;
    }

    printf("SDCard OK.\n");
    if (!writeLogFile("SDCard init OK."))
        return false;
    removeOldLogFiles();
    return true;
}

bool SDCard::writeLogFile(String logMessage_)
{
    if (!SD.exists(LOG_DIR_NAME))
    {
        printf("SDCard writeLogFile ERROR. Dir: %s is not exists!\n", LOG_DIR_NAME);
        return false;
    }
        
    String logFile = String(LOG_DIR_NAME) + "/" + ds3231rtc->getDateNow() + ".log";
    File file = SD.open(logFile, FILE_APPEND);

    if (!file)
    {
        printf("Failed to open file %s for appending!\n", logFile.c_str());
        return false;
    }
    String message = ds3231rtc->getDateTimeNow() + " - " + logMessage_;
    if (!file.println(message))
    {
        printf("Data appended in to file %s failed!\n", logFile.c_str());
        file.close();
        return false;
    }
    file.close();

    return true;
}

void SDCard::removeOldLogFiles()
{
    String oldLogFileName = String(LOG_DIR_NAME) + "/" + ds3231rtc->oldLogFileDate() + ".log";
    File logDir = SD.open(LOG_DIR_NAME);
    if (!logDir)
    {
        printf("Failed to open directory %s!\n", LOG_DIR_NAME);
        mainInitError = false;
        return;
    }

    logDir.rewindDirectory();

    File file = logDir.openNextFile();
    while (file)
    {
        if (strcmp(file.name(), oldLogFileName.c_str()) < 0)
        {
            if (!SD.remove(file.name()))
            {
                mainInitError = writeLogFile("Delete " + String(file.name()) + " failed!");
            }
            else
            {
                mainInitError = writeLogFile("Delete file: " + String(file.name()));
            }
        }
        file = logDir.openNextFile();
    }
    logDir.close();
}

bool SDCard::openingWsIniFile() {
    wsIni = new minIni(String(WS_INI_FILE));
    if (!wsIni->getbool(READY_SECTION, READY_KEY))
    {
        mainInitError = writeLogFile("Failed open " + String(WS_INI_FILE) + " file.");
        printf("...Failed open %s file...\n", WS_INI_FILE);
        delete wsIni;
        return false;
    }
    return true;
}

bool SDCard::saveThresholdValuesToArray(int *array_)
{
    if(!openingWsIniFile())
        return false;

    String key;
    
    for (int i = 0; key = wsIni->getkey(ANALOGSENSORS_SECTION, i), key.length() > 0; i++)
    {
        array_[i] = wsIni->geti(ANALOGSENSORS_SECTION, key, 0);
    }
    delete wsIni;

    /* writeLogFile("Analog Sensors threshold values from " + WS_INI_FILE + " saved to array.");
    printf("...Analog Sensors threshold values from %s saved to array...\n", WS_INI_FILE.c_str()); */

    return true;
}

String SDCard::getValueFromIni(String section_, String key_) 
{
    if(!openingWsIniFile())
        return EMPTY_STRING;
    
    String result;
    result = wsIni->gets(section_, key_);
    delete wsIni;

    return result;
}
