# include

This directory contains the public headers shared by all Raspberry Pi Pico node implementations.

It defines the supported boundary between node-specific firmware and the common runtime.

---

## Purpose

The `include` layer exposes the types, constants, callbacks, and functions that node-specific code may use.

It allows a node implementation to:

- receive lifecycle callbacks
- read active command values
- query common runtime state
- publish feedback
- report completion

It does not expose the private storage or internal transition helpers used by `common/core/`.

---

## Key Files

| File | Responsibility |
|---|---|
| [`node_core.h`](./node_core.h) | Common register constants, status and command bits, callback table, runtime entry points, and public node-extension API |
| [`node_app.h`](./node_app.h) | Declares the function used by the common runtime to obtain the selected node callback table |

Implementation files:

- [`../core/node_core.c`](../core/node_core.c)
- [`../core/node_app.c`](../core/node_app.c)

---

## Dependency Boundary

```text
node-specific implementation
        ↓
common/include
        ↓
common/core
```

Node-specific firmware should include headers from this directory.

It should not:

- include private implementation files from `common/core/`
- access the core register array directly
- modify READY, BUSY, ESTOP, completion flags, `DATA_READY`, or `UPDATE_CNT`
- reproduce lifecycle transitions in node-specific code
- call internal helpers that are not declared in the public headers

This keeps the shared runtime behavior consistent across node types.

---

## `node_core.h`

`node_core.h` currently contains four groups of declarations:

1. Pico and GPIO configuration constants
2. register addresses and public bit definitions
3. callback types and common runtime entry points
4. node-extension getter, publication, state-query, and completion APIs

Because the current platform target is Raspberry Pi Pico, the header includes Pico SDK types.

Portability currently means reusing node implementations across Pico-based profiles, not complete MCU independence.

---

## Register and Bit Definitions

`node_core.h` defines the common 64-byte I²C register window:

```text
0x00–0x3F
```

It also defines:

- register addresses
- `STATUS` bits
- E-STOP source bits
- completion bits
- command bits
- command-result values

These constants describe the host-visible register contract.

Human-readable specification:

