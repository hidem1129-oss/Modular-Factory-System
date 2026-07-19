# Adding a New Node

This document describes the current procedure for adding a new firmware node type to the Modular Factory System.

The goal is to extend the existing common firmware architecture without duplicating lifecycle, register, command, or E-STOP handling.

---

## Overview

A new node is added by combining:

```text
new node-specific implementation
        +
shared common firmware core
        +
new node profile
        +
build selection entry
```

The common core already provides:

- I²C register handling
- staged parameter writes
- `LATCH_APPLY`
- `RUN` start and stop handling
- READY, BUSY, and ESTOP lifecycle management
- completion flags
- `DATA_READY`
- `UPDATE_CNT`
- E-STOP latching and reset rules

The new node implementation should provide only the device-specific behavior.

---

## Before Starting

Confirm that the new device can be represented through the existing common interface.

Check whether the node can use:

- the existing register window
- the existing setpoint, target time, hold time, and limit fields
- the existing feedback values
- the existing operation modes
- the existing completion paths
- the existing callback contract

If the new node requires a new common register or lifecycle behavior, update the common specification first rather than hiding the requirement inside node-specific code.

---

## Recommended File Structure

A new node directory should follow the same pattern as the existing node implementations.

Example:

```text
Firmware/
└ new_node/
   ├ README.md
   ├ CMakeLists.txt
   ├ new_node.c
   └ new_node.h
```

The exact filenames may differ, but the responsibilities should remain clear:

| File | Responsibility |
|---|---|
| `README.md` | Node purpose, supported modes, parameters, feedback, and constraints |
| `CMakeLists.txt` | Build registration for the node-specific source |
| source file | Callback implementation and local device logic |
| header file | Public declarations required by the build |

Avoid copying common lifecycle code into the new node directory.

---

## Step 1: Define the Node Profile ID

Add a new profile identifier in:

[`../platform/node_profile.h`](../platform/node_profile.h)

The identifier must be unique.

Example pattern:

```c
#define NODE_PROFILE_NEW_DEVICE 4
```

Use the next available value rather than reusing an existing profile ID.

---

## Step 2: Define or Reuse the Module Type

The profile includes a module type used to identify the device category.

First determine whether the new node belongs to an existing category.

Examples:

- motor
- servo
- sensor
- generic

Reuse an existing module type when the host software can interpret the new node as the same class.

Add a new module type only when the distinction is meaningful to external software or documentation.

The module type definition is also maintained in:

[`../platform/node_profile.h`](../platform/node_profile.h)

---

## Step 3: Define Supported Capabilities

Create a profile entry that describes the node.

A profile typically includes:

- profile ID
- profile name
- module type
- hardware revision
- firmware revision
- capability bits
- default callback table
- supported operation modes

Do not advertise a capability that the node-specific implementation does not actually support.

Typical capabilities include:

- staged parameter handling
- RUN and STOP
- E-STOP handling
- feedback channels

---

## Step 4: Implement the Callback Table

The node-specific implementation must provide callbacks matching the contract in:

[`Node_Extension_API.md`](./Node_Extension_API.md)

Current callbacks are:

```text
init
tick_10ms
on_latch_apply
on_run_start
on_run_stop
on_estop_enter
```

A typical callback table has the following conceptual form:

```c
static const node_callbacks_t callbacks = {
    .init = new_node_init,
    .tick_10ms = new_node_tick_10ms,
    .on_latch_apply = new_node_on_latch_apply,
    .on_run_start = new_node_on_run_start,
    .on_run_stop = new_node_on_run_stop,
    .on_estop_enter = new_node_on_estop_enter,
};
```

Use the exact type and field names defined in:

[`../include/node_core.h`](../include/node_core.h)

---

## Step 5: Implement Initialization

The node-specific `init` callback should:

- configure required GPIO
- initialize PWM, ADC, or other peripherals
- set outputs to a safe inactive state
- initialize local variables
- avoid starting the device automatically

The common core separately initializes shared state and communication behavior.

A node should remain safe even if initialization completes but the host never sends a command.

---

## Step 6: Apply Active Parameters

Implement `on_latch_apply`.

This callback runs after staged register values have been copied into active runtime values.

Use public getter functions such as:

- `core_get_op_mode()`
- `core_get_target_time()`
- `core_get_setpoint0()`
- `core_get_limit0()`
- `core_get_limit1()`

Typical work includes:

- converting common values into device-local units
- storing active parameters in local state
- checking parameter combinations
- preparing control variables
- selecting an operation mode

Do not begin physical operation in this callback.

---

## Step 7: Validate and Start the Operation

Implement `on_run_start`.

The callback should:

1. confirm that the requested operation mode is supported
2. confirm that parameters are valid for this node
3. initialize operation-local state
4. enable the required device output
5. return `true` when the operation starts successfully

Return `false` when the node cannot safely or correctly start.

The common core already rejects RUN when:

- the node is BUSY
- E-STOP is active
- parameters have not been latched

The node-specific callback should focus on device-specific validation.

---

## Step 8: Implement Periodic Processing

Implement `tick_10ms`.

Typical responsibilities include:

- update actuator output
- read sensor input
- update elapsed time
- evaluate target or timeout conditions
- publish feedback
- report completion

The callback runs from a cooperative main loop.

Its period is approximate and should not be treated as a hard real-time guarantee.

Avoid blocking operations that would delay common-core polling.

---

## Step 9: Publish Feedback

Use the public feedback API instead of writing common registers directly.

The current common interface supports three 16-bit feedback values.

