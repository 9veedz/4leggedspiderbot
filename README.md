# ESP32 Quadruped Spider Robot

A 4-legged (12-servo) walking robot controlled by an ESP32, with a built-in WiFi access point and web-based control interface for live tuning, calibration, and gait control — no app required, just a browser.

## Features

- **Inverse kinematics leg control** — each leg independently computes coxa/femur/tibia servo angles from X/Y/Z target positions.
- **Live web UI** — connects directly to the ESP32's own WiFi access point and serves a mobile-friendly control panel over WebSocket (`/ws`) with a captive portal so it opens automatically on connect.
- **Body posture control** — adjustable body height, width, stride length, pitch, and roll, with automatic tilt trim calculation per leg.
- **Walking gaits** — forward and reverse walking gaits using a diagonal-pair stepping pattern (lift → stride → push → recover).
- **Per-leg calibration** — manual servo angle control, configurable min/max safety limits, and offset trims per joint (coxa/femur/tibia), saved persistently to flash (ESP32 NVS/Preferences).
- **Per-leg power control** — individually or globally power legs on/off (releases servo PWM signal) for safe handling or storage.
- **Robot state machine** — `SIT`, `STAND`, `FORWARD`, `REVERSE` states drive automatic transitions between resting and walking behavior.

## Hardware

- ESP32 development board
- PCA9685 16-channel PWM/servo driver (I2C)
- 12x servos (3 per leg: coxa, femur, tibia)
- 4-leg chassis, with legs 1 and 2 mirrored relative to legs 0 and 3

| Leg | Coxa Pin | Femur Pin | Tibia Pin | Position          | Mirrored |
|-----|----------|-----------|-----------|--------------------|----------|
| 0   | 0        | 1         | 2         | Front-left         | No       |
| 1   | 3        | 4         | 5         | Front-right        | Yes      |
| 2   | 6        | 7         | 8         | Back-left          | Yes      |
| 3   | 9        | 10        | 11        | Back-right         | No       |

## Project Structure

This is an Arduino IDE sketch. Arduino IDE requires a `.ino` file matching the sketch folder name, but all real logic lives in standard `.cpp`/`.h` files, which the IDE compiles automatically alongside it.

```
SpiderRobot/
├── SpiderRobot.ino     # Entry point: setup() / loop()
├── Spider.h / .cpp     # High-level robot behavior: stand, sit, walk, calibrate
├── SpiderLeg.h / .cpp  # Per-leg inverse kinematics & servo output
├── Robotstate.h / .cpp # Shared global state, persistence (flash/NVS)
├── Spiderserver.h / .cpp # WiFi AP, captive portal, WebSocket server
└── Webpage.h           # Embedded HTML/CSS/JS control panel (served from flash)
```

## Getting Started

1. Open `SpiderRobot.ino` in Arduino IDE.
2. Install required libraries via Library Manager:
   - `Adafruit PWM Servo Driver Library`
   - `ESPAsyncWebServer`
   - `AsyncTCP`
   - `ArduinoJson`
   - `DNSServer` (bundled with ESP32 core)
3. Select your ESP32 board under **Tools > Board**.
4. Wire the PCA9685 to the ESP32 via I2C and connect all 12 servos.
5. Flash the sketch.
6. On boot, the ESP32 creates a WiFi access point:
   - **SSID:** `ESP32_Spider`
   - **Password:** `12345678`
7. Connect to that network from a phone or laptop. A captive portal should prompt you to open the control page automatically (or browse to `192.168.4.1` manually).

> **Note:** The AP credentials are currently hardcoded in `Spiderserver.cpp`. Change them before deploying anywhere you don't fully control.

## Using the Web Interface

- **Robot Motion State** — switch between Sit / Stand / Forward / Reverse.
- **Body / Global** — tune body height, width, stride, pitch, and roll in real time.
- **Leg Select & Power** — choose which leg's parameters you're editing, and power individual or all legs on/off.
- **Geometry** — per-leg W/H/L trim offsets.
- **Servo Offsets** — per-joint calibration offsets (coxa/femur/tibia).
- **Servo Angle Control** — manually drive a joint to a specific angle (calibration mode).
- **Servo Min/Max** — set safe angle limits per joint to protect hardware.
- **Save to Flash** — persists all current settings to the ESP32's non-volatile storage.
- **Reload/Reset Defaults** — restores factory default values.

All changes apply live over WebSocket; no page reloads are needed.

## How It Works

- `Robotstate` holds all shared configuration (body posture, per-leg parameters, current/commanded state) and computes derived kinematic arrays (`legHeights`, `legWidths`) whenever body posture changes, including tilt trim from pitch/roll.
- `SpiderLeg` converts target X/Y/Z leg positions into joint angles via geometric inverse kinematics, then maps those angles to PWM pulses, accounting for whether the leg is mirrored.
- `Spider` orchestrates all four legs together — gait sequencing for walking, synchronized sit/stand/center routines, and calibration mode.
- `Spiderserver` hosts the WiFi AP, captive portal DNS, and WebSocket server, parsing simple text commands (e.g. `bodyHeight=70.0`, `leg=2 offC=85`) from the web UI and broadcasting updated state back to all connected clients.

## License

Add your preferred license here (e.g. MIT) before publishing.
