# common

This directory contains shared firmware components used across all Raspberry Pi Pico nodes.

---

## Purpose

The `common` layer provides the reusable foundation of the firmware architecture.

It defines how all nodes behave in a consistent and unified way, regardless of whether the node controls a motor, a servo, or a sensor.

---

## Responsibilities

The `common` layer is responsible for:

- shared node behavior
- register-based communication handling
- state handling and control flow
- public APIs for node-specific implementations
- platform and build configuration
- common design documentation

---

## Directory Structure

- `core/` : shared control logic and state handling
- `include/` : public headers used by node-specific implementations
- `platform/` : build/runtime configuration and Pico-specific entry points
- `docs/` : design notes and common firmware documentation

---

## How it fits into the system

Node-specific firmware directories such as `servo_node/`, `motor_node/`, and `sensor_node/` extend this common layer.

```text
node-specific implementation
        ↓
common/include
        ↓
common/core
        ↓
common/platform
```

This structure allows each node to share the same communication model and execution behavior while keeping device-specific logic separate.

## Design Concept

The common layer abstracts each physical device as a standardized firmware node.

This enables:

- consistent I²C behavior across nodes
- reusable register handling
- unified state transitions
- easier addition of new node types
- reduced duplication between firmware implementations

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- Servo node → [`../servo_node/`](../servo_node/)
- Motor node → [`../motor_node/`](../motor_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)
