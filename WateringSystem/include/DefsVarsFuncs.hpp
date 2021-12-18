#ifndef __DEFSVARSFUNCS_HPP__
#define __DEFSVARSFUNCS_HPP__

#include <stdio.h>

/* DS3231RTC Defines */
#define DS3231_CONTROL_STATUS_REGISTER_ADDRESS 0x68
#define EEPROM_24C32_ADDRESS 0x53
#define EEPROM_24C32_READ_START_ADDRESS 0
/* Digital Output Defines */
#define RED_LED GPIO_NUM_26               /* Turn off/on RED_LED indicator*/
#define GREEN_LED GPIO_NUM_27             /* Turn off/on GREEN_LED indicator*/
#define POWER_SENORS_1_5_CH1 GPIO_NUM_25  /* Turn off/on rain 1 and wetness 1-4 sensors which are connected on channel 1 */
#define POWER_SENORS_6_10_CH2 GPIO_NUM_33 /* Turn off/on rain 2 and wetness 5-8 sensors which are connected on channel 2 */
#define STCP GPIO_NUM_4                   /* RCLK/STCP (shift register clock input, latch pin) - ST_CP. 74HC595 pin 12 */
#define SHCP GPIO_NUM_16                  /* SRCLK/SHCP (storage register clock pin, SPI clock) - SH_CP. 74HC595 pin 11 */
#define DS GPIO_NUM_17                    /* SER/DS (serial data input, SPI data - DS. 74HC595 pin 14  */
#define DELAY_SHORT 300                   /* Delay of LED flash 0.3ms */
#define DELAY_1SEC 1000                   /* Delay of LED flash 1s */
#define DELAY_2SEC 2000                   /* Delay of LED flash 2s */
/* Main Variables */

/* Main Functions */
#endif /* __DEFSVARSFUNCS_HPP__ */