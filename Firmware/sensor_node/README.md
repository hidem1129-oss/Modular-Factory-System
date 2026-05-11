# sensor_node

This directory contains firmware for a sensor input node.

---

## Purpose

The `sensor_node` reads sensor values through the shared I²C node architecture.

It is used to connect physical sensors such as photo-reflectors to the Modular Factory System.

---

## Responsibilities

The sensor node is responsible for:

- reading analog or digital sensor values
- storing measurement results in feedback registers
- reporting status and feedback values
- synchronizing sensor data with host-side orchestration
- integrating sensor-specific behavior with the shared firmware core

---

## Directory Structure

- `src/` : sensor-specific firmware implementation
- `README.md` : overview of the sensor node firmware

### `src/`

This directory contains the actual firmware implementation for the sensor node.

It includes sensor-specific logic such as:

- reading sensor input values
- converting raw input into feedback values
- updating feedback registers
- reporting node status
- exposing sensor data to the host controller through I²C registers

---

## Relationship to common firmware

The sensor node is built on top of the shared firmware core.

```text
sensor_node
    ↓
common/include
    ↓
common/core
    ↓
common/platform
```

The host controller reads sensor feedback values from I²C registers.

The common firmware core handles register access and state transitions, while the sensor node implements the device-specific input behavior.

---

## Example use in the demo

In the physical demo system, the sensor node can be used for:

- workpiece detection
- conveyor timing triggers
- branch conditions in orchestration scripts
- logging sensor values for analysis

---

## Related Documents

- Firmware overview → [../README.md](../README.md)
- Common firmware core → [../common/](../common/)
- Motor node → [../motor_node/](../motor_node/)
- Servo node → [../servo_node/](../servo_node/)
