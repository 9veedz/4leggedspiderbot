#include "Spider.h"
#include "Robotstate.h"

Spider::Spider()
    : leg0(0, 1, 2, 0),
      leg1(3, 4, 5, 1),
      leg2(6, 7, 8, 2),
      leg3(9, 10, 11, 3) {}

void Spider::begin() {
  updateAll();
}



void Spider::spiderOn() {
  resetSit();
  leg0.legOn();
  leg1.legOn();
  leg2.legOn();
  leg3.legOn();
}

void Spider::spiderOff() {
  leg0.legOff();
  leg1.legOff();
  leg2.legOff();
  leg3.legOff();
}

void Spider::spiderCalibrate() {
  // Force all 4 legs to a neutral 90-degree pose, then let
  // calibrateServos() handle clamping + the actual PWM write per leg.
 

  leg0.calibrateServos();
  leg1.calibrateServos();
  leg2.calibrateServos();
  leg3.calibrateServos();
}

void Spider::stand() {
  spiderOn();

  // Re-evaluate the dynamic kinematic arrays globally using updated body dimensions & pitch/roll
  updateKinematicArrays();

  // Dispatch targets now that legHeights and legWidths arrays contain the full calculated values
  leg0.setTarget(0, 0, 0);
  leg1.setTarget(0, 0, 0);
  leg2.setTarget(0, 0, 0);
  leg3.setTarget(0, 0, 0);
}

void Spider::sit() {
  if (sitPhase >= 4) return; // already done, legs off - call resetSit() to re-arm

  if (millis() - lastSitTime <= sitStepDelay) return;

  if (sitPhase <= 2) {
    leg0.home();
    leg1.home();
    leg2.home();
    leg3.home();

    sitPhase++;
    lastSitTime = millis();
  }
  else if (sitPhase == 3) {
    // sitStepDelay has already elapsed since the last home() call above —
    // legs have had one full sitStepDelay to settle before power-off.
    spiderOff();
    sitPhase++;
    lastSitTime = millis();
  }
}

void Spider::resetSit() {
  sitPhase = 0;
  lastSitTime = 0;
}





void Spider::updateAll() {
  leg0.updatePosition();
  leg1.updatePosition();
  leg2.updatePosition();
  leg3.updatePosition();
}

void Spider::center() {
  // Tilt-aware re-centering - unlike sit(), this honors bodyHeight,
  // bodyWidth, bodyPitch, and bodyRoll via kinematic arrays.
  static int centerSubStep = 0;
  static unsigned long lastCenterTime = 0;

  if (millis() - lastCenterTime > stepDelay) {
    if (centerSubStep <= 2) {
      updateKinematicArrays();

      leg0.home();
      leg1.home();
      leg2.home();
      leg3.home();

      centerSubStep++;
      lastCenterTime = millis();
    }
  }
}

void Spider::walkForward() {
  if (millis() - lastStepTime <= stepDelay) return;

  if (gaitPhase == 0) {
    // Leg0 solo: home -> lift -> strideF -> down
    switch (subStep) {
      case 0: leg0.home(); break;
      case 1: leg0.lift(legs[0].L); break;
      case 2: leg0.strideF(bodyStride); break;
      case 3: leg0.down(); break;
    }
    subStep++;
    if (subStep > 3) { subStep = 0; gaitPhase = 1; }
  }
  else if (gaitPhase == 1) {
    // Leg1 solo: home -> lift -> strideF -> down
    switch (subStep) {
      case 0: leg1.home(); break;
      case 1: leg1.lift(legs[1].L); break;
      case 2: leg1.strideF(bodyStride); break;
      case 3: leg1.down(); break;
    }
    subStep++;
    if (subStep > 3) { subStep = 0; gaitPhase = 2; }
  }
  else if (gaitPhase == 2) {
    // leg2/leg3 run home->down->strideF (push); leg0/leg1's home() (retract)
    // fires on the same sub-step as leg2/3's strideF (the push).
    switch (subStep) {
      case 0:
        leg2.home();
        leg3.home();
        break;
      case 1:
        leg2.down();
        leg3.down();
        break;
      case 2:
        leg2.strideF(bodyStride);
        leg3.strideF(bodyStride);
        leg0.home();
        leg1.home();
        break;
    }
    subStep++;
    if (subStep > 2) { subStep = 0; gaitPhase = 3; }
  }
  else if (gaitPhase == 3) {
    // Leg2 recovery: lift -> home
    switch (subStep) {
      case 0: leg2.lift(legs[2].L); break;
      case 1: leg2.home(); break;
    }
    subStep++;
    if (subStep > 1) { subStep = 0; gaitPhase = 4; }
  }
  else if (gaitPhase == 4) {
    // Leg3 recovery: lift -> home
    switch (subStep) {
      case 0: leg3.lift(legs[3].L); break;
      case 1: leg3.home(); break;
    }
    subStep++;
    if (subStep > 1) { subStep = 0; gaitPhase = 0; } // loop back to start
  }

  lastStepTime = millis();
}

void Spider::walkReverse() {
  if (millis() - lastStepTime <= stepDelay) return;

  if (gaitPhase == 0) {
    // Leg2 solo: home -> lift -> strideF -> down
    switch (subStep) {
      case 0: leg2.home(); break;
      case 1: leg2.lift(legs[2].L); break;
      case 2: leg2.strideF(bodyStride); break;
      case 3: leg2.down(); break;
    }
    subStep++;
    if (subStep > 3) { subStep = 0; gaitPhase = 1; }
  }
  else if (gaitPhase == 1) {
    // Leg3 solo: home -> lift -> strideF -> down
    switch (subStep) {
      case 0: leg3.home(); break;
      case 1: leg3.lift(legs[3].L); break;
      case 2: leg3.strideF(bodyStride); break;
      case 3: leg3.down(); break;
    }
    subStep++;
    if (subStep > 3) { subStep = 0; gaitPhase = 2; }
  }
  else if (gaitPhase == 2) {
    // leg0/leg1 run home->down->strideF (push); leg2/leg3's home() (retract)
    // fires on the same sub-step as leg0/1's strideF (the push).
    switch (subStep) {
      case 0:
        leg0.home();
        leg1.home();
        break;
      case 1:
        leg0.down();
        leg1.down();
        break;
      case 2:
        leg0.strideF(bodyStride);
        leg1.strideF(bodyStride);
        leg2.home();
        leg3.home();
        break;
    }
    subStep++;
    if (subStep > 2) { subStep = 0; gaitPhase = 3; }
  }
  else if (gaitPhase == 3) {
    // Leg0 recovery: lift -> home
    switch (subStep) {
      case 0: leg0.lift(legs[0].L); break;
      case 1: leg0.home(); break;
    }
    subStep++;
    if (subStep > 1) { subStep = 0; gaitPhase = 4; }
  }
  else if (gaitPhase == 4) {
    // Leg1 recovery: lift -> home
    switch (subStep) {
      case 0: leg1.lift(legs[1].L); break;
      case 1: leg1.home(); break;
    }
    subStep++;
    if (subStep > 1) { subStep = 0; gaitPhase = 0; } // loop back to start
  }

  lastStepTime = millis();
}