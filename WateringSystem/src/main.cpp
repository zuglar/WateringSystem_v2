#include <Arduino.h>

#include "Controller.hpp"
#include "DefsVarsFuncs.hpp"
#include "InterruptTimer1.hpp"

volatile bool mainAppError;
volatile bool asyncTcpWdt;
volatile bool updateData;
/* Create Controller object */
Controller *controller = new Controller();
/* Create InterruptTimer1 object */
InterruptTimer1 *interruptTimer1 = new InterruptTimer1();

/* Function for initializing the configuration of system */
bool startUp(void);
/* Function for LED's message */
void ledFlashMessage(DigitalOutput *digitalOutput_, uint8_t flashTime_, uint16_t delay_);

void setup() {
    // put your setup code here, to run once:

    delay(DELAY_2SEC);
    printf("APP SETUP Start.\n");
    mainAppError = false;
    asyncTcpWdt = false;
    updateData = false;
    // printf("1. mainAppError: %d\n", mainAppError);
    /* If startUp function return result is false than the program stops */
    bool startUpFinished = false;
    bool startUpResult = false;
    while (!startUpFinished) {
        startUpResult = startUp();
        startUpFinished = true;
    }

    // printf("1. startUpResult: %d\n", startUpResult);
    if (!startUpResult && startUpFinished) {
        // printf("2. mainAppError: %d\n", mainAppError);
        mainAppError = true;
        return;
    }

    mainAppError = false;
    systemTimer1Interrupt = 0;
    wateringTimer1Interrupt = 0;
    // printf("3. mainInitErmainAppErrorror: %d\n", mainAppError);
    printf("APP SETUP End.\n");
    /* When the setup has been finished successfully the green LED turns ON */
    delay(DELAY_03_SEC);
    mainAppError = controller->getSdCard()->writeLogFile("Watering System MCU Started.");
    controller->getGreenLED()->setLevel(HIGH);
    delay(DELAY_2SEC);
    /* Start Watering */
    controller->controllerPrepareWatering();
    // ESP.getFreePsram();
}

void loop() {
    // put your main code here, to run repeatedly:
    if (mainAppError) {
        printf("LOOP: mainAppError: %d\n", mainAppError);
        delay(DELAY_03_SEC);
        controller->getRedLED()->setLevel(HIGH);
        printf("********** !!! ERROR OCCURRED !!! **********\n");
        printf("  !!! System will restart in 5 seconds. !!!\n");
        delay(5000);
        ESP.restart();
        return;
    }

    if (asyncTcpWdt) {
        printf("1. LOOP: asyncTcpWdt: %d\n", asyncTcpWdt);
        /* controller->wifi32s->getClientData(); */
        asyncTcpWdt = false;
        delay(1);
        printf("2. LOOP: asyncTcpWdt: %d\n", asyncTcpWdt);
    }
    delay(1);
    controller->wifi32s->ftp->handle();

    if ((controller->systemRefreshInterval * 60) == systemTimer1Interrupt) {
        printf("System Duration Time reached!\n");
        // if the new settings of rule and system under upload process we have to wait to be finished
        while (updateNewSettingsProccess) {
        }

        if (controller->activeRuleExists) {
            if (controller->checkTemperature) {
                printf("System Checks Temperature!\n");
                controller->controllerCheckTemperature();
            }

            if (controller->checkRainSensor) {
                printf("System Checks Rain Sensor!\n");
                controller->controllerCheckTemperature();
            }

            if (controller->checkSoilWetness) {
                printf("System Checks Soil Wetness!\n");
                controller->controllerCheckTemperature();
            }
        }

        systemTimer1Interrupt = 0;
    }

    if (controller->wateringDurationTime == wateringTimer1Interrupt) {
        printf("Watering Duration Time reached!\n");
        // if the new settings of rule and system under upload process we have to wait to be finished
        while (updateNewSettingsProccess) {
        }

        controller->controllerPrepareWatering();
        wateringTimer1Interrupt = 0;
    }
}

