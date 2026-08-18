# 4-Legged Spiderbot — PCB

So this is the PCB folder for my spiderbot project a 4-legged walking robot I've been building in my spare time. It's been a fun (and sometimes painful) journey getting here.



---

## PCB Designs

I went through 2 design revisions and ended up with 3 boards total. Here's how it went:

---

### Design 1 Single MCU Board

My first attempt everything on one board. Kept it simple.

| Component | Role |
|---|---|
| ESP32-S3 Supermini | Main MCU — hosts a WiFi HTML control page |
| PCA9685 | 16-channel PWM driver — drives all 12 servos |
| Buck step-down converter | Steps down input voltage to power the logic and servos |
| USB-C PD trigger board | Triggers USB-PD to get the right voltage into the buck |
| MPU-6050 | 6-axis IMU for orientation and tilt sensing |
| Output filter capacitors | Clean up the buck output |
| Header pins | Connections and module mounting |

Also broke out multiple I2C ports on separate pins makes it easy to chain extra sensors or modules without fighting over the same lines.

The robot gets controlled through a webpage the ESP32-S3 hosts over WiFi. the phone does all the heavy lifting (IK, gait logic) and sends commands down to the PCA9685 which then drives the servos.

**Front**

![Design 1 MCU Board Front](screenshots/1f.png)

**Back**

![Design 1 MCU Board Back](screenshots/1b.png)

---

### Design 2 MCU Board + Voltage Divider Stackup (2 Boards)

After getting Design 1 working I wanted to add proper servo position feedback so I split it into two stacked boards. Also upgraded the power side since the servos were pulling more current than I expected.

**Board 1 MCU Board**

Pretty much the same as Design 1 but with a beefier buck:

| Component | Role |
|---|---|
| ESP32-S3 Supermini | Main MCU WiFi control page |
| PCA9685 | 16-channel PWM driver all 12 servos |
| 5V 8A buck converter | Much beefier this time takes battery input directly, 5V @ 8A for the servos |
| MPU-6050 | 6-axis IMU |
| Header pins | Interface to the voltage divider board stacked on top |



Also in this revision I mapped out all the GPIOs to header pins so if I want to hook up extra peripherals, sensors, or expand the project later, the pins are already there and accessible without any rework.

**Board 2 Voltage Divider Board** (stacks on top of Board 1)

This little board stacks right below the mcu and reads servo positions by tapping into each servo's internal pot:

| Component | Role |
|---|---|
| Resistor voltage dividers | One per servo scales the pot signal down to a safe ADC input range |
| Resistor voltage divider | Battery level monitoring |
| Header pins | Connects down to Board 1 and feeds signals into the ESP32-S3 ADC pins |

The idea is to tap into the servo's internal position potentiometer and use that as feedback for closed-loop position control. Pretty cool once it works!

**Front**

![Design 2 MCU Board + Voltage Divider Stackup Front](screenshots/2f.png)

**Back**

![Design 2 MCU Board + Voltage Divider Stackup Back](screenshots/2b.png)

---


## Repo

Full project: [github.com/9veedz/4leggedspiderbot](https://github.com/9veedz/4leggedspiderbot)

