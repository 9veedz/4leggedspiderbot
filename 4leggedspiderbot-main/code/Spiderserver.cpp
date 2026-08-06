#include "Spiderserver.h"
#include "Robotstate.h"
#include <WiFi.h>
#include <DNSServer.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include "Webpage.h"

// Instantiate server on port 80 and websocket at route /ws
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

// DNS Server instance to handle Captive Portal interception
DNSServer dnsServer;
const byte DNS_PORT = 53;

// Serializes and pushes the complete live robot state down the websocket
void sendRobotStateSnapshot(uint32_t clientId) {
  JsonDocument doc;

  // Global Body Configs
  doc["bH"] = bodyHeight;
  doc["bW"] = bodyWidth;
  doc["bS"] = bodyStride;
  doc["bP"] = bodyPitch;
  doc["bR"] = bodyRoll;
  doc["aL"] = activeLeg;
  doc["cSt"] = (int)commandedState;

  // Active Leg Configurations
  JsonObject legObj = doc["leg"].to<JsonObject>();
  legObj["W"]     = legs[activeLeg].W;
  legObj["H"]     = legs[activeLeg].H;
  legObj["L"]     = legs[activeLeg].L;
  legObj["offC"]  = legs[activeLeg].offC;
  legObj["offF"]  = legs[activeLeg].offF;
  legObj["offT"]  = legs[activeLeg].offT;
  legObj["servoC"]= legs[activeLeg].servoC;
  legObj["servoF"]= legs[activeLeg].servoF;
  legObj["servoT"]= legs[activeLeg].servoT;
  legObj["X"]     = legs[activeLeg].X;
  legObj["Y"]     = legs[activeLeg].Y;
  legObj["Z"]     = legs[activeLeg].Z;

  legObj["cMin"]  = legs[activeLeg].cMin;
  legObj["cMax"]  = legs[activeLeg].cMax;
  legObj["fMin"]  = legs[activeLeg].fMin;
  legObj["fMax"]  = legs[activeLeg].fMax;
  legObj["tMin"]  = legs[activeLeg].tMin;
  legObj["tMax"]  = legs[activeLeg].tMax;

  legObj["enabled"] = !legs[activeLeg].powerDisabled;

  String jsonString;
  serializeJson(doc, jsonString);

  // Transmit initialization payload directly to the connecting client
  ws.text(clientId, jsonString);
}

