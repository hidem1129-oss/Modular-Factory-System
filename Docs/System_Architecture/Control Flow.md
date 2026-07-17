# Control Flow

This document describes how control commands move from Raspberry Pi 5 host software to Raspberry Pi Pico firmware nodes and how the nodes report acceptance, execution, completion, and abnormal states.

---

## Control Path

```text
Host-side orchestration
        ↓
Write command and parameter registers
        ↓ I²C
Common firmware core
        ↓
Validate request
        ↓
Apply active parameters
        ↓
Invoke node-specific callback
        ↓
Drive actuator or read sensor
        ↓
Publish feedback and completion state
        ↓ I²C
Host-side monitoring
```

The host communicates with all node types through the same register-based interface.

The common firmware core controls the lifecycle. Node-specific code controls the device behavior.

---

## Standard Operation Sequence

A typical operation follows this sequence:

```text
1. Write parameter registers
2. Send LATCH_APPLY
3. Confirm command result
4. Set RUN
5. Node enters BUSY
6. Node-specific firmware performs the operation
7. Firmware publishes feedback and completion
8. Node clears RUN and returns to READY
```

---

## 1. Parameter Write

The host writes operation parameters such as:

- setpoint
- target time
- hold time
- limits
- operation mode
- direction or mode flags

The current common register window is `0x00` to `0x3F`.

Sixteen-bit values are transferred as:

```text
High byte
Low byte
```

The common core stages the high and low bytes and commits the value when the low byte is written.

When a staged value changes:

- the `LATCHED` flag is cleared
- `UPDATE_CNT` is incremented
- `DATA_READY` is set

This prevents previously applied parameters from being treated as current after a new value is written.

---

## 2. LATCH_APPLY

The host sends the one-shot `LATCH_APPLY` command through `CMD_FLAGS`.

When accepted, the common core:

1. copies staged register values into active runtime values
2. sets the `LATCHED` flag
3. calls the node-specific `on_latch_apply` callback
4. stores the command result
5. increments `UPDATE_CNT`
6. sets `DATA_READY`

### Acceptance conditions

`LATCH_APPLY` is accepted only when the node is:

- not BUSY
- not in ESTOP

### Rejection results

| Condition | Result |
|---|---|
| Node is BUSY | `CMDRES_REJECT_BUSY` |
| Node is in ESTOP | `CMDRES_REJECT_ESTOP` |

---

## 3. RUN Start

The host starts an operation by changing `CMD_RUN` from 0 to 1.

The common core checks the request before invoking node-specific code.

### Acceptance conditions

RUN start is accepted only when:

- the node is not BUSY
- the node is not in ESTOP
- active parameters have been latched
- the node-specific `on_run_start` callback returns `true`

### Rejection results

| Condition | Result |
|---|---|
| Node is already BUSY | `CMDRES_REJECT_BUSY` |
| Node is in ESTOP | `CMDRES_REJECT_ESTOP` |
| Parameters are not latched | `CMDRES_REJECT_UNLATCHED` |
| Node-specific callback rejects start | `CMDRES_REJECT_STATE` |

When accepted:

- previous completion flags are cleared
- the node-specific start callback runs
- the node enters BUSY
- the command result becomes `CMDRES_OK`

---

## 4. Execution

While BUSY, the common runtime calls the node-specific `tick_10ms` callback.

The node-specific implementation may:

- update motor output
- move a servo
- inspect a sensor
- evaluate a target condition
- publish feedback values
- finish the operation

The common runtime itself does not implement motor, servo, or sensor behavior.

---

## 5. Feedback Publication

Node-specific code may publish three 16-bit feedback values through the common API.

Publishing feedback:

- updates `FB_VALUE0`
- updates `FB_VALUE1`
- updates `FB_VALUE2`
- increments `UPDATE_CNT`
- sets `DATA_READY`

The host may use the update counter and data-ready flag to detect fresh information.

`DATA_READY` is cleared by writing 1 to its bit in the `STATUS` register.

---

## 6. Normal Completion

The current public completion API supports target completion.

When node-specific code calls the target-completion function, the common core:

- sets `DONE`
- sets `DONE_BY_TARGET`
- clears BUSY
- sets READY
- clears `CMD_RUN`
- increments `UPDATE_CNT`
- sets `DATA_READY`

The host can inspect `COMPLETE_FLAGS` to determine why the operation ended.

---

## 7. Host-Requested Stop

A RUN transition from 1 to 0 requests a stop.

If the node is BUSY, the common core:

1. calls `on_run_stop`
2. marks the operation as stopped
3. sets `DONE`
4. sets `DONE_BY_ABORT`
5. clears RUN
6. returns the node to READY
7. updates `UPDATE_CNT` and `DATA_READY`

If the node is already idle, the request is accepted as a no-op.

---

## 8. E-STOP

The common core monitors:

- shared E-STOP bus input
- optional local E-STOP input

When E-STOP is latched:

- the source is recorded
- the node enters ESTOP
- RUN is cleared
- the operation is marked `DONE_BY_ABORT`
- `on_estop_enter` is called
- `UPDATE_CNT` is incremented
- `DATA_READY` is set

### Reset conditions

An E-STOP reset is accepted only when:

- the node is in ESTOP
- the node is not BUSY
- all physical E-STOP inputs are released

Software cannot reset an E-STOP while the physical input remains active.

---

## Node State Representation

| Logical state | Register representation |
|---|---|
| Idle | `STATUS_READY` |
| Running | `STATUS_BUSY` |
| Emergency stop | `STATUS_ESTOP` |
| Active parameters applied | `COMPLETE_FLAGS.LATCHED` |

`LATCHED` is not a separate STATUS state. It indicates that staged parameters have been copied into active runtime values.

---

## Command Result Reporting

The common core records:

- `LAST_CMD`
- `LAST_CMD_RESULT`

Current result values include:

| Value | Meaning |
|---|---|
| `CMDRES_OK` | Command completed or was accepted |
| `CMDRES_REJECT_BUSY` | Rejected because the node is BUSY |
| `CMDRES_REJECT_UNLATCHED` | RUN rejected because parameters are not latched |
| `CMDRES_REJECT_ESTOP` | Rejected because the node is in ESTOP |
| `CMDRES_REJECT_STATE` | Rejected because the current state or node logic does not allow it |
| `CMDRES_ACCEPTED_NOOP` | Accepted, but no state change was required |

---

## Current Scope

The register map includes fields for additional completion reasons and immediate commands.

However, the current public firmware API exposes only the completion paths implemented by the present node code.

The source code remains authoritative where older spreadsheets describe APIs that are not present in the current implementation.

---

## Authoritative Sources

- Register and bit definitions → [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h)
- Command handling → [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c)
- Node callback contract → [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h)

---

## Related Documents

- System Architecture → [`README.md`](./README.md)
- Layer Responsibilities → [`Layer_Responsibilities.md`](./Layer_Responsibilities.md)
- Monitoring Data Flow → [`Monitoring_Data_Flow.md`](./Monitoring_Data_Flow.md)
- Register Map → [`../Register_Map/`](../Register_Map/)
