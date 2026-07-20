# core

This directory contains the shared runtime logic used by all Raspberry Pi Pico firmware nodes.

The core owns the common register interface, lifecycle transitions, command handling, parameter staging, completion reporting, E-STOP behavior, and invocation of node-specific callbacks.

---

## Purpose

The `core` layer implements the common execution model shared by every node profile.

It separates reusable runtime behavior from device-specific control logic.

The core decides:

- when a command is accepted or rejected
- when parameters become active
- when the node enters READY, BUSY, or ESTOP
- when completion and update indicators change
- when node-specific callbacks are invoked

Node-specific code decides what the connected device actually does.

---

## Key Files

| File | Responsibility |
|---|---|
| [`node_core.c`](./node_core.c) | Register storage, I²C access, staged writes, lifecycle transitions, command validation, E-STOP handling, feedback publication, and completion reporting |
| [`node_app.c`](./node_app.c) | Resolves the selected node-specific callback table |
| [`node_profile.c`](./node_profile.c) | Resolves the selected node profile and build-time configuration |

Public declarations are defined in:

- [`../include/node_core.h`](../include/node_core.h)
- [`../include/node_app.h`](../include/node_app.h)

---

## Runtime Ownership

The common core owns:

- the shared register window
- READY, BUSY, and ESTOP state
- `LATCHED`
- command acceptance and rejection
- `LAST_CMD` and `LAST_CMD_RESULT`
- completion flags
- `DATA_READY`
- `UPDATE_CNT`
- E-STOP source and latch state
- staged and active command values
- invocation order for node-specific callbacks

Node-specific firmware should not edit these values directly.

It should use the public API exposed through `../include/`.

---

## Register Window

The current common register window is:

```text
0x00–0x3F
```

The core stores register data in a backing array and masks the current index to the 64-byte window.

Sequential access therefore wraps:

```text
0x3F → 0x00
```

Human-readable register documentation:

