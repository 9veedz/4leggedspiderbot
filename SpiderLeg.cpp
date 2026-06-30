#include "SpiderLeg.h"

SpiderLeg::SpiderLeg(int cp, int fp, int tp, int id)
    : cPin(cp), fPin(fp), tPin(tp), legId(id) {
  
  // Rule mapping: Legs 1 and 2 are on the mirrored side of the chassis
  mirrored = (legId == 1 || legId == 2);

  curX = tarX = 0.0;
  curY = tarY = 0.0;
  curZ = tarZ = 0.0;
}

void SpiderLeg::setX(float x) { tarX = legWidths[legId] + x; }
void SpiderLeg::setY(float y) { tarY = mirrored ? -(y + 20) : (y + 20); }
void SpiderLeg::setZ(float z) { tarZ = -legHeights[legId] + z; }

void SpiderLeg::setTarget(float x, float y, float z) {
  setX(x); setY(y); setZ(z);
}


void SpiderLeg::calibrateServos() {
  // 1. Pull raw manual angles directly from Web UI sliders (0 to 180)
  float targetC = legs[legId].servoC;
  float targetF = legs[legId].servoF;
  float targetT = legs[legId].servoT;

  // 2. Pull min/max safety limits live from the global configuration
  int cMin = legs[legId].cMin, cMax = legs[legId].cMax;
  int fMin = legs[legId].fMin, fMax = legs[legId].fMax;
  int tMin = legs[legId].tMin, tMax = legs[legId].tMax;

  // 3. Clamp raw slider inputs directly to safety limits (No offsets applied)
  float outC = constrain(targetC, cMin, cMax);
  float outF = constrain(targetF, fMin, fMax);
  float outT = constrain(targetT, tMin, tMax);

  // 4. Write directly to PCA9685 using your physical chassis orientation mapping
  if (mirrored) {
    pwm.setPWM(cPin, 0, map(outC, 0, 180, 600, 150));
    pwm.setPWM(fPin, 0, map(outF, 0, 180, 150, 600));
    pwm.setPWM(tPin, 0, map(outT, 0, 180, 150, 600));
  } else {
    pwm.setPWM(cPin, 0, map(outC, 0, 180, 150, 600));
    pwm.setPWM(fPin, 0, map(outF, 0, 180, 600, 150));
    pwm.setPWM(tPin, 0, map(outT, 0, 180, 600, 150));
  }
}

void SpiderLeg::calculateIK(float x, float y, float z) {
  if (x == 0)
    x = 0.01;
  angC = atan2(y, x) * 180.0 / PI;
  float R = sqrt(x * x + y * y) - L1;
  float D_dist = sqrt(R * R + z * z);
  float cos_t = (pow(L2, 2) + pow(L3, 2) - pow(D_dist, 2)) / (2 * L2 * L3);
  angT = acos(constrain(cos_t, -1, 1)) * 180.0 / PI;
  float alpha = atan2(z, R);
  float cos_f = (pow(L2, 2) + pow(D_dist, 2) - pow(L3, 2)) / (2 * L2 * D_dist);
  angF = (alpha + acos(constrain(cos_f, -1, 1))) * 180.0 / PI;
}

void SpiderLeg::updatePosition() {
  // Accuracy window changed to 1 for physical servo stability
   if (legs[legId].powerDisabled) return;
   
  if (abs(tarX - curX) > 1)
    curX += (tarX > curX) ? stepSize : -stepSize;
  if (abs(tarY - curY) > 1)
    curY += (tarY > curY) ? stepSize : -stepSize;
  if (abs(tarZ - curZ) > 1)
    curZ += (tarZ > curZ) ? stepSize : -stepSize;

  float ikY = mirrored ? -curY : curY;  // Flip Y into IK for mirrored legs
  calculateIK(curX, ikY, curZ);

  // Pull calibration offsets live from the global shared structure array
  float liveOffC = legs[legId].offC;
  float liveOffF = legs[legId].offF;
  float liveOffT = legs[legId].offT;

  // Pull min/max safety limits live from the same structure
  int cMin = legs[legId].cMin, cMax = legs[legId].cMax;
  int fMin = legs[legId].fMin, fMax = legs[legId].fMax;
  int tMin = legs[legId].tMin, tMax = legs[legId].tMax;

  // Clamp final (angle + offset) result to each joint's configured safe range
  // before it's ever mapped to a PWM pulse.
  float outC = constrain(angC + liveOffC, cMin, cMax);
  float outF = constrain(angF + liveOffF, fMin, fMax);
  float outT = constrain(90 - angT + liveOffT, tMin, tMax);

  if (mirrored) {
    pwm.setPWM(cPin, 0, map(outC, 0, 180, 600, 150));
    pwm.setPWM(fPin, 0, map(outF, 0, 180, 150, 600));
    pwm.setPWM(tPin, 0, map(outT, 0, 180, 150, 600));
  } else {
    pwm.setPWM(cPin, 0, map(outC, 0, 180, 150, 600));
    pwm.setPWM(fPin, 0, map(outF, 0, 180, 600, 150));
    pwm.setPWM(tPin, 0, map(outT, 0, 180, 600, 150));
  }
}

void SpiderLeg::legOff() {
    legs[legId].powerDisabled = true; // Enable the lockout
    // Force PCA9685 channels to full off
    pwm.setPWM(cPin, 4096, 0);
    pwm.setPWM(fPin, 4096, 0);
    pwm.setPWM(tPin, 4096, 0);
}
void SpiderLeg::legOn() {
    legs[legId].powerDisabled = false; // Disable the lockout
    updatePosition();      // Resume normal position calculations and PWM
}

bool SpiderLeg::isMoving() {
  return (abs(tarX - curX) > 2.0 || 
          abs(tarZ - curZ) > 2.0 || 
          abs(tarY - curY) > 2.0);
}

void SpiderLeg::lift(float L)            { setZ(L); }              // raw Z, Y untouched
void SpiderLeg::down()                   { setZ(-5); }
void SpiderLeg::strideF(float strideY)   { setY(strideY); }        // raw Y, Z untouched
void SpiderLeg::home()                 { setTarget(0, 0, 0); }            // Y untouched — matches your doc
void SpiderLeg::crab(float strideX)      { setX(strideX); }