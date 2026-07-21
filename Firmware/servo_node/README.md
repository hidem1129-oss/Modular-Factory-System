# servo_node

This directory contains the device-specific firmware implementation for the servo actuator node.

The servo node uses the shared common firmware runtime for I²C register access, lifecycle handling, command validation, E-STOP behavior, completion reporting, and update signaling.

---

## Purpose

The servo node converts the common register-based command model into hobby-servo PWM output.

It currently supports:

- target-angle control
- step-limited movement
- time-oriented interpolation
- host-requested stop
- E-STOP output disable
- three-channel feedback publication

The implementation is intended for small tabletop mechanisms such as sorting gates, diverter arms, clamps, and other low-load positioning tasks.

---

## Directory Structure

| Path | Responsibility |
|---|---|
| [`src/`](./src/) | Servo-specific motion planning, callback implementation, profile metadata, and PWM hardware abstraction |
| [`README.md`](./README.md) | Servo behavior, parameter mapping, feedback mapping, timing limits, and extension points |

---

## Common Firmware Relationship

```text
host controller
      ↓ I²C registers
common/core
      ↓ callbacks and public API
servo_node
      ↓ servo HAL
PWM output
      ↓
servo actuator
```

The common firmware owns register access, lifecycle state, command results, completion flags, `DATA_READY`, and `UPDATE_CNT`.

The servo node owns target-angle interpretation, motion planning, commanded-angle state, PWM generation, servo-specific validation, feedback meaning, and output enable/disable behavior.

---

## Current Hardware Interface

| Signal | GPIO | Purpose |
|---|---:|---|
| Servo PWM | 2 | Servo control pulse output |

Current PWM assumptions:

```text
frame period: 20 ms
frequency: 50 Hz
minimum pulse: 500 µs
maximum pulse: 2400 µs
commanded angle range: 0–180°
center angle: 90°
```

Angle-to-pulse conversion is linear.

These pulse limits are implementation defaults and may not match every servo mechanically.

---

## Output Enable and Stop Behavior

PWM output is disabled during initialization, normal stop, target completion, and E-STOP.

Disabling output sets the PWM compare value to zero and disables the PWM slice.

The current implementation therefore does not maintain a holding pulse after movement completes or after stop.

---

## Supported Operation Modes

| `OP_MODE` | Support | Behavior |
|---|---|---|
| `OP_MODE_DEFAULT` | Supported | Falls back to `OP_MODE_SPEED` |
| `OP_MODE_SPEED` | Supported | Moves toward the target by a limited number of degrees per 10 ms tick |
| `OP_MODE_TIME` | Supported | Attempts to distribute the move over `TARGET_TIME` ticks |
| `OP_MODE_CONTINUOUS` | Not supported | Rejected |
| `OP_MODE_ONE_SHOT` | Not supported | Rejected |

---

## Command and Parameter Mapping

| Register | Servo-node meaning | Current support |
|---|---|---|
| `OP_MODE` | Selects SPEED or TIME planning | Used |
| `SETPOINT0` | Target angle in degrees | Used |
| `SETPOINT1` | Reserved | Not used |
| `TARGET_TIME` | Requested movement time in 10 ms ticks | Used in TIME mode |
| `HOLD_TIME` | Reserved | Not used |
| `LIMIT0` | Maximum angle step per 10 ms tick | Used in SPEED mode |
| `LIMIT1` | Reserved | Not used |
| `CMD_DIR` | No servo-specific meaning | Not used |
| `CMD_MODE` | Reserved | Not used |
| `CMD_USE_SOFTLIMIT` | Reserved | Not used |

---

## Target-Angle Validation

`SETPOINT0` is interpreted as an angle in degrees.

```text
0–180 → accepted
>180   → rejected
```

Values above 180 are rejected during configuration validation.

---

## SPEED Mode

In `OP_MODE_SPEED`, the node moves toward the target by a maximum step on each 10 ms callback.

`LIMIT0` is interpreted as degrees per 10 ms tick.

Normalization:

- `LIMIT0 = 0` becomes `1`
- values above `30` are clamped to `30`

Example:

```text
current angle = 30°
target angle = 90°
LIMIT0 = 5°
```

This requires about 12 nominal ticks, or approximately 120 ms.

Actual timing depends on the cooperative runtime.

---

## TIME Mode

In `OP_MODE_TIME`, the node attempts to interpolate from the current commanded angle to the target over `TARGET_TIME`.

`TARGET_TIME` is interpreted as 10 ms ticks.

- `TARGET_TIME = 0` is rejected
- positive values are accepted

The implementation uses Q8 fixed-point accumulation.

### Timing limitation

`TARGET_TIME` is a planning input, not a strict timing guarantee.

The current implementation forces a minimum movement of 1 degree whenever the calculated per-tick movement would otherwise be zero.

Therefore, a small move combined with a very large `TARGET_TIME` may complete earlier than requested.

Example:

```text
move distance = 10°
TARGET_TIME = 1000 ticks
```

Because movement remains at least 1° per effective update, the target may be reached in roughly 10 ticks rather than 1000 ticks.