- [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
- [`../../../Docs/Register_Map/Common_Register_Map.md`](../../../Docs/Register_Map/Common_Register_Map.md)
- [`../../../Docs/Register_Map/Status_and_Completion.md`](../../../Docs/Register_Map/Status_and_Completion.md)
- [`../../../Docs/Register_Map/Command_and_Setpoint.md`](../../../Docs/Register_Map/Command_and_Setpoint.md)

---

## Lifecycle Model

The common lifecycle uses:

```text
READY
BUSY
ESTOP
```

Expected status combinations:

| Condition | READY | BUSY | ESTOP |
|---|---:|---:|---:|
| Idle | 1 | 0 | 0 |
| Running | 0 | 1 | 0 |
| E-STOP latched | 0 | 0 | 1 |

`LATCHED` is stored separately in `COMPLETE_FLAGS`.

It indicates whether staged parameters have been copied into active runtime values.

Detailed behavior:

- [`../docs/State_Model.md`](../docs/State_Model.md)

---

## Parameter Staging

The following 16-bit fields use staged writes:

- `SETPOINT0`
- `SETPOINT1`
- `TARGET_TIME`
- `HOLD_TIME`
- `LIMIT0`
- `LIMIT1`

The high byte is written first.

The low-byte write commits the full 16-bit value into the register window.

When the committed value changes, the core:

1. clears `LATCHED`
2. increments `UPDATE_CNT`
3. sets `DATA_READY`

Writing only the high byte does not commit the public value.

---

## LATCH_APPLY

`CMD_LATCH_APPLY` copies current register values into active runtime storage.

The current active set includes:

- `SETPOINT0`
- `SETPOINT1`
- `TARGET_TIME`
- `HOLD_TIME`
- `LIMIT0`
- `LIMIT1`
- `OP_MODE`
- level flags `DIR`, `MODE`, and `USE_SOFTLIMIT`

The request is accepted only when the node is neither BUSY nor ESTOP.

On acceptance, the core:

- updates active runtime values
- sets `LATCHED`
- calls `on_latch_apply`
- records `CMDRES_OK`
- increments `UPDATE_CNT`
- sets `DATA_READY`

Applying parameters does not start the node.

---

## RUN Handling

### Rising edge

A RUN rising edge requests operation start.

The common core rejects the request when:

- the node is already BUSY
- E-STOP is active
- parameters are not latched
- node-specific `on_run_start` validation returns `false`

Before invoking `on_run_start`, the core clears previous completion-reason flags.

On acceptance:

```text
READY = 0
BUSY = 1
```

On node-specific rejection:

```text
READY = 1
BUSY = 0
RUN = 0
LAST_CMD_RESULT = CMDRES_REJECT_STATE
```

### Falling edge

A RUN falling edge while BUSY:

1. calls `on_run_stop`
2. marks the operation as stopped
3. sets `DONE` and `DONE_BY_ABORT`
4. clears BUSY
5. sets READY
6. clears RUN

A RUN falling edge while idle is accepted as a no-op.

---

## Command Results

The common core records the most recently handled command and result.

Current result values include:

| Result | Meaning |
|---|---|
| `CMDRES_OK` | Command accepted |
| `CMDRES_REJECT_BUSY` | Rejected because BUSY |
| `CMDRES_REJECT_UNLATCHED` | RUN rejected because parameters are not latched |
| `CMDRES_REJECT_ESTOP` | Rejected because E-STOP is active |
| `CMDRES_REJECT_STATE` | Rejected by lifecycle or node-specific validation |
| `CMDRES_ACCEPTED_NOOP` | Accepted with no required transition |

A successful I²C transfer does not imply semantic command acceptance.

Host software should read `LAST_CMD_RESULT`.

---

## Command Processing Order

A `CMD_FLAGS` write is processed in this order:

```text
1. E-STOP reset
2. LATCH_APPLY
3. RUN rising or falling edge
```

Multiple logical commands in one write may all be processed.

The later action may overwrite `LAST_CMD` and `LAST_CMD_RESULT`.

For precise attribution, host software should send one logical one-shot request per write.

---

## E-STOP Behavior

The core monitors:

- shared E-STOP bus input
- optional local E-STOP input

When E-STOP is latched, the core:

- records source bits
- clears READY and BUSY
- sets ESTOP
- sets `DONE` and `DONE_BY_ABORT`
- clears RUN
- invokes `on_estop_enter`
- increments `UPDATE_CNT`
- sets `DATA_READY`

Reset is accepted only when:

- E-STOP is latched
- the node is not BUSY
- all monitored physical inputs are released

The reset path returns the node to READY but does not automatically clear completion flags.

---

## Feedback Publication

Node-specific firmware publishes three 16-bit feedback values through:

```c
core_publish_fb_u16(v0, v1, v2)
```

The core writes:

- `FB_VALUE0`
- `FB_VALUE1`
- `FB_VALUE2`

and then:

- increments `UPDATE_CNT`
- sets `DATA_READY`

The meaning and unit of each channel are defined by the selected node profile.

---

## Completion Reporting

The current public completion APIs are:

```c
core_finish_done_target()
core_finish_stopped()
```

### Target completion

The core sets:

```text
DONE
DONE_BY_TARGET
READY
```

and clears:

```text
BUSY
RUN
```

### Stopped or aborted completion

The core sets:

```text
DONE
DONE_BY_ABORT
READY
```

and clears:

```text
BUSY
RUN
```

Public APIs for `DONE_BY_TIME` and `DONE_BY_CONDITION` are not currently implemented.

---

## DATA_READY and UPDATE_CNT

The common update helper:

1. increments the 16-bit `UPDATE_CNT`
2. writes its high and low bytes
3. sets `STATUS_DATA_READY`

`STATUS_DATA_READY` uses write-one-to-clear behavior.

`UPDATE_CNT` is a change detector, not a one-command-one-increment counter.

A single host action may cause multiple increments.

---

## Callback Boundary

The common core invokes the callback table defined by `node_app_callbacks_t`.

Current callbacks are:

```c
init
tick_10ms
on_latch_apply
on_run_start
on_run_stop
on_estop_enter
```

The core owns when these callbacks run.

Node-specific firmware owns their device behavior.

Detailed callback and API rules:

- [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)

---

## Profile Selection

`node_profile.c` resolves the profile selected at build time.

The profile supplies:

- profile ID
- module type
- hardware revision
- firmware revision
- capability flags
- optional register defaults

Build selection is configured in:

- [`../platform/node_build_config.h`](../platform/node_build_config.h)

Exactly one profile must be selected.

---

## Current Limitations and Extension Points

The current core intentionally leaves several fields incomplete:

- no public setter for `ERROR_CODE`
- no public setter for `WARN_CODE`
- no public setter for `STATE_FLAGS0/1`
- `FB_SEQ` is not incremented
- no public getter for `SETPOINT1`
- no public getter for `HOLD_TIME`
- no public getter for active DIR, MODE, or USE_SOFTLIMIT
- no dispatcher for `CMD_IMMEDIATE`
- no public completion API for time or condition completion
- nominal read-only access is not enforced for every register by the generic write path

These are current implementation boundaries, not supported behavior.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Register and bit definitions | [`../include/node_core.h`](../include/node_core.h) |
| Runtime and write behavior | [`node_core.c`](./node_core.c) |
| Callback selection | [`node_app.c`](./node_app.c) |
| Profile selection | [`node_profile.c`](./node_profile.c) |
| Lifecycle documentation | [`../docs/State_Model.md`](../docs/State_Model.md) |
| Extension contract | [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- Public headers → [`../include/`](../include/)
- Platform layer → [`../platform/`](../platform/)
- State model → [`../docs/State_Model.md`](../docs/State_Model.md)
- Node Extension API → [`../docs/Node_Extension_API.md`](../docs/Node_Extension_API.md)
- Adding a new node → [`../docs/Adding_New_Node.md`](../docs/Adding_New_Node.md)
- Register map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
