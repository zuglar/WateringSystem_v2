/*  !!! All information are in DataSheets folder !!!
*
* GPIO26 - turn off/on RED LED
* GPIO27 - turn off/on GREEN LED
* GPIO25 - turn off/on power of rain and wetness sensors 1-5
* GPIO33 - turn off/on wetness sensors 6-10
* 
* NodeMCU-32S pin          74HC595 pin
*   GPIO4  --------------- RCLK/STCP (ST_CP) 12
*   GPIO16 --------------- SRCLK/SHCP (SH_CP) 11
*   GPIO17 --------------- SER/DS (DS) 14
*
*   https://forum.arduino.cc/t/pinmode-in-class-constructor-seem-not-to-work/370583
*/
/* DigitalOutput header */
#ifndef __DIGITALOUTPUT_H__
#define __DIGITALOUTPUT_H__

#include "driver/gpio.h"
#include "esp_err.h"

class DigitalOutput
{
private:
    /* data */
    /* To store result of configuring of Digital Output in constructor DigitalOutput(const gpio_num_t &digiGpioNum_); */
    gpio_num_t digiGpioNum; /* GPIOxx number */
    esp_err_t result;
    
public:
    /* Default Constructor */
    /* DigitalOutput(); */
    /* Constructor DigitalOutput(const gpio_num_t &digiGpioNum_); */
    DigitalOutput(const gpio_num_t &digiGpioNum_);
    /* Destructor */
    ~DigitalOutput();
    /* Public method - Initializate digital output pin */
    bool init();
    /* Public method -  Set the level of an output pin. The level should be either 0 (LOW) or 1 (HIGH). */
    void setLevel(const uint32_t &level_);
};

#endif /* __DIGITALOUTPUT_H__ */