# Command and Setpoint

This document describes the command flags, operation mode, staged setpoints, timing fields, limits, and command-write behavior used by the common Raspberry Pi Pico firmware core.

It complements:

- [`Common_Register_Map.md`](./Common_Register_Map.md)
- [`Status_and_Completion.md`](./Status_and_Completion.md)
- [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)

---

## Scope

The main registers covered here are:

| Address | Register |
|---|---|
| `0x1D` | `OP_MODE` |
| `0x30` | `CMD_FLAGS` |
| `0x31–0x32` | `SETPOINT0` |
| `0x33–0x34` | `SETPOINT1` |
| `0x35–0x36` | `TARGET_TIME` |
| `0x37–0x38` | `HOLD_TIME` |
| `0x39` | `CMD_IMMEDIATE` |
| `0x3A` | `CMD_PARAM0` |
| `0x3B` | `CMD_PARAM1` |
| `0x3C–0x3D` | `LIMIT0` |
| `0x3E–0x3F` | `LIMIT1` |

---

## Command Model

The command interface combines:

- level-controlled command bits
- one-shot request bits
- staged 16-bit parameters
- operation-mode selection
- node-specific validation

The common core owns lifecycle validation.

Node-specific firmware owns device-specific interpretation and validation.

---

## CMD_FLAGS (`0x30`)

`CMD_FLAGS` contains both level bits and one-shot request bits.

| Bit | Mask | Symbol | Type | Meaning |
|---:|---:|---|---|---|
| 0 | `0x01` | `CMD_RUN` | Level | Request or maintain operation |
| 1 | `0x02` | `CMD_DIR` | Level | Direction or profile-defined binary option |
| 2 | `0x04` | `CMD_MODE` | Level | Profile-defined binary mode flag |
| 3 | `0x08` | `CMD_ESTOP_RESET` | One-shot | Request E-STOP reset |
| 4 | `0x10` | `CMD_LATCH_APPLY` | One-shot | Copy staged parameters into active runtime state |
| 5 | `0x20` | `CMD_USE_SOFTLIMIT` | Level | Enable profile-defined software-limit behavior |
| 6–7 | — | Reserved | — | Not currently defined |

---

## Level Bits

The current level-mask contains:

```text
CMD_RUN
CMD_DIR
CMD_MODE
CMD_USE_SOFTLIMIT
```

These bits remain represented in `CMD_FLAGS` after a write.

The current core stores the active copies of:

- `CMD_DIR`
- `CMD_MODE`
- `CMD_USE_SOFTLIMIT`

when `LATCH_APPLY` is accepted.

`CMD_RUN` is handled as the lifecycle request itself rather than copied into the active parameter set.

---

## One-Shot Bits

The current one-shot mask contains:

```text
CMD_ESTOP_RESET
CMD_LATCH_APPLY
```

These bits are consumed during command handling and then cleared from `CMD_FLAGS`.

They should be treated as requests, not persistent states.

---

## Preserving Level Bits During One-Shot Writes

The current command handler includes a compatibility behavior.

When a write contains:

- no level bits
- one or more one-shot bits

the previous level-bit state is preserved.

Example:

```text
current CMD_FLAGS = RUN | DIR
write CMD_LATCH_APPLY only
```

The handler keeps the prior RUN and DIR level values while processing the one-shot request.

If a write includes explicit level bits, those level bits replace the previous level state.

This means host software can either:

- write one-shot bits alone and rely on level preservation
- write the complete intended level mask together with the one-shot request

The second approach is usually clearer for explicit host-side state management.

---

## Command Processing Order

A single `CMD_FLAGS` write is processed in this order:

```text
1. E-STOP reset request
2. LATCH_APPLY request
3. RUN rising or falling edge
```

If multiple logical commands are included in one write:

- more than one action may be processed
- `LAST_CMD` and `LAST_CMD_RESULT` may reflect the later action
- `UPDATE_CNT` may increment more than once

