# platform

This directory contains platform-specific components required to run the firmware.

---

## Purpose

The `platform` layer defines how the firmware is executed on a specific hardware environment.

It includes:

- entry point
- build configuration
- platform-dependent implementation

---

## Responsibilities

- Provide the firmware entry point
- Define build-time configuration
- Integrate with the Pico SDK
- Handle platform-specific initialization

---

## Key Components

### main.c

The entry point of the firmware.

Responsibilities:

- initialize the core system
- register node-specific callbacks
- execute the main loop

---

### node_build_config.h

Defines which node configuration is active.

This file selects:

- the active node type (servo, sensor, motor, etc.)
- the corresponding profile and behavior

---

### node_profile.h

Defines the characteristics of each node:

- module type
- capabilities
- supported operation modes
- default configuration

---

### pico_sdk_import.cmake

Handles integration with the Raspberry Pi Pico SDK.

---

## Design Concept

The `platform` layer connects the **abstract firmware core** to the **real hardware environment**.

It defines:

- how the system starts
- what node configuration is used
- how the firmware is built and deployed

---

## Relationship to other layers

- `core/` defines how nodes behave
- `include/` defines how node-specific code interacts with the system
- `platform/` defines how everything runs on actual hardware

---

## Important Note

The platform layer should:

- remain minimal
- avoid embedding application logic
- only handle environment-specific concerns
