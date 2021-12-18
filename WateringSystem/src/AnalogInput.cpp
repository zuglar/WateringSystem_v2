#include "AnalogInput.hpp"

AnalogInput::AnalogInput() {}

AnalogInput::AnalogInput(const adc1_channel_t analogPin_)
{
    analogPin = analogPin_;
    result = adc1_config_channel_atten(analogPin, scaleVoltage);
}

AnalogInput::~AnalogInput() {}

bool AnalogInput::init()
{
    /* Set ADC1 resolution 12bit (0-4095). If error occurs we stop the program.*/
    if (adc1_config_width(ADC_WIDTH_12Bit) != ESP_OK) /* adc1_config_width(ADC_WIDTH_12Bit) */
        return false;

    /* Create objects for Rain Sensor(1-2) and Wetness Sensors (1-8). If error occurs we stop the program. */
    rainSensors_1_2 = new AnalogInput(RSG_1_2);
    if (rainSensors_1_2->result != ESP_OK)
        return false;

    wetnessSensor_1_5 = new AnalogInput(WSG_1_5);
    if (wetnessSensor_1_5->result != ESP_OK)
        return false;

    wetnessSensor_2_6 = new AnalogInput(WSG_2_6);
    if (wetnessSensor_2_6->result != ESP_OK)
        return false;

    wetnessSensor_3_7 = new AnalogInput(WSG_3_7);
    if (wetnessSensor_3_7->result != ESP_OK)
        return false;

    wetnessSensor_4_8 = new AnalogInput(WSG_4_8);
    if (wetnessSensor_4_8->result != ESP_OK)
        return false;

    printf("Analog Inputs: %d, %d OK.\n", RSG_1_2, WSG_1_5);
    return true;
}

int AnalogInput::readAnalogInputPinValue(const adc1_channel_t analogPin_) 
{
    return adc1_get_raw(analogPin_);
}

void AnalogInput::showErrorMessage(adc1_channel_t adc1Channel_) {
    printf("Analog Input \n");
}

void AnalogInput::storeAnalogInputPinValue(const gpio_num_t powerChannel_, int *array_)
{
    if (powerChannel_ == POWER_SENORS_1_5_CH1)
    {
        array_[RS_1_VALUE_ARRAY_INDEX] = rainSensors_1_2->readAnalogInputPinValue(rainSensors_1_2->analogPin);
        array_[WS_1_VALUE_ARRAY_INDEX] = wetnessSensor_1_5->readAnalogInputPinValue(wetnessSensor_1_5->analogPin);
        array_[WS_2_VALUE_ARRAY_INDEX] = wetnessSensor_2_6->readAnalogInputPinValue(wetnessSensor_2_6->analogPin);
        array_[WS_3_VALUE_ARRAY_INDEX] = wetnessSensor_3_7->readAnalogInputPinValue(wetnessSensor_3_7->analogPin);
        array_[WS_4_VALUE_ARRAY_INDEX] = wetnessSensor_4_8->readAnalogInputPinValue(wetnessSensor_4_8->analogPin);
    }
    
    if (powerChannel_ == POWER_SENORS_6_10_CH2)
    {
        array_[RS_2_VALUE_ARRAY_INDEX] = rainSensors_1_2->readAnalogInputPinValue(rainSensors_1_2->analogPin);
        array_[WS_5_VALUE_ARRAY_INDEX] = wetnessSensor_1_5->readAnalogInputPinValue(wetnessSensor_1_5->analogPin);
        array_[WS_6_VALUE_ARRAY_INDEX] = wetnessSensor_2_6->readAnalogInputPinValue(wetnessSensor_2_6->analogPin);
        array_[WS_7_VALUE_ARRAY_INDEX] = wetnessSensor_3_7->readAnalogInputPinValue(wetnessSensor_3_7->analogPin);
        array_[WS_8_VALUE_ARRAY_INDEX] = wetnessSensor_4_8->readAnalogInputPinValue(wetnessSensor_4_8->analogPin);
    }
}

