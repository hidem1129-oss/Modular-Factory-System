# Firmware

This directory contains firmware for distributed Raspberry Pi Pico nodes used in the Modular Factory System.

The firmware is designed around a shared node architecture: each physical device is exposed as an I²C-controlled node with a register-based interface and state-machine-driven execution.

---

## Overview

Each firmware node:

- runs on a Raspberry Pi Pico
- communicates with the host controller via I²C
- exposes control and feedback values through registers
- follows a shared state model
- implements device-specific behavior through node-specific callbacks

---

## Node Types

| Directory | Role | Example use in the demo |
|---|---|---|
| `common/` | Shared firmware core | Register handling, state control, platform abstraction |
| `servo_node/` | Servo actuator node | Sorting gate / mechanical arm control |
| `motor_node/` | DC motor node | Conveyor drive / linear actuator drive |
| `sensor_node/` | Sensor input node | Photo-reflector workpiece detection |

---

## Structure

The firmware is divided into two main parts.

### `common/`

Shared components used across all nodes:

- core control logic
- I²C communication handling
- register-based protocol
- state machine framework
- platform abstraction

### Node-specific directories

Each node directory contains:

- device-specific control logic
- configuration
- quick start guide or usage notes
- design documentation
- test procedures, where available

Examples:

- `servo_node/` → actuator control
- `motor_node/` → drive control
- `sensor_node/` → measurement / detection

---

## Build Concept

Each firmware is built by combining:

- shared components from `common/`
- node-specific implementation from each node directory

In other words:

> Node directories are not standalone systems.  
> They extend the shared firmware core with device-specific behavior.

This structure enables:

- reuse of core functionality
- easy addition of new node types
- consistent behavior across the system

---

## Design Philosophy

The firmware abstracts physical devices into standardized nodes.

This allows:

- hardware modules to be interchangeable
- host-side software to control different devices through a unified register interface
- state transitions and errors to be handled consistently across node types

This architecture is similar to microservices in software systems, but applied to physical devices.

---

## Learn More

- How nodes work → [`../How_It_Works/`](../How_It_Works/)
- Common firmware core → [`common/`](./common/)
- Servo node → [`servo_node/`](./servo_node/)
- Motor node → [`motor_node/`](./motor_node/)
- Sensor node → [`sensor_node/`](./sensor_node/)
