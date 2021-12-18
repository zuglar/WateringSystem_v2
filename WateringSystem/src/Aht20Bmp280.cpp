#include "Aht20Bmp280.hpp"

/* Main objects of sensors */
Adafruit_AHTX0 aht;
Adafruit_BMP280 bmp;

Aht20Bmp280::Aht20Bmp280(/* args */) {}

Aht20Bmp280::~Aht20Bmp280() {}

bool Aht20Bmp280::init()
{
    //aht = new Adafruit_AHTX0();
    if (!aht.begin())
    {
        // Serial.println("Could not find AHT? Check wiring");
        //   while (1)
        //     delay(10);
        printf("AHT20 Error!\n");
        return false;
    }

    // bmp = new Adafruit_BMP280();
    if (!bmp.begin())
    {
        // while (1)
        //     delay(10);
        printf("BMP280 Error!\n");
        return false;
    }

    /* Default settings from datasheet. */
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                    Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                    Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                    Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                    Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

    printf("AHT20+BMP280 OK.\n");
    
    return true;
}

bool Aht20Bmp280::getAht20Bmp280Data(float &temperatue_, float &humdity_, float &pressure_) {
    if (aht.getStatus() == 255)    // Failed if status is 0xFF
        return false;
    
    sensors_event_t hum, temp;
    if(!aht.getEvent(&hum, &temp))
    {
        temperatue_ = 0.0;
        humdity_ = 0.0;
        pressure_ = 0.0;
        return false;
    }

    temperatue_ = temp.temperature;
    humdity_ = hum.relative_humidity;

    pressure_ = bmp.readPressure();

    return true;
}
