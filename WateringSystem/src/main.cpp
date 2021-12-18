#include <Arduino.h>

#include "DefsVarsFuncs.hpp"
#include "Controller.hpp"

volatile bool mainInitError;
/* Create Controller object */
Controller *controller = new Controller();

/* Function for initializing the configuration of system */
bool startUp(void);
/* Function for LED's message */
void ledFlashMessage(DigitalOutput *digitalOutput_, uint8_t flashTime_, uint16_t delay_);

void setup()
{
  // put your setup code here, to run once:
  delay(DELAY_2SEC);
  mainInitError = false;
  /* If startUp function return result is false than the program stops */
  if (!startUp())
  {
    mainInitError = true;
    return;
  }
  
  delay(DELAY_2SEC);
  /* When the setup has been finished successfully the green LED turns ON */
    controller->getGreenLED()->setLevel(HIGH);
}

void loop()
{
  // put your main code here, to run repeatedly:
}

bool startUp(void)
{
  /* Configure Digital Output */
  if (!controller->controllerDigitalOutputInit())
    return false;
  /* If configuration of Digital Outputs have been finished successfully the LED
    turns on-off first green LED after red LED */
  ledFlashMessage(controller->getGreenLED(), 1, DELAY_SHORT);
  ledFlashMessage(controller->getRedLED(), 1, DELAY_SHORT);
  delay(DELAY_1SEC);
  
  /* Initialization DS3231RTC object */
  if (!controller->controllerDS3231RTCInit())
    return false;
  /* If initialization of DS3231RTC has been finished successfully the green LED flashes one time. */
  ledFlashMessage(controller->getGreenLED(), 1, DELAY_SHORT);
  
  
  return true;
}

void ledFlashMessage(DigitalOutput *digitalOutput_, uint8_t flashTime_, uint16_t delay_)
{
  for (int i = 0; i < flashTime_; i++)
  {
    digitalOutput_->setLevel(HIGH);
    delay(delay_);
    digitalOutput_->setLevel(LOW);
    delay(delay_);
  }
}