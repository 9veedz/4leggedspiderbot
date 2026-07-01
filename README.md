# ESP32 Quadruped Spider Robot

> Built largely through vibe coding — iterating with an AI assistant rather than writing every line from scratch. Logic has been tested where possible, but review the code yourself before trusting it on real hardware, especially the servo angle limits.

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
- 3D-printed body: [Thingiverse Thing #2204279](https://www.thingiverse.com/thing:2204279), based on the design from [Instructables: DIY Spider Robot / Quad Robot / Quadruped](https://www.instructables.com/DIY-Spider-RobotQuad-robot-Quadruped/)

> **Important — before assembling the legs onto the 3D-printed body:** power each servo individually and set it to **90 degrees** *before* mounting the horn and locking the leg segments onto the printed body. The codebase has calls toward a `spiderCalibrate()` / `calibrateServos()` path, but it is not a complete, working calibration setup as-is — don't rely on it out of the box. Manually centering each servo to 90° before assembly (e.g. with a basic standalone test sketch) is the safer approach until calibration is properly wired up. The IK math and default offsets (`offC`/`offF`/`offT` = 90) assume each joint's mechanical zero/center point lines up with 90° on the servo. If a horn is attached and the leg geometry locked in at the wrong angle, the leg will be physically offset from where the code thinks it is, and you'll fight it with calibration offsets indefinitely instead of starting from a clean baseline.

| Leg | Coxa Pin | Femur Pin | Tibia Pin | Position          | Mirrored |
|-----|----------|-----------|-----------|--------------------|----------|
| 0   | 0        | 1         | 2         | Front-left         | No       |
| 1   | 3        | 4         | 5         | Front-right        | Yes      |
| 2   | 6        | 7         | 8         | Back-left          | Yes      |
| 3   | 9        | 10        | 11        | Back-right         | No       |

### Leg Segment Lengths

Each leg has three segments, defined as constants in `SpiderLeg.cpp` and used directly in the inverse kinematics calculations:

| Segment | Joint  | Length (mm) | Description                                |
|---------|--------|-------------|---------------------------------------------|
| L1      | Coxa   | 27.5        | Hip joint offset, rotates leg in X/Y plane  |
| L2      | Femur  | 55.0        | Upper leg segment                           |
| L3      | Tibia  | 77.5        | Lower leg segment, reaches the ground       |

These are physical measurements of the chassis/leg hardware, used by `calculateIK()` to solve for coxa, femur, and tibia joint angles from a target X/Y/Z foot position. If you build this on different hardware, these three constants are the first thing to update to match your own leg dimensions.

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

## Code Structure Notes

Each leg is its own object with its own pins and position, and handles its own mirroring internally. `Spider` just tells legs where to go. Makes gait code easier to read, and adding more legs later is mostly just adding more leg objects.

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

## Understanding the Inverse Kinematics

If you want to understand how the leg IK math works before diving into `SpiderLeg.cpp`, this video explains it well:

📺 [Inverse Kinematics for a 3-segment leg — YouTube](https://youtu.be/HjmIOKSp7v4?si=qGuz0ecaw_1wg7XF)

## How to Swap in Your Own Control Scheme

The main robot logic lives in `SpiderLeg`, `Spider`, and `Robotstate` — those are the brains. `Spiderserver` and `Webpage` are just how *this* project talks to the robot over WiFi/WebSocket. You can rip those out entirely and replace them with whatever you want — a Bluetooth remote, an RC receiver, a gamepad, a serial interface — as long as you write to the same variables in `Robotstate`.

Everything the robot responds to is exposed as simple global variables you can read or write from anywhere:

**Body / Posture**
| Variable | Type | Description |
|---|---|---|
| `bodyHeight` | `float` | Overall body height offset |
| `bodyWidth` | `float` | Overall body width offset |
| `bodyStride` | `float` | Step length used in walking gaits |
| `bodyPitch` | `float` | Forward/back tilt of the body |
| `bodyRoll` | `float` | Left/right tilt of the body |

**Robot State**
| Variable | Type | Description |
|---|---|---|
| `commandedState` | `RobotStateMode` | What you want the robot to do: `SIT`, `STAND`, `FORWARD`, `REVERSE` |
| `currentState` | `RobotStateMode` | What the robot is currently doing — declared but not yet actively updated separately from `commandedState`, reserved for future state transition logic |
| `activeLeg` | `int` | Which leg (0–3) is selected for tuning |
| `calibrateMode` | `bool` | ⚠️ Declared but currently unused — reserved for a future calibration mode flow |
| `calibrateLeg` | `int` | ⚠️ Declared but currently unused — intended to track which leg is being calibrated independently of `activeLeg` |

**Computed Per-Leg Arrays** (auto-updated by `updateKinematicArrays()`)
| Variable | Type | Description |
|---|---|---|
| `legHeights[4]` | `float` | Effective height per leg after pitch/roll tilt trim |
| `legWidths[4]` | `float` | Effective width per leg |

**Per-Leg Parameters** (`legs[0]` through `legs[3]`)
| Field | Type | Description |
|---|---|---|
| `W` | `float` | Manual width trim for this leg |
| `H` | `float` | Manual height trim for this leg |
| `L` | `float` | Lift height used during gait steps |
| `offC / offF / offT` | `float` | Calibration angle offset per joint (coxa/femur/tibia) |
| `servoC / servoF / servoT` | `int` | Raw manual servo angle (used in calibration mode) |
| `cMin/cMax` | `int` | Coxa joint safe angle limits |
| `fMin/fMax` | `int` | Femur joint safe angle limits |
| `tMin/tMax` | `int` | Tibia joint safe angle limits |
| `X / Y / Z` | `float` | ⚠️ Stored and persisted but not yet actively read back into gait logic — reserved for future per-leg position tracking |
| `powerDisabled` | `bool` | If true, this leg's servos are powered off |



- `Robotstate` holds all shared configuration (body posture, per-leg parameters, current/commanded state) and computes derived kinematic arrays (`legHeights`, `legWidths`) whenever body posture changes, including tilt trim from pitch/roll.
- `SpiderLeg` converts target X/Y/Z leg positions into joint angles via geometric inverse kinematics, then maps those angles to PWM pulses, accounting for whether the leg is mirrored.
- `Spider` orchestrates all four legs together — gait sequencing for walking, synchronized sit/stand/center routines, and calibration mode.
- `Spiderserver` hosts the WiFi AP, captive portal DNS, and WebSocket server, parsing simple text commands (e.g. `bodyHeight=70.0`, `leg=2 offC=85`) from the web UI and broadcasting updated state back to all connected clients.

## Future Notes

Planned / possible extensions, made straightforward by the existing modular structure:

- **More walking patterns** — turning (left/right), crab walk (lateral movement using the existing `crab()` primitive in `SpiderLeg`), and a dance routine are planned. The gait system is structured as a phase/substep state machine in `Spider.cpp`, so new patterns slot in as additional `walkX()` methods without touching the leg kinematics.
- **IMU-based balancing** — add an IMU (e.g. MPU6050) and feed pitch/roll readings into a PID loop that continuously adjusts `bodyPitch`/`bodyRoll` (or per-leg height directly) to keep the body level on uneven terrain. Since `updateKinematicArrays()` already recalculates per-leg height/width trim from `bodyPitch`/`bodyRoll` via `calculateTiltTrim()`, this mostly means replacing manual slider input with a PID controller's output on the same variables — no kinematics rewrite needed.
- **Remote control input** — because all robot control already flows through simple state changes (`commandedState`, `bodyHeight`, `bodyWidth`, etc. in `Robotstate`) rather than being tightly coupled to the WebSocket parser, an RC receiver, Bluetooth gamepad, or other remote input method can be added as just another input source writing to those same variables, alongside or instead of the web UI.

The decoupled design (state in `Robotstate`, kinematics in `SpiderLeg`, orchestration in `Spider`, I/O in `Spiderserver`) means new input/sensor sources generally plug into the *existing* state variables rather than requiring changes to gait logic or IK code.

## License

CC BY-NC-SA 4.0 (Attribution-NonCommercial-ShareAlike) — see [LICENSE](./LICENSE) for details. Free for personal/non-commercial use and modification with credit to `9veedz`; selling this project or derivatives of it is not permitted.