TIME mode should therefore be understood as:

```text
best-effort time-oriented interpolation
```

not strict arrival-time control.

The cooperative 10 ms runtime also introduces scheduling variation.

---

## LATCH_APPLY Behavior

When `on_latch_apply` runs, the servo node:

1. reads active values through the common API
2. resolves the operation mode
3. validates target and mode parameters
4. creates a new movement plan from the current commanded position
5. publishes the current state

Current getters used:

```c
core_get_op_mode()
core_get_setpoint0()
core_get_target_time()
core_get_limit0()
core_get_limit1()
```

The configuration is validated again during `on_run_start`.

---

## Motion Replanning

A new plan is created from the current commanded angle.

Mode or target changes therefore do not reset the logical position to 90° before movement.

---

## RUN Behavior

### RUN start

On an accepted RUN rising edge:

- active configuration is loaded
- target and mode are validated
- PWM output is enabled
- the current commanded angle is written
- periodic motion begins

### RUN active

During each 10 ms callback:

- E-STOP is checked
- SPEED or TIME movement is updated
- the commanded angle is written to PWM
- feedback is published
- target completion is checked

### Target completion

When the commanded angle reaches the target, `core_finish_done_target()` is called.

The servo output is disabled before completion is reported.

### Host-requested stop and E-STOP

Both stop local RUN state, disable PWM output, and publish feedback.

The common core owns lifecycle state and E-STOP latching.

---

## Feedback Mapping

| Register | Meaning |
|---|---|
| `FB_VALUE0` | Current commanded/modelled angle in degrees |
| `FB_VALUE1` | Pulse width corresponding to the commanded angle, in microseconds |
| `FB_VALUE2` | Absolute difference between commanded angle and target angle |

---

## Commanded Position Is Not Measured Position

`FB_VALUE0` is not measured mechanical position.

The current firmware has no encoder, potentiometer feedback, stall detection, or external position sensor.

It reports the position generated by the internal motion model.

Likewise, `FB_VALUE2` is software-model error, not physical tracking error.

A stalled, disconnected, overloaded, or mechanically blocked servo may still appear to reach the target in firmware.

---

## Initialization

During `init`, the servo node:

1. configures PWM hardware
2. sets the logical center angle to 90°
3. disables PWM output
4. resets motion state
5. publishes initial feedback

Current defaults:

| Parameter | Default |
|---|---:|
| `SETPOINT0` | 90° |
| `TARGET_TIME` | 0 |
| `LIMIT0` | 5° per 10 ms tick |
| `LIMIT1` | 0 |

---

## Example Host Sequence

### SPEED mode

```text
1. Write OP_MODE = OP_MODE_SPEED
2. Write SETPOINT0 = 120
3. Write LIMIT0 = 5
4. Send LATCH_APPLY
5. Confirm LATCHED
6. Set RUN
7. Monitor BUSY and FB_VALUE0
8. Wait for DONE_BY_TARGET
```

### TIME mode

```text
1. Write OP_MODE = OP_MODE_TIME
2. Write SETPOINT0 = 45
3. Write TARGET_TIME = 100
4. Send LATCH_APPLY
5. Confirm LATCHED
6. Set RUN
7. Monitor FB_VALUE0 and FB_VALUE2
8. Wait for DONE_BY_TARGET
```

Because TIME mode is best-effort, actual completion time may differ from the requested value.

---

## Example Uses

- sorting gates
- diverter arms
- paper clamps
- lightweight pushers
- indicator arms
- small tabletop linkages

---

## Current Limitations

- commanded angle is not measured angle
- no encoder or potentiometer feedback
- no stall or overload detection
- no servo-disconnect detection
- no strict arrival-time guarantee in TIME mode
- minimum effective movement is 1° per update while motion remains
- timing depends on the cooperative 10 ms callback
- PWM pulse range is fixed to 500–2400 µs
- angle range is fixed to 0–180°
- PWM frequency is fixed to 50 Hz
- GPIO mapping is fixed
- `LIMIT1`, `SETPOINT1`, and `HOLD_TIME` are unused
- output is disabled after completion, so holding torque is not maintained
- no acceleration or jerk limiting
- no configurable mechanical safety limits below the full 0–180° range

---

## Extension Points

Possible extensions include:

- measured position feedback
- servo-specific calibration
- configurable pulse limits
- configurable angle range
- configurable mechanical safety limits
- optional holding output after completion
- strict time-based trajectory generation
- fractional-angle internal state
- acceleration and deceleration profiles
- stall detection
- current sensing
- external limit switches
- configurable PWM pin and frequency

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Servo motion planning and callback behavior | [`src/node_app_servo.c`](./src/node_app_servo.c) |
| Servo profile metadata and defaults | [`src/node_profile_servo.c`](./src/node_profile_servo.c) |
| PWM hardware behavior | [`src/servo_hal.c`](./src/servo_hal.c) |
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
- Motor node → [`../motor_node/`](../motor_node/)
- Sensor node → [`../sensor_node/`](../sensor_node/)
