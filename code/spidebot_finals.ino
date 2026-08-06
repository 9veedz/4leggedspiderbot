#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include "Robotstate.h"
#include "Spider.h"
#include "Spiderserver.h"

// Defined exactly once, here. Declared extern in SpiderLeg.h.
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

Spider spider;

void setup() {
  Serial.begin(115200);

  // I2C on custom pins: SDA=8, SCL=9
  Wire.begin(7, 6);

  pwm.begin();
  pwm.setPWMFreq(50); // standard analog servo frequency

  robotStateLoad();

  spider.begin();

  spiderServerBegin();
}

void loop() {
  spiderServerLoop();

  if (commandedState == FORWARD) {
    spider.walkForward();
  } else if (commandedState == REVERSE) {
    spider.walkReverse();
  } else if (commandedState == STAND) {
    spider.stand();
  } else if (commandedState == SIT) {
    spider.sit();
  }

  spider.updateAll();
}