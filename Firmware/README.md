# Firmware

This directory contains firmware for distributed Raspberry Pi Pico nodes  
used in the Modular Factory System.

---

## Overview

The firmware is designed based on a **modular node architecture**, where:

- Each node runs on a Raspberry Pi Pico
- Nodes communicate via I²C
- Each node controls a specific function (actuator, sensor, etc.)

---

## How it works

👉 [How It Works](../How It Works/)

---

## Structure

The firmware is divided into two main parts:

### common/

Shared components used across all nodes:

- Core control logic
- I²C communication handling
- Register-based protocol
- State machine framework
- Platform abstraction

These components define the **core system behavior**.

---

### node-specific directories (e.g., servo_node/)

Each node directory contains:

- Device-specific control logic
- Configuration
- Quick start guide
- Design documentation
- Test procedures

Examples:

- `servo_node/` → actuator control
- `sensor_node/` → measurement
- `motor_node/` → drive control

---

## Build Concept

Each firmware is built by combining:

- Shared components from `common/`
- Node-specific implementation from each node directory

In other words:

> Node directories are not standalone firmware,  
> but extensions of the shared firmware core.

This structure enables:

- reuse of core functionality
- easy addition of new node types
- consistent behavior across the system

---

## Design Philosophy

The firmware abstracts physical devices into standardized nodes.

This allows:

- Hardware modules to be interchangeable
- Unified communication via register interfaces
- Consistent behavior across different node types

This architecture is similar to **microservices in software systems**,  
but applied to physical devices.

---

## Learn More

- How nodes work → [How It Works](../How It Works/)
- System Architecture → ../Docs/architecture/
- Protocol Design → ../Docs/protocol/
