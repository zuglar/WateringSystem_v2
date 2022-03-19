/*  !!! All information are in DataSheets folder !!!
*
* We use ADC1 converter and channels: 
* ADC1_CHANNEL_0 (GPIO36) - Rain sensor 1-2
* ADC1_CHANNEL_3 (GPIO39) - Wetness sensor 1-5
* ADC1_CHANNEL_6 (GPIO34) - Wetness sensor 2-6
* ADC1_CHANNEL_7 (GPIO35) - Wetness sensor 3-7
* ADC1_CHANNEL_4 (GPIO32) - Wetness sensor 4-8
* ADC1_CHANNEL_5 (GPIO33)
*   !!! ADCH1, ADCH2 cannot be used !!!
* These pins connects some 270pF capacitors which determinate the gain of internal Low Noise Amplifier.
*/

/* AnalogInput header */
#ifndef __ANALOGINPUT_HPP__
#define __ANALOGINPUT_HPP__

#include <driver/adc.h>
#include "esp_err.h"
//#include "string.h"
#include <stdio.h>
#include <stdlib.h>
#include "DefsVarsFuncs.hpp"

class AnalogInput
{
private:
    /* To store result of calling the function adc1_config_channel_atten in AnalogInput(const adc1_channel_t analogPin_) constructor*/
    esp_err_t result;
    /* ADC1_CHANNEL_x channel */
    adc1_channel_t analogPin;
    /* Voltage scaling factor, scale the voltage between 0-3.6V */
    const adc_atten_t scaleVoltage = ADC_ATTEN_11db;
    /* ID of analog input */
    //const char *id;
    /* Private method - Read value of analog input (ADC1_CHANNEL_x) */
    uint32_t readAnalogInputPinValue(const adc1_channel_t analogPin_);
    /* Private objects for sensors */
    AnalogInput *rainSensors_1_2;
    AnalogInput *wetnessSensor_1_5;
    AnalogInput *wetnessSensor_2_6;
    AnalogInput *wetnessSensor_3_7;
    AnalogInput *wetnessSensor_4_8;

public:
    /* Default Constructor */
    AnalogInput();
    /* Contructor with adc1_channel_t argumet to set analog input channel*/
    AnalogInput(const adc1_channel_t analogPin_);
    /* Destructor */
    ~AnalogInput();
    bool init();
    /* Public method - Store values of analog input pins (ADC1_CHANNEL_x) which are on power chanel 1 or 2 */
    void storeAnalogInputPinValue(const gpio_num_t powerChannel_, int *array_);
    /* Public method - Store valueof rain sensor*/
    void storeRainSensorPinValue(int *array_);
};
#endif /* __ANALOGINPUT_H__ */
