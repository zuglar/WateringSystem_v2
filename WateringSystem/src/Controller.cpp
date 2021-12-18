#include "Controller.hpp"

Controller::Controller(/* args */) {}

Controller::~Controller() {}

bool Controller::controllerDS3231RTCInit()
{
    /* Create dS3231RTC object and init it */
    ds3231rtc = new DS3231RTC();
    if (!ds3231rtc->init())
        return false;
    return true;
}

bool Controller::controllerDigitalOutputInit()
{
    /* Create Red LED object and configure it */
    redLED = new DigitalOutput(RED_LED);
    if (!redLED->init())
    {
        return false;
    }
    /* Create Green LED object and configure it */
    greenLED = new DigitalOutput(GREEN_LED);
    if (!greenLED->init())
    {
        return false;
    }
    /* Create power sensors channel 1 object and configure it */
    powerSensorsCH1 = new DigitalOutput(POWER_SENORS_1_5_CH1);
    if (!powerSensorsCH1->init())
    {
        return false;
    }
    /* Create power sensors channel 2 object and configure it */
    powerSensorsCH2 = new DigitalOutput(POWER_SENORS_6_10_CH2);
    if (!powerSensorsCH2->init())
    {
        return false;
    }
    /* Create SHCP object and configure it */
    sh_cp = new DigitalOutput(SHCP);
    if (!sh_cp->init())
    {
        return false;
    }
    /* Create STCP object and configure it */
    st_cp = new DigitalOutput(STCP);
    if (!st_cp->init())
    {
        return false;
    }
    /* Create DS object and configure it */
    ds = new DigitalOutput(DS);
    if (!ds->init())
    {
        return false;
    }
    return true;
}

DigitalOutput *Controller::getGreenLED() const
{
    return greenLED;
}

DigitalOutput *Controller::getRedLED() const
{
    return redLED;
}