For precise command attribution, send one logical one-shot request per write.

---

## RUN Rising Edge

A RUN rising edge occurs when:

```text
old RUN = 0
new RUN = 1
```

The common core accepts the start only when:

- the node is not BUSY
- E-STOP is not active
- `LATCHED` is set
- the node-specific `on_run_start` callback returns `true`

### Accepted result

```text
STATUS_BUSY = 1
STATUS_READY = 0
LAST_CMD = CMD_RUN
LAST_CMD_RESULT = CMDRES_OK
```

Previous completion-reason bits are cleared before the node-specific start callback is evaluated.

### Rejection results

| Condition | Result |
|---|---|
| Already BUSY | `CMDRES_REJECT_BUSY` |
| E-STOP active | `CMDRES_REJECT_ESTOP` |
| Parameters not latched | `CMDRES_REJECT_UNLATCHED` |
| Node-specific validation rejects start | `CMDRES_REJECT_STATE` |

When node-specific validation rejects the start:

- READY is restored
- RUN is cleared
- previous completion-reason bits remain cleared

---

## RUN Falling Edge

A RUN falling edge occurs when:

```text
old RUN = 1
new RUN = 0
```

### While BUSY

The common core:

1. calls `on_run_stop`
2. reports stopped completion
3. clears BUSY
4. sets READY
5. clears RUN
6. sets `DONE` and `DONE_BY_ABORT`
7. records `CMDRES_OK`

### While already idle

The request is accepted as a no-op:

```text
LAST_CMD_RESULT = CMDRES_ACCEPTED_NOOP
```

No completion flags are added.

---

## LATCH_APPLY

`CMD_LATCH_APPLY` copies staged and current configuration values into active runtime variables.

Accepted only when:

- the node is not BUSY
- the node is not in E-STOP

On acceptance, the core copies:

- `SETPOINT0`
- `SETPOINT1`
- `TARGET_TIME`
- `HOLD_TIME`
- `LIMIT0`
- `LIMIT1`
- `OP_MODE`
- level bits `DIR`, `MODE`, and `USE_SOFTLIMIT`

Then it:

- sets `LATCHED`
- calls `on_latch_apply`
- records `CMDRES_OK`
- increments `UPDATE_CNT`
- sets `DATA_READY`

Rejected results:

| Condition | Result |
|---|---|
| BUSY | `CMDRES_REJECT_BUSY` |
| E-STOP active | `CMDRES_REJECT_ESTOP` |

Applying parameters does not start the operation.

---

## E-STOP Reset Request

`CMD_ESTOP_RESET` requests software clearing of the E-STOP latch.

The request succeeds only when:

- E-STOP is currently latched
- the node is not BUSY
- all monitored physical E-STOP inputs are released

Possible results:

| Condition | Result |
|---|---|
| Valid latched E-STOP and all inputs released | `CMDRES_OK` |
| BUSY | `CMDRES_REJECT_BUSY` |
| Physical E-STOP input still active | `CMDRES_REJECT_STATE` |
| E-STOP not currently latched | `CMDRES_REJECT_STATE` |

The reset request does not itself clear completion flags.

---

## OP_MODE (`0x1D`)

`OP_MODE` is a common one-byte enumeration.

Current values are:

| Value | Symbol | Meaning |
|---:|---|---|
| `0x00` | `OP_MODE_DEFAULT` | Profile-defined default behavior |
| `0x01` | `OP_MODE_SPEED` | Speed-oriented behavior |
| `0x02` | `OP_MODE_TIME` | Time-oriented behavior |
| `0x10` | `OP_MODE_CONTINUOUS` | Continuous operation |
| `0x11` | `OP_MODE_ONE_SHOT` | One-shot operation |

The numeric meaning is shared, but actual support and parameter interpretation are defined by each node profile.

Unsupported modes should be rejected by node-specific `on_run_start` logic.

Fallback behavior should not be assumed unless explicitly documented by the profile.

---