Use them consistently and document their meaning in the node README.

| Feedback | Meaning |
|---|---|
| `FB_VALUE0` | Primary measured value |
| `FB_VALUE1` | Secondary measured value |
| `FB_VALUE2` | State, count, or diagnostic value |

Publishing through the common API also updates `UPDATE_CNT` and `DATA_READY`.

---

## Step 10: Report Completion

When the operation reaches its target, call the appropriate public completion API.

The current implementation provides completion paths for:

- target completion
- stopped or aborted completion

Do not directly modify:

- READY
- BUSY
- RUN
- completion flags
- `UPDATE_CNT`
- `DATA_READY`

The common core owns these values.

---

## Step 11: Implement Stop and E-STOP Behavior

### `on_run_stop`

This callback should stop the active operation safely when the host clears RUN.

Typical actions:

- disable PWM
- stop motor output
- release timers
- clear local running state

### `on_estop_enter`

This callback should force the device into the safest available local state.

Typical actions:

- disable all active outputs
- stop motion
- set control pins to a safe level
- clear device-local operation state

The common core separately records and manages the E-STOP lifecycle.

---

## Step 12: Register the Callback Table

Add the new node to callback selection in:

[`../core/node_app.c`](../core/node_app.c)

The selection logic maps `ACTIVE_NODE_PROFILE` to the callback table for the selected node.

Add a case for the new profile.

Do not change the default fallback behavior unless there is a deliberate reason to do so.

---

## Step 13: Register the Profile

Add the new profile to profile selection in:

[`../core/node_profile.c`](../core/node_profile.c)

The selected profile should match the new callback implementation and advertised capabilities.

Check that:

- profile ID is correct
- module type is correct
- revision values are correct
- capability bits match implementation
- default callback table is correct

---

## Step 14: Add the Build Selection Entry

Add a new build-selection flag in:

[`../platform/node_build_config.h`](../platform/node_build_config.h)

Example:

```c
#define BUILD_PROFILE_DUMMY      0
#define BUILD_PROFILE_MOTOR      0
#define BUILD_PROFILE_SENSOR     0
#define BUILD_PROFILE_SERVO      0
#define BUILD_PROFILE_NEW_DEVICE 1
```

Also include the new flag in the compile-time selection count:

```c
#if (BUILD_PROFILE_DUMMY + \
     BUILD_PROFILE_MOTOR + \
     BUILD_PROFILE_SENSOR + \
     BUILD_PROFILE_SERVO + \
     BUILD_PROFILE_NEW_DEVICE) != 1
#error "Exactly one node profile must be enabled."
#endif
```

Then add the corresponding selection branch:

```c
#elif BUILD_PROFILE_NEW_DEVICE
#define ACTIVE_NODE_PROFILE NODE_PROFILE_NEW_DEVICE
```

Exactly one build profile must remain set to `1`.

Test both failure cases:

- all profiles set to `0`
- multiple profiles set to `1`

Both must stop the build.

---

## Step 15: Update the Build Files

Add the new node-specific source files to the relevant CMake configuration.

Check:

- source files are included
- include directories are correct
- the common core is not duplicated
- the node profile selection reaches the new implementation
- the UF2 output is generated successfully

Follow the existing node build structure rather than creating an independent firmware project unless there is a clear architectural reason.

---

## Step 16: Build and Flash

1. Select the new profile in `node_build_config.h`.
2. Run the existing firmware build procedure.
3. Confirm that the UF2 file is generated under `build/`.
4. Put the Raspberry Pi Pico into BOOTSEL mode.
5. Copy the generated UF2 file to the Pico mass-storage device.
6. Allow the Pico to reboot.

The generated UF2 should contain only the selected node profile.

---

## Step 17: Verify I²C Communication

Before testing the physical operation, verify the common interface.

Recommended checks:

1. Confirm the expected I²C address.
2. Read the signature.
3. Read module type and profile identification.
4. Read firmware and hardware revision fields.
5. Confirm READY state.
6. Write a parameter.
7. Confirm that `LATCHED` is cleared.
8. Send `LATCH_APPLY`.
9. Confirm that `LATCHED` is set.
10. Send RUN.
11. Confirm BUSY.
12. Confirm completion and return to READY.
13. Confirm feedback updates.
14. Confirm `DATA_READY` write-one-to-clear behavior.

This separates protocol problems from device-control problems.

---

## Step 18: Verify Failure and Safety Behavior

Test at least:

- RUN without `LATCH_APPLY`
- RUN while already BUSY
- `LATCH_APPLY` while BUSY
- E-STOP while READY
- E-STOP while BUSY
- E-STOP reset while the physical input is still active
- host-requested stop
- unsupported operation mode
- invalid parameter combination
- loss of the connected actuator or sensor, where practical

Record which behaviors were tested and which remain unevaluated.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Public callback and API contract | [`../include/node_core.h`](../include/node_core.h) |
| Callback selection | [`../core/node_app.c`](../core/node_app.c) |
| Profile selection | [`../core/node_profile.c`](../core/node_profile.c) |
| Profile definitions | [`../platform/node_profile.h`](../platform/node_profile.h) |
| Build profile selection | [`../platform/node_build_config.h`](../platform/node_build_config.h) |
| Common lifecycle behavior | [`../core/node_core.c`](../core/node_core.c) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- Node Extension API → [`Node_Extension_API.md`](./Node_Extension_API.md)
- State Model → [`State_Model.md`](./State_Model.md)
- Register Map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
- Firmware overview → [`../../README.md`](../../README.md)
