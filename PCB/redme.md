# 4-Legged Spiderbot — PCB

This folder contains the PCB design files for the 4-Legged Spiderbot project, a custom quadruped robot built around an ESP32 microcontroller and PCA9685 PWM driver.

---

## PCB Designs

This project went through two design revisions, producing a total of 3 boards.

---

### Design 1 — Single MCU Board

A single all-in-one board acting as the central hub for the spiderbot's electronics:

| Component | Role |
|---|---|
| ESP32-S3 Supermini | Main MCU — BLE comms, serial-to-PWM translation |
| PCA9685 | 16-channel PWM driver — controls all 12 servo channels |
| Buck step-down converter | Steps down input voltage to logic/servo supply rails |
| USB-C PD trigger board | Negotiates USB-PD supply voltage for the buck input |
| MPU-6050 | 6-axis IMU — accelerometer + gyroscope for orientation sensing |
| Output filter capacitors | Smooth and regulate the buck converter output |
| Header pins | Inter-board connections and module mounting |

The robot's inverse kinematics and gait computation run entirely on the phone — the ESP32-S3 receives serial commands over BLE and drives the PCA9685 to output the corresponding PWM signals to each servo.

![Design 1 — MCU Board](screenshots/design1.png)

---

### Design 2 — MCU Board + Voltage Divider Stackup (2 Boards)

The second revision splits functionality across two stacked boards and upgrades the power supply to handle the full servo load directly from battery.

**Board 1 — MCU Board**

Same core as Design 1, with a key power upgrade:

| Component | Role |
|---|---|
| ESP32-S3 Supermini | Main MCU — BLE comms, serial-to-PWM translation |
| PCA9685 | 16-channel PWM driver — controls all 12 servo channels |
| 5V 8A buck converter | High-current step-down — regulates battery voltage directly to 5V @ 8A for servos |
| MPU-6050 | 6-axis IMU — accelerometer + gyroscope for orientation sensing |
| Header pins | Stackup interface to voltage divider board |

> The USB-C PD trigger board is dropped in Design 2 — the beefier buck takes battery input directly, removing the dependency on USB-PD negotiation for power.

**Board 2 — Voltage Divider Board** (stacks on top of Board 1)

| Component | Role |
|---|---|
| Resistor voltage dividers | One per servo — scales position feedback signal to ADC-safe range |
| Resistor voltage divider | batery level monitoring |
| Header pins | Mates with Board 1; routes feedback signals to ESP32-S3 ADC inputs |

This stackup allows us to tap into the servos internal postiion pot and use it as ref for servo position.

![Design 2 — MCU Board + Voltage Divider Stackup](screenshots/design2.png)

---

## Sponsor — PCBWay

This project's PCBs were manufactured and sponsored by **[PCBWay](https://www.pcbway.com)**.

### About PCBWay

PCBWay is a leading PCB manufacturer and assembly service based in Shenzhen, China. They offer:

- PCB fabrication (1–20+ layers, rigid, flex, rigid-flex)
- PCB assembly (SMT + through-hole, full turnkey)
- 3D printing (FDM, SLA, SLS, metal)
- CNC machining and sheet metal fabrication
- One-stop prototyping and low-to-mid volume production

Their instant online quoting, fast turnaround (as quick as 24 hours for standard PCBs), and competitive pricing make them a go-to for hardware prototypers and makers.

---

### Why PCBWay for This Project

The spiderbot PCB has tight requirements — compact form factor, multiple power domains, and a dense connector layout for 12 servo channels. PCBWay's manufacturing tolerances and surface finish quality (HASL / ENIG options) made them the right choice for getting a clean, reliable board on the first spin.

**Order process:**
1. Exported Gerbers, drill files, and BOM from KiCad
2. Uploaded to PCBWay's online portal — instant DFM check and quote
3. Selected specs (2-layer, 1.6mm FR4, HASL, green solder mask)
4. Order confirmed and dispatched within 24 hours

**Unboxing:**
- PCBs arrived well-packaged in anti-static bags inside a rigid mailer
- Panel of boards with clean V-score lines for easy depanelization
- Surface finish was uniform, soldermask crisp, silkscreen readable

**Dimensions & Precision:**
- Board outline within ±0.1mm of design
- Via diameters and trace widths matched Gerber files exactly
- No solder bridge issues on fine-pitch pads

**End Application:**
The PCB is the brain of a 4-legged walking robot controlled wirelessly via a custom Android app and Galaxy Watch. It's used for gait experimentation, inverse kinematics validation, and general quadruped locomotion research.

**PCBWay advantages for this project:**
- Fast prototyping turnaround
- Consistent quality across board runs
- Wide range of services under one roof (PCB + 3D printing for enclosures)
- Responsive support team

> If you're building a robotics or embedded systems project, PCBWay is worth checking out: [pcbway.com](https://www.pcbway.com)

---

## Repository

Full project repo: [github.com/9veedz/4leggedspiderbot](https://github.com/9veedz/4leggedspiderbot)

PCB files: [PCB folder](https://github.com/9veedz/4leggedspiderbot/tree/main/PCB)

