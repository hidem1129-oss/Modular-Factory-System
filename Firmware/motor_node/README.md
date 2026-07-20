# motor_node

This directory contains the device-specific firmware implementation for the DC motor node.

The motor node uses the shared common firmware runtime for I²C register access, lifecycle handling, command validation, E-STOP behavior, completion reporting, and update signaling.

---

## Purpose

The motor node converts the common register-based command model into DC motor output behavior.

It currently supports:

- PWM output control
- forward and reverse direction
- continuous speed-oriented operation
- time-limited operation
- host-requested stop
- E-STOP safe shutdown
- three-channel feedback publication

The implementation is intended for tabletop motion mechanisms and proof-of-concept systems.

---

## Directory Structure

| Path | Responsibility |
|---|---|
| [`src/`](./src/) | Motor-specific callback implementation and hardware control |
| [`README.md`](./README.md) | Motor-node behavior, parameter mapping, feedback mapping, and limitations |

---

## Common Firmware Relationship

```text
host controller
      ↓ I²C registers
common/core
      ↓ callbacks and public API
motor_node
      ↓ GPIO and PWM
motor driver
      ↓
DC motor
```

The common firmware owns register access, lifecycle state, command results, completion flags, `DATA_READY`, and `UPDATE_CNT`.

The motor node owns PWM, direction, operation-mode interpretation, motor-specific validation, feedback meaning, and physical stop behavior.

---

## Current Hardware Interface

The current implementation targets a TB6612-style H-bridge interface.

| Signal | GPIO | Purpose |
|---|---:|---|
| `PWMA` | 2 | PWM output |
| `AIN2` | 3 | Direction input |
| `AIN1` | 4 | Direction input |
| `STBY` | 5 | Driver standby control |

PWM uses an 8-bit range of `0–255`.

The host-facing output command uses a normalized range of `0–1000`.

---

## Motor Driver Behavior

### Forward

```text
AIN1 = 1
AIN2 = 0
```

### Reverse

```text
AIN1 = 0
AIN2 = 1
```

### Normal stop

The current stop behavior is coast stop:

```text
PWM = 0
AIN1 = 0
AIN2 = 0
STBY = 1
```

### E-STOP

```text
PWM = 0
AIN1 = 0
AIN2 = 0
STBY = 0
```

The E-STOP path disables the driver in addition to removing PWM output.

---

## Supported Operation Modes

| `OP_MODE` | Support | Behavior |
|---|---|---|
| `OP_MODE_DEFAULT` | Supported | Falls back to `OP_MODE_SPEED` |
| `OP_MODE_SPEED` | Supported | Runs continuously at the configured output level |
| `OP_MODE_TIME` | Supported | Runs until `TARGET_TIME` expires |
| `OP_MODE_CONTINUOUS` | Not supported | Rejected |
| `OP_MODE_ONE_SHOT` | Not supported | Rejected |

Unsupported modes cause `on_run_start` to return `false`.

---

## Command and Parameter Mapping

| Register or flag | Motor-node meaning | Current support |
|---|---|---|
| `OP_MODE` | Selects SPEED or TIME behavior | Used |
| `SETPOINT0` | Normalized output level, `0–1000` | Used |
| `SETPOINT1` | Reserved | Not used |
| `TARGET_TIME` | Operation duration in 10 ms ticks | Used in TIME mode |
| `HOLD_TIME` | Reserved | Not used |
| `LIMIT0` | Reserved for future constraint | Not used |
| `LIMIT1` | Reserved for future constraint | Not used |
| `CMD_DIR` | Direction selection | Used |
| `CMD_MODE` | Reserved | Not used |
| `CMD_USE_SOFTLIMIT` | Reserved | Not used |

---

## Output-Level Normalization

`SETPOINT0` is interpreted as:

```text
0      → 0% output
1000   → full configured output
>1000  → clamped to 1000
```

Conversion:

```text
PWM duty = SETPOINT0 × 255 / 1000
```

The current implementation does not include acceleration ramps, current limiting, or closed-loop control.

---

## Direction

The active direction is read through:

```c
core_get_cmd_flags_level()
```

Direction mapping:

```text
CMD_DIR = 0 → forward
CMD_DIR = 1 → reverse
```

The host should write the intended direction before `LATCH_APPLY`.

---

## SPEED Mode

In `OP_MODE_SPEED`:

- output starts after an accepted RUN rising edge
- PWM remains active while RUN remains active
- the node does not complete automatically
- RUN falling edge stops the motor
- E-STOP disables the output immediately

`TARGET_TIME` is ignored.

---

## TIME Mode

In `OP_MODE_TIME`:

- output starts after an accepted RUN rising edge
- `TARGET_TIME` is interpreted as 10 ms ticks
- `TARGET_TIME = 0` is rejected
- the node stops when elapsed ticks reach the target
- completion is reported through `core_finish_done_target()`

Example:

```text
TARGET_TIME = 100
≈ 1 second
```

Timing is cooperative and approximate rather than hard real-time.

---

## LATCH_APPLY Behavior

When `on_latch_apply` runs, the motor node:

1. reads active values from the common API
2. resolves the operation mode
3. validates the configuration
4. creates an internal motor plan
5. publishes state

