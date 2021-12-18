/* Controller header */
#ifndef __CONTROLLER_HPP__
#define __CONTROLLER_HPP__

#include "DefsVarsFuncs.hpp"
#include "DS3231RTC.hpp"
#include "DigitalOutput.hpp"

class Controller
{
private:
    /* data */
    /* DigitalOutput objects */
    DigitalOutput *redLED;
    DigitalOutput *greenLED;
    DigitalOutput *powerSensorsCH1;
    DigitalOutput *powerSensorsCH2;
    DigitalOutput *sh_cp, *st_cp, *ds;
    /* DS3231 object */
    DS3231RTC *ds3231rtc;
public:
    /* Constructor */
    Controller(/* args */);
    /* Destructor */
    ~Controller();
    /* Public method - Configure DS3231RTC */
    bool controllerDS3231RTCInit();
    /* Public method - Configure Digital Outputs */
    bool controllerDigitalOutputInit();
    /* Getter Green Led Digital Output */
    DigitalOutput *getGreenLED() const;
    /* Getter Red Led Digital Output */
    DigitalOutput *getRedLED() const;
};


#endif /* __CONTROLLER_HPP__ */