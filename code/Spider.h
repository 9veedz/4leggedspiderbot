#ifndef SPIDER_H
#define SPIDER_H

#include <Arduino.h>
#include "SpiderLeg.h"
#include "Robotstate.h"

class Spider {
private:
    SpiderLeg leg0; // legId 0 - front-left  (non-mirrored)
    SpiderLeg leg1; // legId 1 - front-right (mirrored)
    SpiderLeg leg2; // legId 2 - back-left   (mirrored)
    SpiderLeg leg3; // legId 3 - back-right  (non-mirrored)
    
    // Internal helper functions
    float tiltTrim(int legId);
    float actualWidth(int legId);
    void applyTiltedCenter(int legId, SpiderLeg &leg, int step);
    
    int gaitPhase = 0;    // 0=leg pair A solo, 1=leg pair B solo, 2=simultaneous push/retract, 3/4=recovery
    int subStep = 0;      // sub-step within the current gaitPhase
    unsigned long lastStepTime = 0;
    unsigned long stepDelay = 150; // ms between gait sub-steps

    int sitPhase = 0;     // 0..2 = home() settle phases, 3 = post-home delay, 4 = done/legs off
    unsigned long lastSitTime = 0;
    unsigned long sitStepDelay = 400; // ms between sit phases

public:
    Spider();

    void begin();
    void walkForward();
    void walkReverse();          // call every loop() tick while walking forward/reverse
    void updateAll();            // calls updatePosition() on all 4 legs
    void stand();
    void sit();                  // call every loop() tick while commandedState == SIT
    void resetSit();             // re-arms sit() to run its centering sequence again
    void spiderOn();
    void spiderOff();
    void spiderCalibrate();
    void center();               // graceful re-center, mirrors old Center() sequence

    // Required public variables accessed directly in Spider.cpp
   
};

#endif