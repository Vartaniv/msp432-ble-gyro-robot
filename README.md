# msp432-ble-gyro-robot
Bluetooth controlled rover robot using MSP432
# MSP432 BLE Gyroscope Robot Controller

## Overview

This project transforms a TI-RSLK MAX robot powered by the MSP432P401R microcontroller into a Bluetooth-controlled robotic vehicle using the Adafruit Bluefruit LE UART Friend module and the Bluefruit Connect mobile application.

Unlike traditional button-only control, this system allows real-time robot navigation using a smartphone’s accelerometer and gyroscope, enabling intuitive motion-based driving through phone tilt.

---

## Features

* Bluetooth Low Energy (BLE) wireless communication
* UART communication using MSP432 eUSCI_A3
* Smartphone gyroscope / accelerometer control
* Forward, reverse, left, and right movement
* Proportional speed control based on phone tilt angle
* Adjustable dead zone to prevent unintended movement
* Safe motor stop when phone is neutral
* PWM motor speed control
* Modular driver architecture

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

## Software Architecture

### Main Components

* `main.c` → Main control loop and packet decoding
* `BLE_UART.c` → BLE UART communication driver
* `Motor.c` → Motor control using PWM
* `Clock.c` → Timing functions
* `GPIO.c` → Pin configuration

---

## Bluefruit App Usage

### Setup:

1. Install Bluefruit Connect
2. Pair with BLE UART module
3. Open:

   * Controller
   * Accelerometer mode
4. Tilt phone to control robot

---

## Motion Controls

| Phone Position | Robot Action |
| -------------- | ------------ |
| Tilt Forward   | Move Forward |
| Tilt Backward  | Reverse      |
| Tilt Left      | Turn Left    |
| Tilt Right     | Turn Right   |
| Flat           | Stop         |

---

## Speed Control

Motor speed increases proportionally with tilt angle.

### Formula:

```c
speed = MOTOR_DUTY_MIN + (abs(tilt) * TILT_SCALE);
```

---

## Safety Features

* Dead zone filtering
* PWM clamping
* Automatic stop on invalid packets
* Neutral stop mode

---

## Build Instructions

### Clone repository:

```bash
git clone https://github.com/YourUsername/msp432-ble-gyro-robot.git
```

### Open in Code Composer Studio:

* Import CCS project
* Build project
* Flash MSP432

---

## Recommended `.gitignore`

```txt
Debug/
Release/
*.out
.metadata/
```

---

## Future Improvements

* Differential steering
* Steering smoothing
* Turbo mode
* Sensor fusion
* Obstacle avoidance
* PID steering correction

---

## Educational Concepts Demonstrated

* UART protocol
* BLE communication
* Embedded systems programming
* PWM motor control
* Sensor-based robotics
* Real-time control systems

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

This project is intended for educational and academic use.
