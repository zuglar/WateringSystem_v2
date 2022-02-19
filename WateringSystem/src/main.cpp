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
/* Function Power On Channel 1 */
void powerOnCH1();
/* Function Power On Channel 2 */
void powerOnCH2();
/* Function reads analog sensors value On Channel 1 */
void readValueOnCH1();
/* Function reads analog sensors value On Channel 2 */
void readValueOnCH2();
/* Status of CH1 or CH2 */
bool channelSwithcedOn = false;

uint32_t lastSystemTimer1Interrupt = 0;
uint32_t lastWateringTimer1Interrupt = 0;

void setup() {
    // put your setup code here, to run once:

    delay(DELAY_2SEC);
    printf("APP SETUP Start.\n");
    mainAppError = false;
    asyncTcpWdt = false;
    updateData = false;

    /* If startUp function return result is false than the program stops */
    bool startUpFinished = false;
    bool startUpResult = false;
    while (!startUpFinished) {
        startUpResult = startUp();
        startUpFinished = true;
    }

    if (!startUpResult && startUpFinished) {
        mainAppError = true;
        return;
    }

    mainAppError = false;

    printf("APP SETUP End.\n");

    /* When the setup has been finished successfully the green LED turns ON */
    delay(DELAY_03_SEC);
    mainAppError = controller->getSdCard()->writeLogFile("Watering System MCU Started.");
    controller->getGreenLED()->setLevel(HIGH);
    /* Start Watering */
    systemTimer1Interrupt = 0;
    wateringTimer1Interrupt = 0;
    powerOnCH1();
    delay(DELAY_1SEC);
    readValueOnCH1();
    delay(DELAY_1SEC);
    powerOnCH2();
    delay(DELAY_1SEC);
    readValueOnCH2();
    controller->controllerStoreSensorsValue();
    controller->controllerPrepareWatering();
    systemTimer1Interrupt = 0;
    wateringTimer1Interrupt = 0;
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

    // if (asyncTcpWdt) {
    //     printf("1. LOOP: asyncTcpWdt: %d\n", asyncTcpWdt);
    //     /* controller->wifi32s->getClientData(); */
    //     asyncTcpWdt = false;
    //     delay(1);
    //     printf("2. LOOP: asyncTcpWdt: %d\n", asyncTcpWdt);
    // }
    delay(1);
    // Handle FTP connection
    if (controller->ftpServerStarted) {
        controller->wifi32s->ftp->handle();
    }
    // Saving last System and Watering Timer1 Interrupt value
    lastSystemTimer1Interrupt = systemTimer1Interrupt;
    lastWateringTimer1Interrupt = wateringTimer1Interrupt;

    // if the new settings of rule and system under upload process we have to wait to be finished
    // and set the base System and Watering Timer 1 Interrupt value to the last saved value
    while (updateNewSettingsProccess) {
        if (systemTimer1Interrupt > lastSystemTimer1Interrupt)
            systemTimer1Interrupt = lastSystemTimer1Interrupt;

        if (wateringTimer1Interrupt > lastWateringTimer1Interrupt)
            wateringTimer1Interrupt = lastSystemTimer1Interrupt;
    }

    if ((controller->systemRefreshInterval * 60) == systemTimer1Interrupt) {
        printf("System Duration Time reached!\n");

        if (controller->activeRuleExists) {
            controller->controllerCheckWateringRules();
        }

        if (controller->ddnsEnabled) {
            // Check for new public IP every 10 seconds
            EasyDDNS.update(10000);
        }

        systemTimer1Interrupt = 0;
        channelSwithcedOn = false;
    }

    if (systemTimer1Interrupt == POWERON_ANALOG_CH1 && !channelSwithcedOn) {
        powerOnCH1();
    }

    if (systemTimer1Interrupt == READ_SENSORS_CH1 && channelSwithcedOn) {
        readValueOnCH1();
    }

    if (systemTimer1Interrupt == POWERON_ANALOG_CH2 && !channelSwithcedOn) {
        powerOnCH2();
    }

    if (systemTimer1Interrupt == READ_SENSORS_CH2 && channelSwithcedOn) {
        readValueOnCH2();
        controller->controllerStoreSensorsValue();
        controller->controllerCheckWateringRules();
    }

    if (controller->wateringDurationTime == wateringTimer1Interrupt) {
        printf("Watering Duration Time reached!\n");

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

void powerOnCH1() {
    channelSwithcedOn = true;
    printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_1_5_CH1);
    controller->getPowerSensorsCH1()->setLevel(HIGH);
}

void powerOnCH2() {
    channelSwithcedOn = true;
    printf("Controler Power Channel: %d switched on.\n", POWER_SENORS_6_10_CH2);
    controller->getPowerSensorsCH2()->setLevel(HIGH);
}

void readValueOnCH1() {
    printf("Read sensors value on channel: %d\n", POWER_SENORS_1_5_CH1);
    channelSwithcedOn = false;
    controller->getSensorsValueCH1();
    controller->getPowerSensorsCH1()->setLevel(LOW);
}

void readValueOnCH2() {
    printf("Read sensors value on channel: %d\n", POWER_SENORS_6_10_CH2);
    channelSwithcedOn = false;
    controller->getSensorsValueCH2();
    controller->getPowerSensorsCH2()->setLevel(LOW);
}