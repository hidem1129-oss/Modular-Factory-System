# platform

This directory contains the Raspberry Pi Pico-specific runtime entry point, build-profile selection, profile definitions, and Pico SDK integration used by the common firmware.

It connects the shared firmware core to the current hardware and build environment.

---

## Purpose

The `platform` layer defines:

- how the firmware starts
- how the cooperative runtime loop is scheduled
- which node profile is compiled
- which common profile metadata is available
- how the Pico SDK is imported

The platform layer should remain small.

Device behavior belongs in node-specific implementations, while shared lifecycle and register behavior belong in `common/core/`.

---

## Key Files

| File | Responsibility |
|---|---|
| [`main.c`](./main.c) | Pico firmware entry point and cooperative polling loop |
| [`node_build_config.h`](./node_build_config.h) | Compile-time selection of exactly one node profile |
| [`node_profile.h`](./node_profile.h) | Shared profile IDs, module types, capability flags, profile metadata, and operation-mode enum |
| [`pico_sdk_import.cmake`](./pico_sdk_import.cmake) | Pico SDK import helper used by CMake |

Related implementation:

- [`../core/node_profile.c`](../core/node_profile.c)
- [`../core/node_app.c`](../core/node_app.c)

---

## Runtime Startup

`main.c` performs the current startup sequence:

```text
1. initialize stdio
2. obtain the selected callback table
3. initialize the common core
4. enter the cooperative main loop
```

The initialization call is:

```c
node_core_init(node_app_get_callbacks());
```

This connects the selected node-specific callback table to the common runtime.

---

## Cooperative Runtime Loop

The current loop calls:

```c
node_core_poll_1ms();
```

once per loop, followed by:

```c
sleep_ms(1);
```

A simple counter calls:

```c
node_core_tick_10ms();
```

after ten loop iterations.

Conceptually:

```text
common poll approximately every 1 ms
node-specific tick approximately every 10 ms
```

The 10 ms function invokes the node-specific `tick_10ms` callback through the common core.

---

## Timing Characteristics

The current runtime is cooperative and approximate.

The actual interval includes:

- work performed by `node_core_poll_1ms`
- work performed by `node_core_tick_10ms`
- callback execution time
- Pico SDK and interrupt activity
- `sleep_ms(1)` scheduling overhead

Therefore:

- the loop is not a hard real-time scheduler
- the 1 ms and 10 ms values are nominal intervals
- long-running callbacks delay subsequent work
- node-specific callbacks should remain short and non-blocking

This model is suitable for the current tabletop PoC and demonstration environment.

Hard real-time guarantees would require a different scheduling and timing design.

---

## Platform Responsibilities

The platform layer owns:

- firmware entry point
- common runtime startup
- cooperative scheduling
- compile-time profile selection
- shared profile declarations
- Pico SDK build integration

It should not own:

- actuator control algorithms
- sensor interpretation
- profile-specific parameter validation
- host command policy
- lifecycle state transitions
- register update rules
- completion behavior

Those responsibilities belong to node-specific code or `common/core/`.

---

## Build Profile Selection

The active build profile is selected in:

- [`node_build_config.h`](./node_build_config.h)

Each profile flag must be either `0` or `1`.

Current options are:

```c
BUILD_PROFILE_DUMMY
BUILD_PROFILE_MOTOR
BUILD_PROFILE_SENSOR
BUILD_PROFILE_SERVO
```

Exactly one profile must be set to `1`.

Example:

```c
#define BUILD_PROFILE_DUMMY  0
#define BUILD_PROFILE_MOTOR  1
#define BUILD_PROFILE_SENSOR 0
#define BUILD_PROFILE_SERVO  0
```

---

## Compile-Time Validation

`node_build_config.h` sums the profile flags and requires the result to equal one.

The build fails when:

- every profile is `0`
- more than one profile is `1`

Current validation:

```c
#if (BUILD_PROFILE_DUMMY +      BUILD_PROFILE_MOTOR +      BUILD_PROFILE_SENSOR +      BUILD_PROFILE_SERVO) != 1
#error "Exactly one node profile must be enabled."
#endif
```

This prevents an ambiguous configuration from producing an unintended UF2 image.

---

## Active Profile Mapping

The enabled build flag maps to one `ACTIVE_NODE_PROFILE`.

| Build flag | Active profile |
|---|---|
| `BUILD_PROFILE_DUMMY` | `NODE_PROFILE_DUMMY` |
| `BUILD_PROFILE_MOTOR` | `NODE_PROFILE_MOTOR_BASIC` |
| `BUILD_PROFILE_SENSOR` | `NODE_PROFILE_SENSOR_BASIC` |
| `BUILD_PROFILE_SERVO` | `NODE_PROFILE_SERVO_BASIC` |

The selected profile is resolved by the common profile implementation.

Changing the node type currently requires editing the build configuration and rebuilding the firmware.

Runtime profile switching is not implemented.

---

## Profile IDs

`node_profile.h` defines the current profile IDs:

| Value | Profile |
|---:|---|
| `0x00` | `NODE_PROFILE_DUMMY` |
| `0x01` | `NODE_PROFILE_MOTOR_BASIC` |
| `0x02` | `NODE_PROFILE_SENSOR_BASIC` |
| `0x03` | `NODE_PROFILE_SERVO_BASIC` |

