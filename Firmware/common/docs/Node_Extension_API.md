# Node Extension API

This document defines the supported interface between the shared firmware core and node-specific firmware implementations.

It is intended for developers adding or modifying motor, servo, sensor, or future node types without depending on internal core implementation details.

---

## Architectural Boundary

```text
Node-specific implementation
        ↓ callbacks and public API
Firmware/common/include
        ↓
Firmware/common/core
        ↓
Firmware/common/platform
```

Node-specific code should depend on the public headers in `Firmware/common/include/`.

It should not access internal variables or functions in `Firmware/common/core/` directly.

---

## Public Callback Contract

Node-specific behavior is provided to the common core through a callback table.

The current callback contract includes:

| Callback | Called when | Responsibility |
|---|---|---|
| `init` | During common core initialization | Initialize node-specific GPIO, peripherals, and local state |
| `tick_10ms` | Approximately every 10 ms from the cooperative main loop | Perform periodic node-specific processing |
| `on_latch_apply` | After staged parameters are copied into active values | Apply new active parameters to node-local state |
| `on_run_start` | Before the common core enters BUSY | Validate and start the requested operation |
| `on_run_stop` | When RUN changes from 1 to 0 while BUSY | Stop the active operation safely |
| `on_estop_enter` | When E-STOP becomes latched | Disable output and move the device to a safe state |

---

## Callback Behavior

### `init`

Typical responsibilities:

- configure node-specific GPIO
- initialize PWM, ADC, timers, or communication peripherals
- set outputs to a safe default state
- initialize local control variables

The common core initializes shared register and platform behavior separately.

### `tick_10ms`

Typical responsibilities:

- update actuator output
- sample sensor input
- evaluate elapsed time or target conditions
- publish feedback
- detect completion
- call a completion API when the operation ends

The current runtime is cooperative rather than hard real-time.

The callback period is approximate because it is driven by the main loop.

### `on_latch_apply`

This callback runs after staged values have been committed as active runtime parameters.

Typical responsibilities:

- copy active values into node-local control variables
- configure direction, mode, limits, or timing
- validate device-specific parameter combinations
- prepare the device for a later RUN request

This callback does not start the operation.

### `on_run_start`

This callback decides whether the node-specific operation can start.

Return value:

| Return | Meaning |
|---|---|
| `true` | Start accepted |
| `false` | Start rejected by node-specific logic |

The common core already rejects RUN when the node is BUSY, in ESTOP, or not latched.

Node-specific code may additionally reject requests that are unsupported or unsafe for that node type.

### `on_run_stop`

This callback is used for a host-requested stop.

Typical responsibilities:

- disable PWM or actuator output
- stop motion
- release node-local timers
- preserve or reset local state as required

After the callback, the common core records an aborted completion and returns the node to READY.

### `on_estop_enter`

This callback should perform the fastest available safe shutdown supported by the node.

Typical responsibilities:

- disable actuator output
- stop motion
- force control pins into a safe state
- clear node-local active-operation state

The common core separately records the E-STOP state and source.

---

## Public Core API

Node-specific code can read active parameters, inspect lifecycle state, publish feedback, and report completion through the public core API.

### State Queries

| API | Purpose |
|---|---|
| `core_is_running()` | Return whether the node is currently BUSY |
| `core_is_estopped()` | Return whether E-STOP is latched |

These functions do not modify common-core state.

---

## Active Parameter Access

| API | Purpose |
|---|---|
| `core_get_op_mode()` | Read the active operation mode |
| `core_get_target_time()` | Read the active target time |
| `core_get_setpoint0()` | Read active setpoint 0 |
| `core_get_limit0()` | Read active limit 0 |
| `core_get_limit1()` | Read active limit 1 |

These functions return active values after `LATCH_APPLY`.

Node-specific code should not read staged register bytes directly.

---

## Feedback Publication

The common API provides publication of three 16-bit feedback values.

| Feedback register | Typical use |
|---|---|
| `FB_VALUE0` | Primary measured or calculated value |
| `FB_VALUE1` | Secondary measured or calculated value |
| `FB_VALUE2` | Additional status or diagnostic value |

Publishing feedback through the common API also:

- updates the register value
- increments `UPDATE_CNT`
- sets `DATA_READY`

This allows host software to detect that new data is available.

---

## Completion Reporting

The current public API provides completion functions for:

| Completion API | Meaning |
|---|---|
| target completion | Operation completed because its target condition was reached |
| stopped completion | Operation ended because it was stopped or aborted |

A completion call updates common state, completion flags, RUN, READY/BUSY status, `UPDATE_CNT`, and `DATA_READY`.

Node-specific code should not directly manipulate common completion registers.

---

## Expected Node-Specific Structure

A node-specific implementation should normally contain:

```text
node-specific source
├ initialization
├ callback implementations
├ local device state
├ local control logic
└ callback table
```

A typical dependency direction is:

```text
node-specific source
        ↓
node_core.h / node_app.h
```

The node implementation should not include private headers from `common/core/`.

---

## Unsupported Direct Access

Node-specific code should not directly:

- write common register storage
- change READY, BUSY, or ESTOP bits
- set completion flags
- clear RUN
- increment `UPDATE_CNT`
- set or clear `DATA_READY`
- bypass command validation
- call internal E-STOP handling
- depend on private variables in `node_core.c`

These operations belong to the common core.

---

## Operation Mode Handling

The shared profile definitions include operation modes such as:

- default
- speed
- time
- continuous
- one-shot

A node should explicitly support or reject each relevant mode.

Unsupported modes should not silently fall back to unrelated behavior unless that fallback is intentionally documented.

---

## Error and Warning Scope

The register map includes warning and error fields.

However, the current public node API does not expose a complete general-purpose warning/error setter interface.

Node implementations should therefore follow the current implemented API rather than relying on older spreadsheet-only interface proposals.

Where the Markdown documentation and older spreadsheets differ, the current source code is authoritative.

---

## Portability Scope

The common firmware architecture is portable across node types within the current Raspberry Pi Pico platform.

It is not currently platform-independent because the public and platform headers depend on:

- Raspberry Pi Pico SDK
- fixed GPIO assignments
- Pico-specific I²C and timing behavior

Porting to another MCU would require a separate platform abstraction effort.

---

## Build Profile Selection

The active node profile is selected in:

[`../platform/node_build_config.h`](../platform/node_build_config.h)

Exactly one build profile must be set to `1`.

Example:

```c
#define BUILD_PROFILE_DUMMY  0
#define BUILD_PROFILE_MOTOR  1
#define BUILD_PROFILE_SENSOR 0
#define BUILD_PROFILE_SERVO  0
```

The build stops with a compile-time error when zero or multiple profiles are selected.

This prevents invalid firmware images from being generated due to ambiguous profile configuration.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Callback types and public API | [`../include/node_core.h`](../include/node_core.h) |
| Callback table access | [`../include/node_app.h`](../include/node_app.h) |
| Common runtime behavior | [`../core/node_core.c`](../core/node_core.c) |
| Callback selection | [`../core/node_app.c`](../core/node_app.c) |
| Profile definitions | [`../platform/node_profile.h`](../platform/node_profile.h) |
| Build profile selection | [`../platform/node_build_config.h`](../platform/node_build_config.h) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- State model → [`State_Model.md`](./State_Model.md)
- Adding a new node → [`Adding_New_Node.md`](./Adding_New_Node.md)
- Register map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
