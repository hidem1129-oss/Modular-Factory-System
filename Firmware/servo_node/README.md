# Servo Node

This directory contains the implementation of a **servo actuator node**  
based on the shared firmware core.

---

## Overview

The servo node is an example of a **node-specific implementation**  
that extends the common firmware system.

It demonstrates how a physical actuator can be controlled through:

- register-based commands
- state machine execution
- distributed node architecture

---

## What it does

The servo node allows:

- position control via register input
- execution of movement commands
- feedback reporting through registers

It is designed as a simple and clear example of how to integrate an actuator  
into the modular node system.

---

## Architecture

The servo node is built on top of the shared firmware layers:

```text
node-specific (servo_node)
        ↓
include (public API)
        ↓
core (execution engine)
        ↓
platform (hardware/runtime)
```

## Key Components
node_app_servo.c
Defines the behavior of the servo node.

- reads register values
- executes control logic
- writes feedback values

## node_profile_servo.c

Defines the characteristics of the node:

- module type
- capabilities
- default parameters

## servo_hal.c / servo_hal.h

Hardware abstraction layer for servo control.

- PWM output
- timing control
- device-specific handling

## How it works

1. Host writes target values to registers

2. Core validates and processes commands

3. Servo node receives execution via callbacks

4. Servo movement is performed through HAL

5. Feedback is written back to registers

## Getting Started

👉 Start here:

[Quick Start Guide](Modular-Factory-System/Firmware/servo_node/quick_start/README.md)

## Related

- Core system → ../../common/core/
- API → ../../common/include/
- Platform → ../../common/platform/