## OP_MODE Activation

Writing `OP_MODE` changes the register value immediately.

The active runtime copy changes only when `LATCH_APPLY` is accepted.

Important current behavior:

- changing `OP_MODE` does not automatically clear `LATCHED`
- a host can therefore change `OP_MODE` while `LATCHED` remains set
- the new mode is not active until the next accepted `LATCH_APPLY`

Host software should write the complete intended configuration before applying the latch.

---

## 16-bit Parameter Encoding

The staged 16-bit fields use big-endian register order.

Example for `SETPOINT0`:

```text
SETPOINT0 = (SETPOINT0_H << 8) | SETPOINT0_L
```

Write sequence:

```text
1. Write high byte
2. Write low byte
3. Low-byte write commits the complete staged value
```

If the committed value differs from the current register value, the core:

- clears `LATCHED`
- increments `UPDATE_CNT`
- sets `DATA_READY`

Writing only the high byte does not commit the public value.

---

## SETPOINT0 (`0x31–0x32`)

| Address | Register |
|---|---|
| `0x31` | `SETPOINT0_H` |
| `0x32` | `SETPOINT0_L` |

`SETPOINT0` is the primary 16-bit command value.

The current common public API exposes it through:

```c
core_get_setpoint0()
```

Its unit and meaning depend on the selected node profile and operation mode.

Examples may include speed, position, duty, count, or another normalized target, but the common core does not impose one physical interpretation.

---

## SETPOINT1 (`0x33–0x34`)

| Address | Register |
|---|---|
| `0x33` | `SETPOINT1_H` |
| `0x34` | `SETPOINT1_L` |

`SETPOINT1` is a secondary staged 16-bit command value.

Current common behavior:

- staging and latch application are implemented
- an internal active copy exists
- no public getter is currently exposed in `node_core.h`

Therefore node-specific firmware cannot access it through the supported public extension API without additional common-core work.

It should be treated as reserved for future public use.

---

## TARGET_TIME (`0x35–0x36`)

| Address | Register |
|---|---|
| `0x35` | `TARGET_TIME_H` |
| `0x36` | `TARGET_TIME_L` |

`TARGET_TIME` is a staged 16-bit time-related command value.

The current public API exposes it through:

```c
core_get_target_time()
```

The common core does not define the physical unit.

The selected node profile must define whether the value represents milliseconds, 10 ms ticks, seconds, or another unit.

---

## HOLD_TIME (`0x37–0x38`)

| Address | Register |
|---|---|
| `0x37` | `HOLD_TIME_H` |
| `0x38` | `HOLD_TIME_L` |

`HOLD_TIME` is a staged 16-bit auxiliary timing value.

Current common behavior:

- staging is implemented
- active-copy update is implemented
- no public getter is exposed

It is therefore present in the common map but not currently available through the supported node-extension API.

---

## LIMIT0 (`0x3C–0x3D`)

| Address | Register |
|---|---|
| `0x3C` | `LIMIT0_H` |
| `0x3D` | `LIMIT0_L` |

`LIMIT0` is a staged 16-bit profile-defined limit.

The current public API exposes it through:

```c
core_get_limit0()
```

The selected node implementation defines whether this is a position limit, speed limit, timeout bound, threshold, or another constraint.

---

## LIMIT1 (`0x3E–0x3F`)

| Address | Register |
|---|---|
| `0x3E` | `LIMIT1_H` |
| `0x3F` | `LIMIT1_L` |

`LIMIT1` is a second staged 16-bit profile-defined limit.

The current public API exposes it through:

```c
core_get_limit1()
```

Its meaning is profile-specific.

---

## CMD_IMMEDIATE (`0x39`)

`CMD_IMMEDIATE` reserves a byte for immediate-command dispatch.

Current implementation status:

- register address exists
- generic read and write storage exists
- no immediate-command dispatcher exists
- no command values are defined in the common public API
- writing it does not invoke a node callback

It should be treated as reserved.

