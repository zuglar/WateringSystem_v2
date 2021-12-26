#ifndef __DEFSVARSFUNCS_HPP__
#define __DEFSVARSFUNCS_HPP__

#include <stdio.h>
/* Main Defines */
#define DELAY_03_SEC 300 /* Delay of LED flash 0.3ms */
#define DELAY_1SEC 1000  /* Delay of LED flash 1s */
#define DELAY_2SEC 2000  /* Delay of LED flash 2s */
/* Digital Output Defines */
#define RED_LED GPIO_NUM_26               /* Turn off/on RED_LED indicator*/
#define GREEN_LED GPIO_NUM_27             /* Turn off/on GREEN_LED indicator*/
#define POWER_SENORS_1_5_CH1 GPIO_NUM_25  /* Turn off/on rain 1 and wetness 1-4 sensors which are connected on channel 1 */
#define POWER_SENORS_6_10_CH2 GPIO_NUM_33 /* Turn off/on rain 2 and wetness 5-8 sensors which are connected on channel 2 */
#define STCP GPIO_NUM_4                   /* RCLK/STCP (shift register clock input, latch pin) - ST_CP. 74HC595 pin 12 */
#define SHCP GPIO_NUM_16                  /* SRCLK/SHCP (storage register clock pin, SPI clock) - SH_CP. 74HC595 pin 11 */
#define DS GPIO_NUM_17                    /* SER/DS (serial data input, SPI data - DS. 74HC595 pin 14  */
/* DS3231RTC Defines */
#define DS3231_CONTROL_STATUS_REGISTER_ADDRESS 0x68
#define EEPROM_24C32_ADDRESS 0x53
#define EEPROM_24C32_READ_START_ADDRESS 0
/* DS3231RTC Defines */
#define SD_CS_PIN 5 /* GPIO5  define Card Select pin */
#define EMPTY_STRING ""
#define SDCARD_FREQUENY 4000000
#define SDCARD_MOINTPOINT "/sd"
#define SDCARD_MAX_FILES 10
#define LOG_DIR_NAME "/LOG"   /* Name of log directory */
#define WS_INI_FILE "/ws.ini" /* Main  ws ini file name stored on the SDcard */
/* Analog Inputs Defines */
#define RSG_1_2 ADC1_CHANNEL_0          /* Analog input of Rain sensor group 1_2 GPIO36 */
#define WSG_1_5 ADC1_CHANNEL_3          /* Analog input of Wetness sensor group 1_5  GPIO39 */
#define WSG_2_6 ADC1_CHANNEL_6          /* Analog input of Wetness sensor group 2_6  GPIO34 */
#define WSG_3_7 ADC1_CHANNEL_7          /* Analog input of Wetness sensor group 3_7  GPIO35 */
#define WSG_4_8 ADC1_CHANNEL_4          /* Analog input of Wetness sensor group 4_8  GPIO32 */
#define WS_1_VALUE_ARRAY_INDEX 0        /* Array index for value of Wetness sensor 1 */
#define WS_2_VALUE_ARRAY_INDEX 1        /* Array index for value of Wetness sensor 2 */
#define WS_3_VALUE_ARRAY_INDEX 2        /* Array index for value of Wetness sensor 3 */
#define WS_4_VALUE_ARRAY_INDEX 3        /* Array index for value of Wetness sensor 4 */
#define WS_5_VALUE_ARRAY_INDEX 4        /* Array index for value of Wetness sensor 5 */
#define WS_6_VALUE_ARRAY_INDEX 5        /* Array index for value of Wetness sensor 6 */
#define WS_7_VALUE_ARRAY_INDEX 6        /* Array index for value of Wetness sensor 7 */
#define WS_8_VALUE_ARRAY_INDEX 7        /* Array index for value of Wetness sensor 8 */
#define RS_1_VALUE_ARRAY_INDEX 8        /* Array index for value of rain sensor 1 */
#define RS_2_VALUE_ARRAY_INDEX 9        /* Array index for value of rain sensor 2 */
#define RAIN_SENSORS_QUANTITY 2         /* Number of rain sensors can be used in the system */
/* Controller Defines */
#define ANALOG_DATA_ARRAY_SIZE 10
#define SN74HC595_STEPS 2
#define VALVES_BINARY_STRING_LENGHT 8
#define SENSOR_NOT_IN_USE "0"
#define RAIN_SENSOR_RAINS "1"
#define RAIN_SENSOR_NOT_RAINS "0"
/* ws.ini file SECTIONS and KEYS */
#define READY_SECTION "Ready"
#define READY_KEY "READY"
#define ANALOGSENSORS_SECTION "AnalogSensors"
#define WIFI_AP_SECTION "WiFiAP"
#define WIFI_STA_SECTION "WiFiSTA"
#define WIFI_HIDDEN_KEY "HIDDEN"
#define WIFI_SSID_KEY "SSID"
#define WIFI_STASET_KEY "STASET"
#define WIFI_PWD_KEY "PWD"
#define WIFI_CHANNEL_KEY "CHANNEL"
#define WIFI_MAX_CONNECTION_KEY "MAX_CONNECTION"
#define WIFI_STATIC_IP_KEY "STATIC_IP"
#define WIFI_IP_KEY "IP"
#define WIFI_SUBNET_KEY "SUBNET"
#define WIFI_GATEWAY_KEY "GATEWAY"
#define WIFI_DNS_KEY "DNS"
#define WIFI_NO_DNS_DATA "0.0.0.0"
#define SENSORS_VALUE_INTERVAL_SECTION "SensorsValueInterval"
#define INTERVAL_KEY "INTERVAL"
#define WETNESS_DRYNESS_SECTION "WetnessDryness"
#define WETNESS_KEY "WETNESS"
#define DRYNESS_KEY "DRYNESS"
/* WiFi32s Defines */
#define PAGE_WIFI "wifi"
#define PAGE_WATERING "watering"
#define PAGE_ADMIN "admin"
#define INDEX_HTM_FILE "/resources/index.htm"
#define WIFI_HTM_FILE "/resources/wifi.htm"
#define ERROR_HTM_FILE "/resources/error.htm"
#define CORRECT_HTM_FILE "/resources/correct.htm"
#define ADMIN_HTM_FILE "/resources/admin.htm"
#define FTP_USER "ftp"
#define FTP_PASSWORD "ftp"

/* Main Variables */
extern volatile bool mainAppError;
extern volatile bool asyncTcpWdt;
/* Main Functions */

#endif /* __DEFSVARSFUNCS_HPP__ */