# common

This directory contains the shared firmware components used across all Raspberry Pi Pico nodes in the Modular Factory System.

The common layer defines the register interface, lifecycle behavior, command handling, E-STOP behavior, public node-extension API, and Pico-specific runtime entry point.

---

## Purpose

The common layer provides the reusable firmware foundation for all node types.

It keeps shared communication and lifecycle behavior separate from device-specific control logic.

This allows motor, servo, sensor, and future nodes to expose a consistent host-side interface without duplicating common state handling.

---

## Responsibilities

The common layer owns:

- I²C register handling
- staged parameter writes
- `LATCH_APPLY`
- RUN start and stop validation
- READY, BUSY, and ESTOP lifecycle state
- completion flags
- `DATA_READY` and `UPDATE_CNT`
- E-STOP latching and reset rules
- invocation of node-specific callbacks
- public APIs for node-specific implementations
- build-profile selection
- Pico-specific runtime startup and polling

Node-specific firmware owns the connected device behavior and uses the public common API to interact with this layer.

---

## Directory Structure

| Directory | Responsibility |
|---|---|
| [`core/`](./core/) | Shared register storage, command handling, lifecycle transitions, callback selection, and profile selection |
| [`include/`](./include/) | Public headers defining the callback contract, common API, register constants, and shared types |
| [`platform/`](./platform/) | Pico entry point, runtime scheduling, profile definitions, build selection, and Pico SDK integration |
| [`docs/`](./docs/) | State model, node-extension contract, and new-node integration procedure |

---

## Dependency Direction

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

Node-specific code should depend on the public headers in `common/include/`.

It should not access internal variables or private implementation details in `common/core/` directly.

---

## Common Firmware Documentation

| Document | Purpose |
|---|---|
| [`docs/State_Model.md`](./docs/State_Model.md) | READY, BUSY, ESTOP, LATCHED, completion flags, and update signaling |
| [`docs/Node_Extension_API.md`](./docs/Node_Extension_API.md) | Callback contract, public core API, ownership rules, and extension boundary |
| [`docs/Adding_New_Node.md`](./docs/Adding_New_Node.md) | Procedure for registering, building, flashing, and validating a new node type |

These documents describe the supported extension boundary.

Implementation details remain authoritative where older notes or spreadsheets differ.

---

## Runtime Model

The current runtime uses a cooperative main loop.

```text
common poll approximately every 1 ms
node-specific tick approximately every 10 ms
```

The runtime is intended for tabletop prototyping and is not a hard real-time scheduler.

The current platform target is Raspberry Pi Pico using the Pico SDK.

---

## Build Profile Selection

The active node profile is selected in:

[`platform/node_build_config.h`](./platform/node_build_config.h)

Exactly one build profile must be set to `1`.

The build stops with a compile-time error when:

- all profiles are `0`
- multiple profiles are `1`

This prevents ambiguous configuration from producing an unintended UF2 image.

---

## Register Interface

The shared register interface is implemented in:

- [`include/node_core.h`](./include/node_core.h)
- [`core/node_core.c`](./core/node_core.c)

Human-readable register documentation:

- [`../../Docs/Register_Map/`](../../Docs/Register_Map/)

The current source code is authoritative for register behavior, lifecycle transitions, and public API definitions.

---

## Extension Rules

When adding or modifying a node:

- reuse the common lifecycle and register behavior
- implement device-specific behavior through callbacks
- read active values through the public API
- publish feedback through the public API
- report completion through the public API
- do not edit READY, BUSY, ESTOP, completion flags, `DATA_READY`, or `UPDATE_CNT` directly
- explicitly reject unsupported operation modes
- preserve safe stop and E-STOP behavior

See [`docs/Adding_New_Node.md`](./docs/Adding_New_Node.md) for the complete procedure.

---

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- State model → [`docs/State_Model.md`](./docs/State_Model.md)
- Node Extension API → [`docs/Node_Extension_API.md`](./docs/Node_Extension_API.md)
- Adding a new node → [`docs/Adding_New_Node.md`](./docs/Adding_New_Node.md)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- System architecture → [`../../Docs/System_Architecture/`](../../Docs/System_Architecture/)
- Servo node → [`../servo_node/`](../servo_node/)
- Motor node → [`../motor_node/`](../motor_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)
