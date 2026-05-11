# motor_node

This directory contains firmware for a DC motor control node.

---

## Purpose

The `motor_node` controls DC motors through the shared I²C node architecture.

It is used to drive motion mechanisms such as conveyor belts and motor-driven linear actuators in the Modular Factory System.

---

## Responsibilities

The motor node is responsible for:

- receiving motor control commands via I²C registers
- controlling motor start / stop behavior
- applying speed or setpoint values
- executing timed motor operation
- controlling motor direction, if supported by the hardware configuration
- reporting status and feedback values
- integrating motor-specific behavior with the shared firmware core

---

## Directory Structure

- `src/` : motor-specific firmware implementation
- `README.md` : overview of the motor node firmware

### `src/`

This directory contains the actual firmware implementation for the DC motor node.

It includes motor-specific logic such as:

- reading motor control parameters from I²C registers
- applying speed or setpoint values
- controlling start / stop behavior
- controlling motor direction, if supported by the firmware version
- updating status and feedback values

---

## Relationship to common firmware

The motor node is built on top of the shared firmware core.

```text
motor_node
    ↓
common/include
    ↓
common/core
    ↓
common/platform
```

The host controller writes motor parameters to I²C registers.

The common firmware core handles register access and state transitions, while the motor node implements the device-specific output behavior.

---

## Example use in the demo

In the physical demo system, the motor node can be used for:

- conveyor belt drive
- paper feed or take-up mechanisms
- rack-and-pinion style motion
- other simple timed motion tasks

---

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- Common firmware core → [`../common/`](../common/)
- Servo node → [`../servo_node/`](../servo_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)

