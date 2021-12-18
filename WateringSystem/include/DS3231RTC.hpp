/*  !!! All information are in DataSheets folder !!!
 *
 *   Connection:
 *   DS3231 pin     NodeMCU-32S pin
 *       GND --------------------------- Power GND
 *       Vcc --------------------------- Power 3.3 V
 *       SDA ------- SDA-GPIO21
 *       SCL ------- SCL-GPIO22
 */
/* DS3231RTC header */
#ifndef __DS3231RTC_HPP__
#define __DS3231RTC_HPP__

/* Include RTClib by Adafruit */
#include <RTClib.h>
#include <SPI.h>
#include <Wire.h>


class DS3231RTC
{
private:
    /* data */

public:
    /* Constructor */
    DS3231RTC();
    /* Destructor */
    ~DS3231RTC();
    /* Public method - init DS3231RTC - if initialization successful the return is TRUE else FALSE */
    bool init();
};

#endif /* __DS3231RTC_HPP__ */