Profile IDs identify a firmware behavior profile.

They are separate from the broader module type.

---

## Module Types

Current module types are:

| Value | Module type |
|---:|---|
| `0x00` | `NODE_MODULE_GENERIC` |
| `0x02` | `NODE_MODULE_MOTOR` |
| `0x03` | `NODE_MODULE_SENSOR` |
| `0x04` | `NODE_MODULE_SERVO` |

A module type describes the general device category.

A profile ID identifies the concrete common firmware profile within that category.

---

## Capability Flags

Current `CAP_FLAGS0` definitions are:

| Bit | Symbol | Meaning |
|---:|---|---|
| 0 | `NODE_CAP_STAGED_16BIT` | Supports staged 16-bit parameter fields |
| 1 | `NODE_CAP_RUN_STOP` | Supports RUN and stop lifecycle behavior |
| 2 | `NODE_CAP_ESTOP` | Supports common E-STOP behavior |
| 3 | `NODE_CAP_FB_U16_3CH` | Supports three 16-bit feedback channels |

Profiles publish capability metadata to the shared register interface.

A capability flag indicates interface support, but node-specific details still require profile documentation.

---

## Profile Metadata

The common profile structure is:

```c
typedef struct node_profile {
    node_profile_id_t profile_id;
    const char *name;
    uint8_t module_type;
    uint8_t hw_rev;
    uint8_t fw_ver;
    uint8_t cap_flags0;
    uint8_t cap_flags1;
    void (*apply_defaults)(uint8_t *regs,
                           uint16_t reg_count);
} node_profile_t;
```

It currently provides:

- profile ID
- human-readable profile name
- module type
- hardware revision
- firmware version
- capability flags
- optional register-default callback

The profile does not contain device-control logic.

Node-specific callbacks remain separate.

---

## Operation Modes

`node_profile.h` defines the shared `OP_MODE` enumeration:

| Value | Symbol | General meaning |
|---:|---|---|
| `0x00` | `OP_MODE_DEFAULT` | Profile-defined default behavior |
| `0x01` | `OP_MODE_SPEED` | Speed-oriented behavior |
| `0x02` | `OP_MODE_TIME` | Time-oriented behavior |
| `0x10` | `OP_MODE_CONTINUOUS` | Continuous operation |
| `0x11` | `OP_MODE_ONE_SHOT` | One-shot operation |

The numeric meaning is shared across node types.

However, each node implementation defines:

- which modes it supports
- how parameters are interpreted
- which values are valid
- whether an explicitly documented fallback exists

Unsupported modes should be rejected.

Fallback behavior should not be assumed.

---

## Pico SDK Integration

`pico_sdk_import.cmake` imports the Raspberry Pi Pico SDK into the CMake build.

The exact SDK location and build invocation are controlled by the surrounding CMake environment.

This file is platform integration support rather than application logic.

It should normally remain unchanged unless the Pico SDK integration method changes.

---

## Portability Boundary

The common firmware separates node behavior from platform startup, but it is not currently MCU-independent.

Current platform dependencies include:

- Pico SDK headers
- Pico GPIO and timing functions
- Pico I²C slave support
- Pico CMake integration

Current portability mainly means:

```text
reuse the same common runtime
and node-extension pattern
across Pico-based node profiles
```

Supporting another MCU would require a new or refactored platform layer and removal or isolation of Pico-specific types from the public boundary.

---

## Adding a New Build Profile

A new profile generally requires:

1. adding a profile ID to `node_profile.h`
2. adding a build flag to `node_build_config.h`
3. including the new flag in the exactly-one validation expression
4. mapping the flag to `ACTIVE_NODE_PROFILE`
5. defining profile metadata in the profile implementation
6. binding the node callback table
7. updating documentation
8. rebuilding and validating the resulting UF2

Complete procedure:

- [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md)

---

## Build and Flash Relationship

The platform files participate in selecting and producing one firmware image.

Typical flow:

```text
select one build profile
        ↓
configure and build with CMake
        ↓
produce UF2
        ↓
place Pico in BOOTSEL mode
        ↓
copy UF2 to the Pico drive
```

Generated build output should remain outside the source documentation and implementation directories.

See the firmware overview for repository-level build guidance:

- [`../../README.md`](../../README.md)

---

## Current Limitations and Extension Points

- profile selection is compile-time only
- one build produces one active node profile
- timing is cooperative and approximate
- callbacks can delay the main loop
- no hard real-time guarantee exists
- platform declarations still expose Pico SDK dependencies
- capability flags describe broad support, not detailed profile semantics
- no automatic profile generation or build matrix is currently provided

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Runtime entry point and scheduling | [`main.c`](./main.c) |
| Build-profile selection | [`node_build_config.h`](./node_build_config.h) |
| Profile and operation-mode declarations | [`node_profile.h`](./node_profile.h) |
| Profile implementation | [`../core/node_profile.c`](../core/node_profile.c) |
| Callback selection | [`../core/node_app.c`](../core/node_app.c) |
| Pico SDK import | [`pico_sdk_import.cmake`](./pico_sdk_import.cmake) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- Core runtime → [`../core/`](../core/)
- Public headers → [`../include/`](../include/)
- Adding a new node → [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md)
- Node Extension API → [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)
- Firmware overview → [`../../README.md`](../../README.md)
- Register map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
