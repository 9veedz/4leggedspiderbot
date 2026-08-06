#ifndef ROBOT_STATE_H
#define ROBOT_STATE_H

#include <Arduino.h>
// Add to RobotStateMode enum
enum RobotStateMode {
  SIT,
  STAND,
  FORWARD,
  REVERSE,   // ← add this
};

struct LegParams {
  float W = 0, H = 0, L = 20;
  float offC = 90, offF = 90, offT = 90;
  int   servoC = 90, servoF = 90, servoT = 90;
  int   cMin = 0, cMax = 180;
  int   fMin = 0, fMax = 180;
  int   tMin = 0, tMax = 180;
  float X = 70, Y = 0, Z = -70;
  bool powerDisabled = false;
};

extern LegParams legs[4];

extern float bodyHeight;
extern float bodyWidth;
extern float bodyStride;
extern float bodyPitch;
extern float bodyRoll;

// Evaluated per-leg kinematic arrays
extern float legHeights[4];
extern float legWidths[4];

extern int activeLeg;
extern RobotStateMode commandedState;
extern RobotStateMode currentState;

extern bool calibrateMode;
extern int calibrateLeg;  // which leg is being calibrated, mirrors activeLeg

void robotStateLoad();
void robotStateSave();
void robotStateResetDefaults();
void robotStatePrintAll();

// Centralized functions to compute kinematic arrays and tilt trims
void updateKinematicArrays();
float calculateTiltTrim(int legId, float pitch, float roll);

#endif