#ifndef SPIDERLEG_H
#define SPIDERLEG_H

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>
#include "Robotstate.h" // Links with global tracking configs

// Reference the global pwm driver instance from your main sketch
extern Adafruit_PWMServoDriver pwm;

class SpiderLeg {
private:
  const float L1 = 27.5, L2 = 55.0, L3 = 77.5;
  int cPin, fPin, tPin;
  float curX, curY, curZ, tarX, tarY, tarZ;
  float stepSize = 1.0;
  float angC, angF, angT;
 
  
  int legId;       // Holds individual leg index (0 to 3)
  bool mirrored;   // Calculated automatically in constructor

public:
  // Constructor takes pins and the assigned leg index ID
  SpiderLeg(int cp, int fp, int tp, int id);

  void setTarget(float x, float y, float z);

  // Independent single-axis setters — each takes a raw value and drives
  // only its own target axis, leaving the other two untouched.
  void setX(float x);
  void setY(float y);
  void setZ(float z);

  void calibrateServos();
  void calculateIK(float x, float y, float z);
  void updatePosition();
  bool isMoving();
  void legOff();
  void legOn();

  // Swimming gait primitives — each touches exactly one axis via setX/Y/Z
  void lift(float L);
  void down();
  void strideF(float strideY);
  void home();

  // Lateral crab-walk primitive (one-shot X-axis reach)
  void crab(float strideX);
 
};

#endif