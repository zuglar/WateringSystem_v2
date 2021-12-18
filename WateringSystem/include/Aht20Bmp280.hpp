/*  !!! All information are in DataSheets folder !!!
*
*   Connection:
*   AHT20+BMP280 pin     NodeMCU-32S pin
*       GND ------------------------------- Power GND
*       Vcc ------------------------------- Power 3.3 V
*       SDA --------------- SDA-GPIO21
*       SCL --------------- SCL-GPIO22    
*/

#ifndef __AHT20BMP280_HPP__
#define __AHT20BMP280_HPP__
/* Include Adafriut headers for AHT20 and BMP280 sensors */
#include <Adafruit_AHTX0.h>
#include <Adafruit_BMP280.h>

class Aht20Bmp280
{
private:
    /* data */
public:
    /* Constructor */
    Aht20Bmp280(/* args */);
    /* Destructor */
    ~Aht20Bmp280();
    /* Public method - init AHT20 BMP280 sensor - if initialization successful the return is TRUE else FALSE */
    bool init();
    /* Public method - Gat data from AHT20 and BMP280 sensors */
    bool getAht20Bmp280Data(float &temperatue_, float &humdity_, float &pressure_);
};
#endif /* __AHT20BMP280_HPP__ */