- [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
- [`../../../Docs/Register_Map/Common_Register_Map.md`](../../../Docs/Register_Map/Common_Register_Map.md)
- [`../../../Docs/Register_Map/Status_and_Completion.md`](../../../Docs/Register_Map/Status_and_Completion.md)
- [`../../../Docs/Register_Map/Command_and_Setpoint.md`](../../../Docs/Register_Map/Command_and_Setpoint.md)

The header and current core implementation remain authoritative when older spreadsheets differ.

---

## Callback Contract

Node-specific behavior is supplied through:

```c
typedef struct {
    void (*init)(void);
    void (*tick_10ms)(void);
    void (*on_latch_apply)(void);
    bool (*on_run_start)(void);
    void (*on_run_stop)(void);
    void (*on_estop_enter)(void);
} node_app_callbacks_t;
```

### Callback responsibilities

| Callback | Purpose |
|---|---|
| `init` | Initialize node-specific hardware and state |
| `tick_10ms` | Perform cooperative periodic node work |
| `on_latch_apply` | React after staged parameters become active |
| `on_run_start` | Validate and begin operation; return `false` to reject |
| `on_run_stop` | Stop the device after a host RUN falling edge |
| `on_estop_enter` | Immediately move node-specific hardware toward a safe state |

The common core owns when these callbacks run.

Node-specific code owns what the connected device does inside them.

Detailed rules:

- [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)
- [`../docs/State_Model.md`](../docs/State_Model.md)

---

## Callback Provider

`node_app.h` declares:

```c
const node_app_callbacks_t *node_app_get_callbacks(void);
```

The common runtime calls this function to obtain the callback table selected for the current build profile.

Node implementations should normally provide callback tables through the existing profile-selection mechanism rather than modifying the common runtime entry point.

---

## Common Runtime Entry Points

The header also declares functions used by the Pico platform layer:

```c
void node_core_init(const node_app_callbacks_t *callbacks);
void node_core_poll_1ms(void);
void node_core_tick_10ms(void);
uint8_t node_core_get_i2c_addr(void);
void node_core_i2c_slave_handler(i2c_inst_t *i2c,
                                 i2c_slave_event_t event);
const node_profile_t *node_core_get_profile(void);
```

These functions initialize and run the common firmware.

They are primarily used by:

- [`../platform/main.c`](../platform/main.c)
- common profile-selection code
- the Pico I²C slave integration

Node-specific control code normally does not need to call them directly.

---

## Active Parameter Getters

The current public parameter getters are:

```c
uint8_t core_get_op_mode(void);
uint16_t core_get_target_time(void);
uint16_t core_get_setpoint0(void);
uint16_t core_get_limit0(void);
uint16_t core_get_limit1(void);
```

These functions return active values copied by an accepted `LATCH_APPLY`.

They do not expose partially written staged bytes.

### Current public coverage

| Active value | Public getter |
|---|---|
| `OP_MODE` | `core_get_op_mode()` |
| `SETPOINT0` | `core_get_setpoint0()` |
| `TARGET_TIME` | `core_get_target_time()` |
| `LIMIT0` | `core_get_limit0()` |
| `LIMIT1` | `core_get_limit1()` |
| `SETPOINT1` | Not currently exposed |
| `HOLD_TIME` | Not currently exposed |
| DIR | Not currently exposed |
| MODE | Not currently exposed |
| USE_SOFTLIMIT | Not currently exposed |

A register existing in the map does not automatically mean node-specific code can access its active copy through the supported API.

---

## Runtime State Queries

The current state-query functions are:

```c
bool core_is_running(void);
bool core_is_estopped(void);
```

Use these functions instead of reading or modifying internal status storage.

The common core remains responsible for READY, BUSY, and ESTOP transitions.

There is currently no dedicated public `core_is_ready()` function.

---

## Feedback Publication

Node-specific code publishes three 16-bit feedback channels through:

```c
void core_publish_fb_u16(uint16_t v0,
                         uint16_t v1,
                         uint16_t v2);
```

The common core then updates:

- `FB_VALUE0`
- `FB_VALUE1`
- `FB_VALUE2`
- `UPDATE_CNT`
- `DATA_READY`

The meaning and unit of each feedback channel are profile-specific.

Node-specific code should not write feedback registers directly.

---

## Completion Reporting

The current public completion functions are:

```c
void core_finish_done_target(void);
void core_finish_stopped(void);
```

### `core_finish_done_target()`

Use when the active operation reaches its target.

The common core records target completion and returns the lifecycle to READY.

### `core_finish_stopped()`

Use when an operation ends through stop or abort handling.

The common core records abort-style completion and returns the lifecycle to READY.

Node-specific code should not directly edit:

- `DONE`
- completion-reason bits
- READY
- BUSY
- RUN
- `UPDATE_CNT`
- `DATA_READY`

Public completion functions for `DONE_BY_TIME` and `DONE_BY_CONDITION` are not currently provided.

---

## Recommended Node-Specific Usage

A node implementation typically:

```text
1. defines a callback table
2. initializes hardware in init
3. reads active parameters during on_latch_apply or on_run_start
4. rejects unsupported modes by returning false from on_run_start
5. performs cooperative work in tick_10ms
6. publishes feedback through core_publish_fb_u16
7. reports completion through a public completion function
8. stops hardware in on_run_stop and on_estop_enter
```

A full integration procedure is documented in:

- [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md)

---

## Ownership Rules

### Common core owns

- register storage
- lifecycle transitions
- command results
- staged-to-active transfer
- E-STOP latch and reset
- completion flags
- update signaling
- callback invocation timing

### Node-specific code owns

- GPIO, PWM, ADC, or device-driver behavior
- interpretation of profile-supported operation modes
- validation of device-specific parameter ranges
- feedback meaning and units
- physical safe-stop behavior

The public API is the handoff point between these responsibilities.

---

## Current Limitations and Extension Points

The current header does not expose:

- `SETPOINT1`
- `HOLD_TIME`
- active DIR
- active MODE
- active USE_SOFTLIMIT
- setters for `ERROR_CODE`
- setters for `WARN_CODE`
- setters for `STATE_FLAGS0/1`
- time-completion reporting
- condition-completion reporting
- profile-defined feedback flags
- an immediate-command callback

These fields may exist in the register map or internal storage, but they are not part of the current supported node-extension API.

---

## Header Maintenance Rule

When a new public function is added:

1. declare it in the appropriate public header
2. implement it in the common core
3. document ownership and lifecycle effects
4. update [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)
5. update Register Map documentation when host-visible behavior changes
6. update [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md) when the integration procedure changes

Avoid exposing raw internal storage merely to make one node implementation easier.

Prefer a narrow API that preserves common lifecycle ownership.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Public declarations and constants | [`node_core.h`](./node_core.h) |
| Callback provider declaration | [`node_app.h`](./node_app.h) |
| Runtime implementation | [`../core/node_core.c`](../core/node_core.c) |
| Callback selection | [`../core/node_app.c`](../core/node_app.c) |
| Extension contract | [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md) |
| Lifecycle behavior | [`../docs/State_Model.md`](../docs/State_Model.md) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- Core runtime → [`../core/`](../core/)
- Platform layer → [`../platform/`](../platform/)
- Node Extension API → [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)
- Adding a new node → [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md)
- Register map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