---

## CMD_PARAM0 and CMD_PARAM1 (`0x3A–0x3B`)

These fields reserve two one-byte parameters for immediate or auxiliary commands.

Current implementation status:

- backing storage exists
- generic read and write behavior exists
- no public immediate-command contract uses them

They should not be treated as implemented command parameters until a dispatcher and ownership contract are added.

---

## Recommended Host Write Sequence

A clear host-side configuration sequence is:

```text
1. Confirm node is READY and not ESTOP
2. Write OP_MODE
3. Write SETPOINT0 high byte
4. Write SETPOINT0 low byte
5. Write any additional supported staged values
6. Write desired DIR, MODE, and USE_SOFTLIMIT level bits
7. Send LATCH_APPLY
8. Read LAST_CMD and LAST_CMD_RESULT
9. Confirm LATCHED
10. Set RUN while preserving intended level bits
11. Confirm CMDRES_OK and BUSY
```

When the node finishes:

```text
1. Detect DATA_READY or UPDATE_CNT change
2. Read STATUS and COMPLETE_FLAGS
3. Read feedback values
4. Confirm READY
5. Clear DATA_READY
```

---

## Example Configuration

Example only:

```text
OP_MODE = OP_MODE_SPEED
SETPOINT0 = 0x012C
LIMIT0 = 0x03E8
DIR = 1
USE_SOFTLIMIT = 1
```

Possible write flow:

```text
write 0x1D = 0x01
write 0x31 = 0x01
write 0x32 = 0x2C
write 0x3C = 0x03
write 0x3D = 0xE8
write CMD_FLAGS = DIR | USE_SOFTLIMIT | LATCH_APPLY
read LAST_CMD_RESULT
write CMD_FLAGS = RUN | DIR | USE_SOFTLIMIT
```

The actual physical meaning of `0x012C` and `0x03E8` depends on the selected profile.

---

## Command Result Verification

After a command write, the host should verify:

| Register | Purpose |
|---|---|
| `LAST_CMD` | Confirms which common command was recorded |
| `LAST_CMD_RESULT` | Confirms whether it was accepted |
| `STATUS` | Confirms the resulting lifecycle state |
| `COMPLETE_FLAGS` | Confirms `LATCHED` or completion state |
| `UPDATE_CNT` | Confirms relevant state changed |

I²C acknowledgement alone confirms transport, not semantic command acceptance.

---

## Current Limitations and Extension Points

- `SETPOINT1` has no public getter
- `HOLD_TIME` has no public getter
- active DIR, MODE, and USE_SOFTLIMIT values have no public getters
- `CMD_IMMEDIATE` has no dispatcher
- `CMD_PARAM0/1` have no public command contract
- physical units are not encoded by the common core
- unsupported-mode rejection is node-specific
- `OP_MODE` changes do not clear `LATCHED`
- level-bit changes do not clear `LATCHED`
- multiple command requests in one write can obscure `LAST_CMD` attribution
- nominal access rules are not fully enforced by the generic write handler

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Register and command-bit definitions | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Command processing and staged writes | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| Operation-mode definitions | [`../../Firmware/common/platform/node_profile.h`](../../Firmware/common/platform/node_profile.h) |
| Node-extension getters and callbacks | [`../../Firmware/common/docs/Node_Extension_API.md`](../../Firmware/common/docs/Node_Extension_API.md) |
| Lifecycle and rejection behavior | [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md) |

---

## Related Documents

- Register map index → [`README.md`](./README.md)
- Complete register table → [`Common_Register_Map.md`](./Common_Register_Map.md)
- Status and completion → [`Status_and_Completion.md`](./Status_and_Completion.md)
- Firmware state model → [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)
- Node Extension API → [`../../Firmware/common/docs/Node_Extension_API.md`](../../Firmware/common/docs/Node_Extension_API.md)
- System control flow → [`../System_Architecture/Control_Flow.md`](../System_Architecture/Control_Flow.md)