// Intercepts and processes incoming payloads from the Web UI / WebSocket channel
void onWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("[WS] client #%u connected from %s\n", (unsigned int)client->id(), client->remoteIP().toString().c_str());

    // Trigger two-way bidirectional handshake instantly upon WebSocket connection
    sendRobotStateSnapshot(client->id());
  }
  else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("[WS] client #%u disconnected\n", (unsigned int)client->id());
  }
  else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->opcode == WS_TEXT) {
      data[len] = 0;
      String msg = (char*)data;
      msg.trim();

      Serial.printf("[WS] received: %s\n", msg.c_str());

      // Parse commands (e.g., "state=sit", "leg=0", "bodyHeight=70.0", etc.)
      if (msg.startsWith("save")) {
        robotStateSave();
        Serial.println("[State] saved to flash");
      }
      // RESET command: Pulls configurations from NVS and pushes the state back out
      else if (msg.startsWith("reset")) {
        robotStateResetDefaults(); // Overwrites RAM with factory defaults
        Serial.println("[State] reset to factory defaults");

        // Push the refreshed variables back to all connected WebSockets clients
        for (AsyncWebSocketClient &c : ws.getClients()) {
          if (c.status() == WS_CONNECTED) {
            sendRobotStateSnapshot(c.id());
          }
        }
      }
      else if (msg.startsWith("state=")) {
        String stateStr = msg.substring(6);
        stateStr.toLowerCase();

        if (stateStr == "sit") {
          commandedState = SIT;
        } else if (stateStr == "stand") {
          commandedState = STAND;
        } else if (stateStr == "forward") {
          commandedState = FORWARD;
        } else if (stateStr == "reverse") {
          commandedState = REVERSE;
        }
      }
      else if (msg.startsWith("leg=") && msg.indexOf(' ') == -1) {
        activeLeg = msg.substring(4).toInt();
        if (activeLeg < 0) activeLeg = 0;
        if (activeLeg > 3) activeLeg = 3;
        Serial.printf("[WS] leg select -> %d\n", activeLeg);
      }
      // Global values parsing with kinematic updates & broadcast sync
      else if (msg.startsWith("bodyHeight=")) {
        bodyHeight = msg.substring(11).toFloat();
        updateKinematicArrays();
        for (AsyncWebSocketClient &c : ws.getClients()) {
          if (c.status() == WS_CONNECTED) sendRobotStateSnapshot(c.id());
        }
        Serial.printf("[WS] bodyHeight=%.2f\n", bodyHeight);
      }
      else if (msg.startsWith("bodyWidth=")) {
        bodyWidth = msg.substring(10).toFloat();
        updateKinematicArrays();
        for (AsyncWebSocketClient &c : ws.getClients()) {
          if (c.status() == WS_CONNECTED) sendRobotStateSnapshot(c.id());
        }
        Serial.printf("[WS] bodyWidth=%.2f\n", bodyWidth);
      }
      else if (msg.startsWith("bodyStride=")) {
        bodyStride = msg.substring(11).toFloat();
        Serial.printf("[WS] bodyStride=%.2f\n", bodyStride);
      }
      else if (msg.startsWith("bodyPitch=")) {
        bodyPitch = msg.substring(10).toFloat();
        updateKinematicArrays();
        for (AsyncWebSocketClient &c : ws.getClients()) {
          if (c.status() == WS_CONNECTED) sendRobotStateSnapshot(c.id());
        }
        Serial.printf("[WS] bodyPitch=%.2f\n", bodyPitch);
      }
      else if (msg.startsWith("bodyRoll=")) {
        bodyRoll = msg.substring(9).toFloat();
        updateKinematicArrays();
        for (AsyncWebSocketClient &c : ws.getClients()) {
          if (c.status() == WS_CONNECTED) sendRobotStateSnapshot(c.id());
        }
        Serial.printf("[WS] bodyRoll=%.2f\n", bodyRoll);
      }
      // All-legs broadcast (e.g. "leg=all enabled=0")
      else if (msg.startsWith("leg=all")) {
        int spaceIdx = msg.indexOf(' ');
        if (spaceIdx != -1) {
          String paramPair = msg.substring(spaceIdx + 1);
          int eqIdx = paramPair.indexOf('=');
          if (eqIdx != -1) {
            String pKey = paramPair.substring(0, eqIdx);
            float pVal = paramPair.substring(eqIdx + 1).toFloat();

            if (pKey == "enabled") {
              bool isEnabled = (pVal != 0);
              for (int i = 0; i < 4; i++) {
                legs[i].powerDisabled = !isEnabled;
              }
              Serial.printf("[WS] all legs enabled=%d\n", isEnabled);
            }
          }
        }
      }
      // Per-leg parameters mapping (e.g. "leg=0 W=70")
      else if (msg.startsWith("leg=")) {
        int legIdx = msg.substring(4, 5).toInt();
        int spaceIdx = msg.indexOf(' ');

        if (legIdx >= 0 && legIdx <= 3 && spaceIdx != -1) {
          String paramPair = msg.substring(spaceIdx + 1);
          int eqIdx = paramPair.indexOf('=');

          if (eqIdx != -1) {
            String pKey = paramPair.substring(0, eqIdx);
            float pVal = paramPair.substring(eqIdx + 1).toFloat();

            LegParams &l = legs[legIdx];

            if (pKey == "W") l.W = pVal;
            else if (pKey == "H") l.H = pVal;
            else if (pKey == "L") l.L = pVal;
            else if (pKey == "offC") l.offC = pVal;
            else if (pKey == "offF") l.offF = pVal;
            else if (pKey == "offT") l.offT = pVal;
            else if (pKey == "servoC") l.servoC = (int)pVal;
            else if (pKey == "servoF") l.servoF = (int)pVal;
            else if (pKey == "servoT") l.servoT = (int)pVal;
            else if (pKey == "cMin") l.cMin = (int)pVal;
            else if (pKey == "cMax") l.cMax = (int)pVal;
            else if (pKey == "fMin") l.fMin = (int)pVal;
            else if (pKey == "fMax") l.fMax = (int)pVal;
            else if (pKey == "tMin") l.tMin = (int)pVal;
            else if (pKey == "tMax") l.tMax = (int)pVal;
            else if (pKey == "X") l.X = pVal;
            else if (pKey == "Y") l.Y = pVal;
            else if (pKey == "Z") l.Z = pVal;
            else if (pKey == "enabled") l.powerDisabled = (pVal == 0);
          }
        }
      }
    }
  }
}

void spiderServerBegin() {
  // Setup AP configuration
  WiFi.softAP("ESP32_Spider", "12345678");
  IPAddress apIP = WiFi.softAPIP(); // default: 192.168.4.1
  Serial.print("AP started. IP: ");
  Serial.println(apIP);

  // Intercept all DNS queries and point them to the ESP32 IP
  dnsServer.start(DNS_PORT, "*", apIP);

  // Attach web root file
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // Catch-all route to redirect any unassigned requests back to index.html (Captive Portal fallback)
  server.onNotFound([](AsyncWebServerRequest *request){
    request->send(200, "text/html", index_html);
  });

  // Attach WebSocket handler
  ws.onEvent(onWebSocketEvent);
  server.addHandler(&ws);

  server.begin();
  Serial.println("Server started.");
}

void spiderServerLoop() {
  dnsServer.processNextRequest(); // Continuously handle background DNS requests
  ws.cleanupClients();            // Cleans up stale/closed websocket sessions
}