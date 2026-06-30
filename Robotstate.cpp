#include "Robotstate.h"
#include <Preferences.h>

LegParams legs[4];

float bodyHeight = 0;
float bodyWidth  = 0;
float bodyStride = 40;
float bodyPitch  = 0;
float bodyRoll   = 0;

bool calibrateMode = false;
int calibrateLeg = 0;

// Definitions for computed per-leg arrays
float legHeights[4] = {70, 70, 70, 70};
float legWidths[4]  = {70, 70, 70, 70};

RobotStateMode commandedState = SIT;
RobotStateMode currentState = SIT;
int activeLeg = 0;

namespace {
Preferences prefs;
const char* NVS_NAMESPACE = "spider";

String legKey(int i, const char* field) {
  return "leg" + String(i) + "_" + field;
}
}

// Pitch pairs: front {0,1} vs rear {2,3}
// Roll pairs: right {0,2} vs left {1,3}
float calculateTiltTrim(int legId, float pitch, float roll) {
  float pitchPart = (legId == 0 || legId == 1) ? pitch : -pitch;
  float rollPart  = (legId == 0 || legId == 2) ? roll  : -roll;
  return pitchPart + rollPart;
}

// Function to update the kinematic arrays based on current body + leg parameters
void updateKinematicArrays() {
  for (int i = 0; i < 4; i++) {
    // Height = Base Body Height + Manual Leg H Trim + Tilt Trim (Pitch/Roll)
    legHeights[i] = bodyHeight + legs[i].H + calculateTiltTrim(i, bodyPitch, bodyRoll);
    // Width = Base Body Width + Manual Leg W Trim
    legWidths[i]  = bodyWidth + legs[i].W;
  }
}

void robotStateLoad() {
  prefs.begin(NVS_NAMESPACE, true);

  bodyHeight     = prefs.getFloat("bodyHeight", bodyHeight);
  bodyWidth      = prefs.getFloat("bodyWidth",  bodyWidth);
  bodyStride     = prefs.getFloat("bodyStride", bodyStride);
  bodyPitch      = prefs.getFloat("bodyPitch",  bodyPitch);
  bodyRoll       = prefs.getFloat("bodyRoll",   bodyRoll);
  activeLeg      = prefs.getInt("activeLeg",    activeLeg);
  commandedState = (RobotStateMode)prefs.getInt("cmdState", (int)SIT);
  currentState   = (RobotStateMode)prefs.getInt("curState", (int)SIT);

  for (int i = 0; i < 4; i++) {
    LegParams &l = legs[i];
    l.W             = prefs.getFloat(legKey(i, "W").c_str(),     l.W);
    l.H             = prefs.getFloat(legKey(i, "H").c_str(),     l.H);
    l.L             = prefs.getFloat(legKey(i, "L").c_str(),     l.L);
    l.offC          = prefs.getFloat(legKey(i, "offC").c_str(),  l.offC);
    l.offF          = prefs.getFloat(legKey(i, "offF").c_str(),  l.offF);
    l.offT          = prefs.getFloat(legKey(i, "offT").c_str(),  l.offT);
    l.cMin          = prefs.getInt(legKey(i, "cMin").c_str(),    l.cMin);
    l.cMax          = prefs.getInt(legKey(i, "cMax").c_str(),    l.cMax);
    l.fMin          = prefs.getInt(legKey(i, "fMin").c_str(),    l.fMin);
    l.fMax          = prefs.getInt(legKey(i, "fMax").c_str(),    l.fMax);
    l.tMin          = prefs.getInt(legKey(i, "tMin").c_str(),    l.tMin);
    l.tMax          = prefs.getInt(legKey(i, "tMax").c_str(),    l.tMax);
    l.powerDisabled = prefs.getBool(legKey(i, "pwrOff").c_str(), l.powerDisabled);
  }

  prefs.end();
  updateKinematicArrays();
  Serial.println("[State] loaded from flash & kinematic arrays initialized");
}

void robotStateSave() {
  prefs.begin(NVS_NAMESPACE, false);

  prefs.putFloat("bodyHeight", bodyHeight);
  prefs.putFloat("bodyWidth",  bodyWidth);
  prefs.putFloat("bodyStride", bodyStride);
  prefs.putFloat("bodyPitch",  bodyPitch);
  prefs.putFloat("bodyRoll",   bodyRoll);
  prefs.putInt("activeLeg",    activeLeg);
  prefs.putInt("cmdState",     (int)commandedState);
  prefs.putInt("curState",     (int)currentState);

  for (int i = 0; i < 4; i++) {
    LegParams &l = legs[i];
    prefs.putFloat(legKey(i, "W").c_str(),     l.W);
    prefs.putFloat(legKey(i, "H").c_str(),     l.H);
    prefs.putFloat(legKey(i, "L").c_str(),     l.L);
    prefs.putFloat(legKey(i, "offC").c_str(),  l.offC);
    prefs.putFloat(legKey(i, "offF").c_str(),  l.offF);
    prefs.putFloat(legKey(i, "offT").c_str(),  l.offT);
    prefs.putInt(legKey(i, "cMin").c_str(),    l.cMin);
    prefs.putInt(legKey(i, "cMax").c_str(),    l.cMax);
    prefs.putInt(legKey(i, "fMin").c_str(),    l.fMin);
    prefs.putInt(legKey(i, "fMax").c_str(),    l.fMax);
    prefs.putInt(legKey(i, "tMin").c_str(),    l.tMin);
    prefs.putInt(legKey(i, "tMax").c_str(),    l.tMax);
    prefs.putBool(legKey(i, "pwrOff").c_str(), l.powerDisabled);
  }

  prefs.end();
  Serial.println("[State] saved to flash");
}

void robotStateResetDefaults() {
  bodyHeight = 0.0;
  bodyWidth = 0.0;
  bodyStride = 40.0;
  bodyPitch = 0.0;
  bodyRoll = 0.0;

  activeLeg = 0;
  commandedState = SIT;

  for (int i = 0; i < 4; i++) {
    legs[i].W = 70.0;
    legs[i].H = 70.0;
    legs[i].L = 20.0;
    legs[i].offC = 90.0;
    legs[i].offF = 90.0;
    legs[i].offT = 90.0;
    legs[i].servoC = 90;
    legs[i].servoF = 90;
    legs[i].servoT = 90;
    legs[i].cMin = 20;
    legs[i].cMax = 160;
    legs[i].fMin = 20;
    legs[i].fMax = 160;
    legs[i].tMin = 20;
    legs[i].tMax = 160;
    legs[i].X = 0.0;
    legs[i].Y = 0.0;
    legs[i].Z = 0.0;
    legs[i].powerDisabled = false;
  }
 
  updateKinematicArrays();
}

void robotStatePrintAll() {
  Serial.printf("Body Height: %.2f, Width: %.2f, Stride: %.2f\n", bodyHeight, bodyWidth, bodyStride);
  Serial.printf("Body Pitch: %.2f, Roll: %.2f\n", bodyPitch, bodyRoll);
  for(int i=0; i<4; i++) {
    Serial.printf("Leg %d: W=%.2f, H=%.2f, L=%.2f\n", i, legs[i].W, legs[i].H, legs[i].L);
  }
}