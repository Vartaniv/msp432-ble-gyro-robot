# MSP432 BLE Quaternion Tilt/Gesture Controlled Robot

## Overview

This project transforms a TI-RSLK MAX robot powered by the MSP432P401R microcontroller into a Bluetooth-controlled robotic platform using the Adafruit Bluefruit LE UART Friend module and the Bluefruit Connect mobile application.

The robot is controlled entirely through smartphone orientation using quaternion motion data transmitted wirelessly over BLE UART. By tilting or rotating the phone, the user can drive the robot in multiple directions with optional proportional speed control.

This system replaces traditional button-based driving with a more advanced motion-controlled interface.

---

## Features

* Bluetooth Low Energy (BLE) wireless communication
* UART communication using MSP432 eUSCI_A3
* Quaternion-based phone orientation control
* Vertical (portrait) motion control
* 8-directional robot movement:

  * Forward
  * Backward
  * Left
  * Right
  * Forward-left
  * Forward-right
  * Backward-left
  * Backward-right
* Optional proportional speed control based on tilt magnitude
* PWM motor control
* RGB LED directional feedback
* Neutral-position automatic stop
* Dead-zone filtering for stability
* Real-time motion packet decoding

---

## Hardware Requirements

### Required Components

* MSP432P401R LaunchPad
* TI-RSLK MAX chassis
* Adafruit Bluefruit LE UART Friend
* DC motors
* Breadboard / jumper wires
* 6 AA batteries
* Smartphone with Bluefruit Connect App (iOS or Android)

---

## Wiring Connections

| BLE Module Pin | MSP432 Pin     |
| -------------- | -------------- |
| MOD            | P1.6           |
| CTS            | GND            |
| TXO            | P9.6 (UCA3RXD) |
| RXI            | P9.7 (UCA3TXD) |
| VIN            | 3.3V           |
| GND            | GND            |

---

## Bluefruit App Configuration

### Setup:

1. Install **Bluefruit Connect**
2. Connect to BLE UART module
3. Open:

   * Controller
   * Quaternion / Orientation Mode
4. Tilt or rotate phone to control robot

---

## Motion Controls

| Phone Orientation | Robot Action        |
| ----------------- | ------------------- |
| Tilt Forward      | Move Forward        |
| Tilt Backward     | Reverse             |
| Tilt Left         | Rotate Left         |
| Tilt Right        | Rotate Right        |
| Forward + Right   | Forward-right curve |
| Forward + Left    | Forward-left curve  |
| Backward + Right  | Reverse-right curve |
| Backward + Left   | Reverse-left curve  |
| Neutral / Flat    | Stop                |

---

## Control Modes

### Fixed Speed Mode

Robot moves at preset PWM values.

### Proportional Speed Mode

Motor speed scales dynamically based on phone tilt angle.

### Formula:

```c id="j55jrx"
PWM = abs(quaternion_value) * scaling_factor
```

More tilt = more speed.

---

## Software Architecture

### Main Components

* `main.c` → Quaternion packet processing and control logic
* `BLE_UART.c` → BLE UART communication driver
* `Motor.c` → PWM motor driver
* `Clock.c` → Delay and timing functions
* `GPIO.c` → Hardware initialization

---

## Packet Format

### Bluefruit Quaternion Packet:

```txt id="stl1sr"
!Q qx qy qz qw checksum
```

### Packet Contents:

* `qx` → Forward/backward orientation
* `qy` → Left/right orientation
* `qz` → Rotation axis
* `qw` → Quaternion magnitude

---

## Directional Logic

The software analyzes:

* `qx` for vertical tilt
* `qy` for horizontal tilt

Motor outputs are determined by threshold ranges and combined tilt states.

---

## RGB LED Indicators

| Color  | Meaning           |
| ------ | ----------------- |
| Green  | Forward           |
| Blue   | Backward          |
| Yellow | Left              |
| Pink   | Right             |
| White  | Diagonal movement |
| Red    | Stop              |

---

## Safety Features

* Automatic stop on invalid BLE packets
* Dead zone threshold
* PWM duty cycle clamping
* Neutral position stop
* Controlled delay for stability

---

## Build Instructions

### Clone repository:

```bash id="5yj7l8"
git clone https://github.com/YourUsername/msp432-ble-quaternion-robot.git
```

### Open in Code Composer Studio:

* Import CCS project
* Build project
* Flash MSP432

---

## Recommended `.gitignore`

```txt id="zqvv6l"
Debug/
Release/
*.out
.metadata/
```

---

## Educational Concepts Demonstrated

* Embedded systems programming
* UART communication
* Bluetooth Low Energy
* Quaternion mathematics
* PWM motor control
* Real-time robotics
* Gesture-based interfaces
* Sensor fusion concepts

---

## Future Improvements

* Horizontal/landscape control mode
* Obstacle avoidance
* PID steering stabilization
* Adjustable sensitivity via app
* Gesture recognition
* Autonomous assist mode
* Live telemetry feedback

---

## Author

**Vedi Vartani**

ECE 528/L Robotics and Embedded Systems
California State University, Northridge

---

## Acknowledgments

* Aaron Nanas
* Texas Instruments
* Adafruit
* Bluefruit Connect

---

## License

This project is intended for educational, academic, and demonstration purposes.
