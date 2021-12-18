#include "DS3231RTC.hpp"

/* Main object of RTC */
RTC_DS3231 rtc;

DS3231RTC::DS3231RTC(/* args */)
{
}

DS3231RTC::~DS3231RTC()
{
}

bool DS3231RTC::init()
{
    Wire.begin();
    if (!rtc.begin() || rtc.lostPower())
    {
        printf("...RTC Error...\n");
        return false;
    }

    // Base settings
    rtc.disable32K();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
    rtc.disableAlarm(1);
    rtc.disableAlarm(2);

    printf("...RTC Started....\n");
    return true;
}
