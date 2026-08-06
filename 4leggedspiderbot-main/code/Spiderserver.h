#ifndef SPIDERSERVER_H
#define SPIDERSERVER_H

#include <Arduino.h>
#include "Robotstate.h" // Ensures RobotStateMode enums are recognized globally

// Starts the Wi-Fi AP, Captive Portal DNS, and Async WebServer/WebSockets
void spiderServerBegin();
void spiderServerLoop();
void sendRobotStateSnapshot(uint32_t clientId);
#endif