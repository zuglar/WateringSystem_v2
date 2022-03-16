/* InterruptTimer1 header */
#ifndef __INTERRUPTTIMER1_HPP__
#define __INTERRUPTTIMER1_HPP__
#include "Arduino.h"
#include "driver/timer.h"
#include "esp_err.h"

// extern volatile uint32_t secondTimer1Interrupt;
extern volatile uint32_t wateringTimer1Interrupt;
extern volatile uint32_t systemTimer1Interrupt;

class InterruptTimer1
{
private:
    /* ESP error code */
    // esp_err_t error_code;
    /* Timer1 configuration */
    timer_config_t timer1Conf;
    // String errorMessage(esp_err_t code_);
    
public:
    /* Constructor */
    InterruptTimer1(/* args */);
    /* Destructor */
    ~InterruptTimer1();
    /* Public method - init timer1 interrupt */
    bool initInterruptTimer1();
};

#endif
