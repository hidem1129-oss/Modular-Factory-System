# servo_node

This directory contains firmware for a servo actuator node.

---

## Purpose

The `servo_node` controls servo motors through the shared I²C node architecture.

It is used to drive small actuator mechanisms such as sorting gates, diverter arms, and paper clamp mechanisms in the Modular Factory System.

---

## Responsibilities

The servo node is responsible for:

- receiving servo control commands via I²C registers
- applying target position or setpoint values
- executing timed or position-based servo movement
- generating PWM output for servo control
- reporting status and feedback values
- integrating servo-specific behavior with the shared firmware core

---

## Directory Structure

- `src/` : servo-specific firmware implementation
- `README.md` : overview of the servo node firmware

### `src/`

This directory contains the actual firmware implementation for the servo node.

It includes servo-specific logic such as:

- reading servo control parameters from I²C registers
- applying target position or movement parameters
- executing servo movement
- generating PWM output through the hardware abstraction layer
- updating status and feedback values

---

## Relationship to common firmware

The servo node is built on top of the shared firmware core.

```text
servo_node
    ↓
common/include
    ↓
common/core
    ↓
common/platform
```
The host controller writes servo parameters to I²C registers.

The common firmware core handles register access and state transitions, while the servo node implements the device-specific servo output behavior.

---

## Example use in the demo

In the physical demo system, the servo node can be used for:

- sorting gates
- diverter arms
- paper clamp mechanisms
- small mechanical actuation tasks

---

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- Common firmware core → [`../common/`](../common/)
- Motor node → [`../motor_node/`](../motor_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)