bool startUp(void) {
    /* Configure Digital Output */
    if (!controller->controllerDigitalOutputInit())
        return false;
    /* If configuration of Digital Outputs have been finished successfully the LED
      turns on-off first green LED after red LED */
    ledFlashMessage(controller->getGreenLED(), 1, DELAY_03_SEC);
    ledFlashMessage(controller->getRedLED(), 1, DELAY_03_SEC);
    /* Turns off valves */
    controller->valvesTurnOffOn(ALL_VALVES_OFF);
    delay(DELAY_1SEC);

    /* Initialization DS3231RTC object */
    if (!controller->controllerDS3231RTCInit())
        return false;
    /* If initialization of DS3231RTC has been finished successfully the green LED flashes one time. */
    ledFlashMessage(controller->getGreenLED(), 1, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Initialization SDCard object */
    if (!controller->controllerSDCardInit())
        return false;
    /* If initialization of SDcard has been finished successfully the green LED flashes two times. */
    ledFlashMessage(controller->getGreenLED(), 2, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Initialization Aht20Bmp280 object */
    if (!controller->controllerAht20Bmp280Init())
        return false;
    /* If initialization of Aht20Bmp280 has been finished successfully the green LED flashes three times. */
    ledFlashMessage(controller->getGreenLED(), 3, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Configure analog inputs */
    if (!controller->controllerAnalogInputsInit())
        return false;
    /* If initialization of Analog Inputs have been finished successfully the green led flashes four times. */
    ledFlashMessage(controller->getGreenLED(), 4, DELAY_03_SEC);
    delay(DELAY_1SEC);

    /* START - Collecting data from ws.ini file and Analog Inputs */
    /* Create array of analog sensors threshold values */
    if (!controller->analogSensorsThresholdValues())
        return false;
    /* If creation of array has been finished successfully the red led flashes one time. */
    ledFlashMessage(controller->getRedLED(), 1, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Get max dryness and wetness values of soil and refresh time interval from ws.ini file */
    if (!controller->getSystemGlobalValues())
        return false;
    /* If getting max dryness and wetness values of soil have been finished successfully the red led flashes two times. */
    ledFlashMessage(controller->getRedLED(), 2, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Read Aht20Bmp280 sensor values - temperature, humidity, air pressure */
    if (!controller->controllerGetAht20Bmp280Data())
        return false;
    /* If reading data from sensor has been finished successfully the red led flashes three times */
    ledFlashMessage(controller->getRedLED(), 3, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* If reading data from sensor has been finished successfully the red led flashes four times */
    ledFlashMessage(controller->getRedLED(), 4, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Read Analog sensors value / power channel */
    // controller->getPowerSensorsCH1()->setLevel(HIGH);
    // delay(DELAY_1SEC);
    // controller->controllerReadAnalogInputPinValue(POWER_SENORS_1_5_CH1);
    // controller->getPowerSensorsCH1()->setLevel(LOW);
    // controller->getPowerSensorsCH2()->setLevel(HIGH);
    // delay(DELAY_1SEC);
    // controller->controllerReadAnalogInputPinValue(POWER_SENORS_6_10_CH2);
    // controller->getPowerSensorsCH2()->setLevel(LOW);
    // delay(DELAY_03_SEC);
    // controller->setActiveValves();
    // controller->valvesTurnOffOn();
    // /* END - Collecting data from ws.ini file and Analog Inputs */
    // delay(DELAY_1SEC);
    /* Initialization WiFi32s object */
    if (!controller->controllerWiFi32sInit())
        return false;
    /* If initialization of WiFi32s has been finished successfully the green LED flashes one time.
    WEB and FTP servers have been started */
    ledFlashMessage(controller->getGreenLED(), 1, DELAY_03_SEC);
    delay(DELAY_1SEC);
    /* Initialization InterruptTimer1 object */
    if (!interruptTimer1->initInterruptTimer1())
        return false;
    /* If initialization of InterruptTimer1 has been finished successfully the green LED flashes two times. */
    ledFlashMessage(controller->getRedLED(), 2, DELAY_03_SEC);
    delay(DELAY_1SEC);

    /* Start Web Htm */
    /* controller->controllerStartWebHtm(); */
    // printf("Date NOW: %s\n", controller-);
    return true;
}

void ledFlashMessage(DigitalOutput *digitalOutput_, uint8_t flashTime_, uint16_t delay_) {
    for (int i = 0; i < flashTime_; i++) {
        digitalOutput_->setLevel(HIGH);
        delay(delay_);
        digitalOutput_->setLevel(LOW);
        delay(delay_);
    }
}
