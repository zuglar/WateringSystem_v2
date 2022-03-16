#include "InterruptTimer1.hpp"

volatile uint32_t wateringTimer1Interrupt;
volatile uint32_t systemTimer1Interrupt;

void IRAM_ATTR timer1ISR(void *arg)
{
    int timerIdx = (int)arg;
    if (timerIdx == TIMER_1)
    {
        TIMERG1.hw_timer[timerIdx].update = 1;
        TIMERG1.int_clr_timers.t1 = 1;
        TIMERG1.hw_timer[timerIdx].config.alarm_en = true;
        wateringTimer1Interrupt++;
        systemTimer1Interrupt++;
    }
}
/* 
String InterruptTimer1::errorMessage(esp_err_t code_) 
{
    switch (code_)
    {
    case ESP_OK:
        return "OK";
    case ESP_FAIL:
        return "Fail";
    case ESP_ERR_NO_MEM:
        return "No memory";
    case ESP_ERR_INVALID_ARG:
        return "Invalid argument";
    case ESP_ERR_INVALID_SIZE:
        return "Invalid size";
    case ESP_ERR_INVALID_STATE:
        return "Invalid state";
    case ESP_ERR_NOT_FOUND:
        return "Not found";
    case ESP_ERR_NOT_SUPPORTED:
        return "Not supported";
    case ESP_ERR_TIMEOUT:
        return "Timeout";
    case ESP_ERR_INVALID_RESPONSE:
        return "Received response was invalid";
    case ESP_ERR_INVALID_CRC:
        return "CRC or checksum was invalid";
    case ESP_ERR_INVALID_VERSION:
        return "Version was invalid";
    }
    return "Unknown ESP_ERR error";
}
 */
InterruptTimer1::InterruptTimer1(/* args */) {}

InterruptTimer1::~InterruptTimer1() {}

bool InterruptTimer1::initInterruptTimer1() 
{
    esp_err_t errorCode = ESP_OK;
    timer_idx_t timerIdx = TIMER_1;
    timer1Conf.divider = 80;
    timer1Conf.counter_dir = TIMER_COUNT_UP;
    timer1Conf.counter_en = TIMER_PAUSE;
    timer1Conf.alarm_en = true;
    timer1Conf.auto_reload = true;
    timer1Conf.intr_type = TIMER_INTR_LEVEL;
    // Configure timer
    ESP_ERROR_CHECK(errorCode = timer_init(TIMER_GROUP_1, timerIdx, &timer1Conf));
    // Stop timer counter
    // timer_pause(TIMER_GROUP_1, timerIdx);
    ESP_ERROR_CHECK(errorCode = timer_pause(TIMER_GROUP_1, timerIdx));
    // Set alarm value
    // timer_set_alarm_value(TIMER_GROUP_1, timerIdx, 500000); // 0.5 s
    ESP_ERROR_CHECK(errorCode = timer_set_alarm_value(TIMER_GROUP_1, timerIdx, 1000000));
    // Load counter value
    // timer_set_counter_value(TIMER_GROUP_1, timerIdx, 0);
    ESP_ERROR_CHECK(errorCode = timer_set_counter_value(TIMER_GROUP_1, timerIdx, 0));
    // Enable timer interrupt
    // timer_enable_intr(TIMER_GROUP_1, timerIdx);
    ESP_ERROR_CHECK(errorCode = timer_enable_intr(TIMER_GROUP_1, timerIdx));
    // Set ISR handler
    // timer_isr_register(TIMER_GROUP_1, timerIdx, &timer1ISR, (void *)timerIdx, ESP_INTR_FLAG_IRAM, NULL);
    ESP_ERROR_CHECK(errorCode = timer_isr_register(TIMER_GROUP_1, timerIdx, &timer1ISR, (void *)timerIdx, ESP_INTR_FLAG_IRAM, NULL));
    // Start timer counter
    // timer_start(TIMER_GROUP_1, timerIdx);
    ESP_ERROR_CHECK(errorCode = timer_start(TIMER_GROUP_1, timerIdx));
    
    if (errorCode != ESP_OK)
    {
        // printf("ERROR InterruptTimer1: %s...!!!\n", errorMessage(errorCode).c_str());
        return false;
    }
    // printf("InterruptTimer1: %s...!!!\n", errorMessage(errorCode).c_str());
    return true;
}



