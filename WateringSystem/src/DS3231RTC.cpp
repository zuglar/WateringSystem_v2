#include "DS3231RTC.hpp"

/* Main object of RTC */
RTC_DS3231 rtc;

DS3231RTC::DS3231RTC(/* args */) {}

DS3231RTC::~DS3231RTC() {}

bool DS3231RTC::init()
{
    Wire.begin();
    if (!rtc.begin() || rtc.lostPower())
    {
        printf("RTC Error!\n");
        return false;
    }
    // Base settings
    rtc.disable32K();
    rtc.clearAlarm(1);
    rtc.clearAlarm(2);
    rtc.writeSqwPinMode(DS3231_OFF);
    rtc.disableAlarm(1);
    rtc.disableAlarm(2);
    printf("RTC OK.\n");
    return true;
}

bool DS3231RTC::isRtcRunning()
{
    Wire.beginTransmission(DS3231_CONTROL_STATUS_REGISTER_ADDRESS);
    Wire.write(0x0F); /* pointing at Control/Status Register that contains OSF flag bit at bit-7 position */
    Wire.endTransmission();
    Wire.requestFrom(DS3231_CONTROL_STATUS_REGISTER_ADDRESS, 1); /* request to get the content of Control/Status Register */
    byte y = Wire.read();
    if (!((y & 0x80) != 0x80))
    {
        printf("RTC is not running!"); /* (OSF) = 1 */
        return false;
    }
    return true; /* (OSF) = 0 */
}

String DS3231RTC::getDateNow()
{
    DateTime dateTimeNow = rtc.now();
    return (dateTimeNow.timestamp(DateTime::TIMESTAMP_DATE));
}

String DS3231RTC::getDateTimeNow()
{
    DateTime dateTimeNow = rtc.now();
    String date = dateTimeNow.timestamp(DateTime::TIMESTAMP_FULL);
    date.replace("T", " ");
    return (date);
}

String DS3231RTC::oldLogFileDate()
{
    DateTime dateTimePast = rtc.now() - TimeSpan(3, 0, 0, 0);
    return (dateTimePast.timestamp(DateTime::TIMESTAMP_DATE));
}

/* https://lastminuteengineers.com/ds3231-rtc-arduino-tutorial/ */
String DS3231RTC::getAdminPwd()
{
    unsigned int addr = EEPROM_24C32_READ_START_ADDRESS; //first address
    String pwd = String();
    /* Access the first address from the memory */
    byte b = i2c24C32EEPROMReadByte(EEPROM_24C32_READ_START_ADDRESS);
    while (b != 0)
    {
        pwd += (char)b;
        addr++;                           /* increase address */
        b = i2c24C32EEPROMReadByte(addr); /* access an address from the memory */
    }
    return pwd;
}

byte DS3231RTC::i2c24C32EEPROMReadByte(unsigned int eepromaddress_)
{
    byte rdata = 0xFF;
    Wire.beginTransmission(EEPROM_24C32_ADDRESS);
    Wire.write((int)(eepromaddress_ >> 8));   // MSB
    Wire.write((int)(eepromaddress_ & 0xFF)); // LSB
    Wire.endTransmission();
    Wire.requestFrom(EEPROM_24C32_ADDRESS, 1);
    if (Wire.available())
        rdata = Wire.read();
    return rdata;
}
