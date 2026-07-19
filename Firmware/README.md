# Firmware

This directory contains firmware for distributed Raspberry Pi Pico nodes used in the Modular Factory System.

The firmware is built around a shared node architecture. Each physical device is exposed as an I²C-controlled node with a common register interface, shared lifecycle handling, and node-specific behavior.

---

## Overview

Each firmware node:

- runs on a Raspberry Pi Pico
- communicates with the host controller through I²C
- exposes identification, status, command, parameter, and feedback registers
- follows the shared READY, BUSY, and ESTOP lifecycle
- applies staged parameters through `LATCH_APPLY`
- implements device-specific behavior through callbacks
- reports feedback and completion through the common core API

---

## Firmware Structure

| Directory | Responsibility |
|---|---|
| [`common/`](./common/) | Shared register handling, lifecycle control, command validation, E-STOP handling, platform entry point, and extension documentation |
| [`motor_node/`](./motor_node/) | DC motor-specific control behavior |
| [`servo_node/`](./servo_node/) | Servo-specific control behavior |
| [`sensor_node/`](./sensor_node/) | Sensor-specific acquisition and feedback behavior |

The node-specific directories extend the shared firmware core. They are not independent firmware architectures.

```text
node-specific implementation
        ↓ callbacks and public API
common/include
        ↓
common/core
        ↓
common/platform
        ↓
Raspberry Pi Pico
```

---

## Shared and Node-Specific Responsibilities

### Shared common core

The common firmware layer owns:

- I²C register behavior
- staged parameter handling
- command validation
- READY, BUSY, and ESTOP lifecycle state
- RUN start and stop handling
- completion flags
- `DATA_READY` and `UPDATE_CNT`
- E-STOP latching and reset rules
- invocation of node-specific callbacks

### Node-specific implementation

Each node implementation owns:

- local GPIO and peripheral initialization
- device-specific parameter interpretation
- actuator or sensor behavior
- device-specific validation
- feedback values
- target-completion decisions
- safe local shutdown during stop and E-STOP handling

Node-specific code should use the public common API instead of editing shared lifecycle state directly.

---

## Build Model

A firmware image is built by combining:

```text
shared common firmware
        +
selected node-specific implementation
        +
selected node profile
```

The active node profile is selected in:

[`common/platform/node_build_config.h`](./common/platform/node_build_config.h)

Exactly one build profile must be set to `1`.

The build stops with a compile-time error when zero or multiple profiles are selected.

This prevents ambiguous profile configuration from producing an unintended UF2 image.

Detailed build and flashing instructions are maintained in the common platform documentation.

---

## Common Firmware Documentation

| Document | Purpose |
|---|---|
| [`common/docs/State_Model.md`](./common/docs/State_Model.md) | READY, BUSY, ESTOP, LATCHED, completion, and update-state behavior |
| [`common/docs/Node_Extension_API.md`](./common/docs/Node_Extension_API.md) | Callback contract, public core API, ownership rules, and extension boundary |
| [`common/docs/Adding_New_Node.md`](./common/docs/Adding_New_Node.md) | Procedure for registering, building, and validating a new node type |

Start with the state model when reviewing runtime behavior.

Use the extension API when implementing node-specific code.

Use the new-node guide when adding another device category or profile.

---

## Register Interface

All node types expose the shared register-based interface defined by the common core.

Human-readable register documentation:

- [`../Docs/Register_Map/`](../Docs/Register_Map/)

Authoritative implementation definitions:

- [`common/include/node_core.h`](./common/include/node_core.h)
- [`common/core/node_core.c`](./common/core/node_core.c)

Where older spreadsheets or notes differ from the current implementation, the current source code is authoritative.

---

## Runtime Characteristics

The current firmware runtime:

- uses Raspberry Pi Pico SDK
- uses a cooperative polling loop
- calls common polling approximately every 1 ms
- calls node-specific periodic processing approximately every 10 ms
- uses I²C at 100 kHz
- derives the node address from a base address and a four-bit DIP setting
- is designed for tabletop prototyping rather than hard real-time control

The current portability target is reuse across node types on Raspberry Pi Pico, not portability across MCU platforms.

---

## Design Intent

The firmware separates shared lifecycle and communication behavior from device-specific logic.

This makes it possible to:

- reuse the same host-side register interface across different node types
- avoid duplicating command and state handling
- add new device behavior through a defined callback boundary
- keep local device control independent from process-level orchestration
- reject invalid build-profile selections automatically

---

## Related Documentation

- Project architecture → [`../Docs/System_Architecture/`](../Docs/System_Architecture/)
- Common firmware overview → [`common/`](./common/)
- Register map → [`../Docs/Register_Map/`](../Docs/Register_Map/)
- Hardware overview → [`../Hardware/`](../Hardware/)
- Software overview → [`../Software/`](../Software/)