Current getters used:

```c
core_get_op_mode()
core_get_setpoint0()
core_get_target_time()
core_get_limit0()
core_get_limit1()
core_get_cmd_flags_level()
```

The node validates the active configuration again during `on_run_start`.

---

## RUN Start Validation

| Condition | Result |
|---|---|
| SPEED mode | Accepted |
| TIME mode with `TARGET_TIME > 0` | Accepted |
| TIME mode with `TARGET_TIME = 0` | Rejected |
| Unsupported mode | Rejected |

The common core also applies lifecycle validation before invoking the motor callback.

---

## Periodic Execution

The motor node runs through the cooperative `tick_10ms` callback.

While running, each tick:

- verifies common RUNNING state
- verifies E-STOP is inactive
- updates elapsed time
- updates pseudo position
- publishes feedback
- checks timed completion

Callbacks should remain short and non-blocking.

---

## Feedback Mapping

| Register | Meaning |
|---|---|
| `FB_VALUE0` | Pseudo integrated position |
| `FB_VALUE1` | Current normalized output level |
| `FB_VALUE2` | Elapsed 10 ms ticks since RUN start |

Publication uses:

```c
core_publish_fb_u16(position, output_level, elapsed_ticks);
```

---

## Pseudo Position

`FB_VALUE0` is not an encoder measurement.

It is a software-only estimate derived from output level and direction.

Conceptually:

```text
step = output_level / 16
```

with a minimum step of 1 for non-zero output.

Each 10 ms tick:

- forward adds the step
- reverse subtracts the step without going below zero

This value is useful for demonstration, UI testing, and feedback-flow validation.

It should not be interpreted as physical position, distance, speed, or encoder count.

---

## Completion and Stop Behavior

### Automatic completion

TIME mode calls:

```c
core_finish_done_target()
```

The common core records target completion and returns the node to READY.

### Host-requested stop

A RUN falling edge invokes `on_run_stop`.

The motor node stops output, clears local RUN state, and publishes feedback.

### E-STOP

The motor node clears local RUN state, clears output level, coasts the H-bridge outputs, disables STBY, and publishes feedback.

The common core owns the E-STOP latch and lifecycle state.

---

## Initialization

During `init`, the motor node:

1. configures STBY, AIN1, and AIN2 as outputs
2. configures PWMA for PWM
3. sets PWM wrap to 255
4. enables the driver
5. applies coast stop
6. resets internal state
7. publishes initial feedback

---

## Example Host Sequence

### Forward SPEED mode at 50%

```text
1. Write OP_MODE = OP_MODE_SPEED
2. Write SETPOINT0 = 500
3. Clear CMD_DIR
4. Send LATCH_APPLY
5. Confirm LATCHED
6. Set RUN
7. Confirm BUSY
```

### Reverse TIME mode

```text
1. Write OP_MODE = OP_MODE_TIME
2. Write SETPOINT0 = 700
3. Write TARGET_TIME = 200
4. Set CMD_DIR
5. Send LATCH_APPLY
6. Set RUN while preserving CMD_DIR
7. Wait for DONE_BY_TARGET
```

`TARGET_TIME = 200` is approximately 2 seconds.

---

## Example Uses

- conveyor belts
- paper-feed mechanisms
- take-up rollers
- rack-and-pinion demonstrations
- lead-screw prototypes
- other simple DC motor loads

These are examples rather than restrictions on the interface.

---

## Current Limitations

- open-loop PWM only
- no encoder input
- no measured RPM
- pseudo position is not measured position
- no acceleration or deceleration ramp
- no current sensing
- no torque estimation
- no closed-loop speed or position control
- `LIMIT0` and `LIMIT1` are unused
- `SETPOINT1` and `HOLD_TIME` are unused
- no dedicated brake-stop mode
- timing depends on the approximate 10 ms callback
- output units are normalized rather than physical
- GPIO mapping is fixed in source

---

## Extension Points

Possible extensions include:

- encoder feedback
- measured RPM
- current sensing
- acceleration ramps
- braking-mode selection
- active software limits
- closed-loop speed control
- closed-loop position control
- physical-unit metadata
- configurable pin mapping
- additional completion conditions

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Motor callback and hardware behavior | [`src/node_app_motor.c`](./src/node_app_motor.c) |
| Motor profile metadata and defaults | [`src/node_profile_motor.c`](./src/node_profile_motor.c) |
| Common public API | [`../common/include/node_core.h`](../common/include/node_core.h) |
| Common runtime behavior | [`../common/core/node_core.c`](../common/core/node_core.c) |
| Register contract | [`../../Docs/Register_Map/`](../../Docs/Register_Map/) |

---

## Related Documents

- Firmware overview → [`../README.md`](../README.md)
- Common firmware overview → [`../common/README.md`](../common/README.md)
- Node Extension API → [`../common/docs/Node_Extension_API.md`](../common/docs/Node_Extension_API.md)
- State model → [`../common/docs/State_Model.md`](../common/docs/State_Model.md)
- Register map → [`../../Docs/Register_Map/`](../../Docs/Register_Map/)
- Servo node → [`../servo_node/`](../servo_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)
