#include "DigitalOutput.hpp"

DigitalOutput::DigitalOutput(const gpio_num_t &digiGpioNum_)
{
    digiGpioNum = digiGpioNum_;
    /* Specify that a given pin should be used for GPIO. */
    gpio_pad_select_gpio(digiGpioNum);
}

bool DigitalOutput::init()
{
    /* Set the mode of the pin to GPIO_MODE_OUTPUT. */
    if (gpio_set_direction(digiGpioNum, GPIO_MODE_OUTPUT) != ESP_OK)
        return false;

    /* Set the level of the pin to 0 (LOW)*/
    if (gpio_set_level(digiGpioNum, 0) != ESP_OK)
    {
        printf("Digital Output GPIO%d Erro!\n", digiGpioNum);
        return false;
    }

    printf("Digital Output GPIO%d OK.\n", digiGpioNum);
    return true;
}

void DigitalOutput::setLevel(const uint32_t &level_)
{
    gpio_set_level(digiGpioNum, level_);
}

gpio_num_t DigitalOutput::getDigiGpioNum() const
{
    return digiGpioNum;
}
