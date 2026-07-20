# Status and Completion

This document describes the runtime status, command-result, completion, E-STOP, and update-indicator registers used by the common Raspberry Pi Pico firmware core.

It complements:

- [`Common_Register_Map.md`](./Common_Register_Map.md)
- [`Command_and_Setpoint.md`](./Command_and_Setpoint.md)
- [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)

---

## Scope

| Address | Register |
|---|---|
| `0x10` | `STATUS` |
| `0x11` | `ERROR_CODE` |
| `0x12` | `WARN_CODE` |
| `0x13–0x14` | `UPDATE_CNT` |
| `0x15` | `ESTOP_SOURCE` |
| `0x16–0x17` | `STATE_FLAGS0/1` |
| `0x18` | `LAST_CMD` |
| `0x19` | `LAST_CMD_RESULT` |
| `0x1A–0x1B` | `FB_SEQ` |
| `0x1C` | `COMPLETE_FLAGS` |

`OP_MODE` at `0x1D` is documented with commands and parameters.

---

## STATUS (`0x10`)

| Bit | Mask | Symbol | Meaning |
|---:|---:|---|---|
| 0 | `0x01` | `STATUS_READY` | Node is idle and available |
| 1 | `0x02` | `STATUS_BUSY` | Node is executing an operation |
| 2 | `0x04` | `STATUS_ERROR` | Error condition is active |
| 3 | `0x08` | `STATUS_WARN` | Warning condition is active |
| 4 | `0x10` | `STATUS_ESTOP` | E-STOP is latched |
| 5 | `0x20` | `STATUS_DATA_READY` | Relevant state or data changed |
| 6–7 | — | Reserved | Not currently defined |

Expected lifecycle combinations:

| Condition | READY | BUSY | ESTOP |
|---|---:|---:|---:|
| Idle | 1 | 0 | 0 |
| Running | 0 | 1 | 0 |
| E-STOP latched | 0 | 0 | 1 |

`LATCHED` is stored in `COMPLETE_FLAGS`; it is not a fourth lifecycle state.

At startup:

```text
READY = 1
BUSY = 0
ESTOP = 0
DATA_READY = 0
```

---

## STATUS Write Behavior

`STATUS` is intended primarily for host reads.

The supported host write is write-one-to-clear for `STATUS_DATA_READY`:

```text
write STATUS_DATA_READY = 1
        ↓
clear STATUS_DATA_READY
```

Other status bits should not be written by host software.

---

## ERROR_CODE (`0x11`)

`ERROR_CODE` is reserved for a node error code.

Current behavior:

- reset value is `0x00`
- no dedicated public setter exists
- no common ownership contract is defined for node-specific firmware

The field and `STATUS_ERROR` bit are therefore extension points rather than complete error-reporting behavior.

---

## WARN_CODE (`0x12`)

`WARN_CODE` is reserved for a node warning code.

Current behavior:

- reset value is `0x00`
- no dedicated public setter exists
- no common ownership contract is defined for node-specific firmware

The field and `STATUS_WARN` bit are reserved extension points.

---

## UPDATE_CNT (`0x13–0x14`)

`UPDATE_CNT` is a 16-bit unsigned counter.

```text
UPDATE_CNT = (UPDATE_CNT_H << 8) | UPDATE_CNT_L
```

It starts at zero and wraps naturally after `0xFFFF`.

The common core increments it when relevant shared state or published data changes.

Current examples include:

- a staged parameter commit changes a value
- `LATCH_APPLY` is handled
- RUN start or stop is handled
- E-STOP entry occurs
- E-STOP reset is handled
- feedback is published
- target completion is reported
- stopped or aborted completion is reported
- a generic writable register changes

The number of increments is not guaranteed to equal the number of host commands.

Use it as a change detector, not a command sequence number.

---

## DATA_READY

`STATUS_DATA_READY` is set by the same helper that increments `UPDATE_CNT`.

Typical host use:

```text
1. Read STATUS
2. If DATA_READY = 1, read required registers
3. Read UPDATE_CNT when consistency matters
4. Clear DATA_READY with write-one-to-clear
```

Because multiple updates can occur while the bit remains set, `UPDATE_CNT` should be used when the host must detect intervening changes.

---

## Consistent Multi-Register Reads

The current interface does not provide an atomic snapshot.

A host can check consistency with:

```text
1. Read UPDATE_CNT → count_before
2. Read STATUS, COMPLETE_FLAGS, feedback, and related registers
3. Read UPDATE_CNT → count_after
4. Accept only if count_before == count_after
5. Otherwise retry
```

---

## ESTOP_SOURCE (`0x15`)

| Bit | Mask | Symbol | Meaning |
|---:|---:|---|---|
| 0 | `0x01` | `ESTOP_SRC_BUS` | Shared E-STOP bus input |
| 1 | `0x02` | `ESTOP_SRC_LOCAL` | Local E-STOP input |
| 2–7 | — | Reserved | Not currently defined |

Multiple bits may be set.

If another monitored source becomes active after E-STOP is already latched, its source bit can be added.

---

## E-STOP Entry

When E-STOP is latched, the common core:

1. records the source bits
2. clears READY and BUSY
3. sets ESTOP
4. sets `DONE` and `DONE_BY_ABORT`
5. clears RUN in `CMD_FLAGS`
6. calls `on_estop_enter`
7. increments `UPDATE_CNT`
8. sets `DATA_READY`

---

## E-STOP Reset

Reset succeeds only when:

- E-STOP is currently latched
- the node is not BUSY
- all monitored E-STOP inputs are released

On success:

- the latch is cleared
- `ESTOP_SOURCE` becomes zero
- ESTOP is cleared
- READY is set
- monitoring is re-armed

The current reset path does not clear completion flags automatically.

---

## STATE_FLAGS0/1 (`0x16–0x17`)

These registers are reserved for profile-specific state flags.

Current common behavior:

- reset to zero
- no common bit definitions
- no public setter
- no standard lifecycle semantics

Host software should not infer meaning without profile-specific documentation.

---

## LAST_CMD (`0x18`)

`LAST_CMD` stores the most recently handled common command mask.

Typical values:

| Value | Meaning |
|---:|---|
| `0x01` | `CMD_RUN` |
| `0x08` | `CMD_ESTOP_RESET` |
| `0x10` | `CMD_LATCH_APPLY` |
| `0x00` | No handled command since initialization |

When multiple one-shot requests are included in one write, later command handling can overwrite the earlier `LAST_CMD`.

Send one logical one-shot command per write when precise attribution is required.

---

## LAST_CMD_RESULT (`0x19`)

| Value | Symbol | Meaning |
|---:|---|---|
| `0x00` | `CMDRES_OK` | Command accepted |
| `0x01` | `CMDRES_REJECT_BUSY` | Rejected because BUSY |
| `0x02` | `CMDRES_REJECT_UNLATCHED` | RUN rejected because parameters are not latched |
| `0x03` | `CMDRES_REJECT_ESTOP` | Rejected because E-STOP is active |
| `0x04` | `CMDRES_REJECT_STATE` | Rejected by lifecycle or node-specific validation |
| `0x05` | `CMDRES_ACCEPTED_NOOP` | Accepted with no required state change |

A successful I²C transfer does not mean the requested command was accepted.

---

## FB_SEQ (`0x1A–0x1B`)

`FB_SEQ` reserves a 16-bit feedback sequence counter.

Current behavior:

- reset to zero
- not incremented by `core_publish_fb_u16`
- no public API controls it

`UPDATE_CNT` is the implemented update detector.

---

## COMPLETE_FLAGS (`0x1C`)

| Bit | Mask | Symbol | Meaning |
|---:|---:|---|---|
| 0 | `0x01` | `DONE` | An operation ended or was aborted |
| 1 | `0x02` | `DONE_BY_TIME` | Reserved completion reason |
| 2 | `0x04` | `DONE_BY_TARGET` | Target completion reported |
| 3 | `0x08` | `DONE_BY_CONDITION` | Reserved completion reason |
| 4 | `0x10` | `DONE_BY_ABORT` | Operation stopped or aborted |
| 5 | `0x20` | `LATCHED` | Staged parameters copied into active state |
| 6–7 | — | Reserved | Not currently defined |

Completion reason bits are not inherently mutually exclusive.

---

## Completion Paths

### Target completion

```text
DONE = 1
DONE_BY_TARGET = 1
BUSY = 0
READY = 1
RUN = 0
```

### Host-requested stop while BUSY

```text
DONE = 1
DONE_BY_ABORT = 1
BUSY = 0
READY = 1
RUN = 0
```

### E-STOP entry

```text
DONE = 1
DONE_BY_ABORT = 1
READY = 0
BUSY = 0
ESTOP = 1
RUN = 0
```

### RUN clear while already idle

No completion bits are added.

The result is `CMDRES_ACCEPTED_NOOP`.

---

## Clearing Completion Flags

Before a new RUN start attempt, the common core clears:

- `DONE`
- `DONE_BY_TIME`
- `DONE_BY_TARGET`
- `DONE_BY_CONDITION`
- `DONE_BY_ABORT`

`LATCHED` is preserved.

Current behavior detail:

If `on_run_start` rejects the operation, the previous completion reason has already been cleared.

---

## LATCHED

`LATCHED` means staged command parameters have been copied into active runtime values.

It is set by accepted `LATCH_APPLY`.

It is cleared when a changed staged 16-bit value is committed for:

- `SETPOINT0`
- `SETPOINT1`
- `TARGET_TIME`
- `HOLD_TIME`
- `LIMIT0`
- `LIMIT1`

Current behavior:

- changing `OP_MODE` alone does not clear `LATCHED`
- changing DIR, MODE, or USE_SOFTLIMIT alone does not clear `LATCHED`
- `LATCH_APPLY` copies those current values into active runtime state

Host software should therefore write all required configuration before applying the latch.

---

## Recommended Host Interpretation

### Ready to start

```text
READY = 1
BUSY = 0
ESTOP = 0
LATCHED = 1
```

Node-specific validation may still reject the start.

### Running

```text
BUSY = 1
ESTOP = 0
```

### Target completion

```text
READY = 1
DONE = 1
DONE_BY_TARGET = 1
```

### Stopped or aborted

```text
DONE = 1
DONE_BY_ABORT = 1
```

Read `STATUS_ESTOP` to distinguish E-STOP from a host-requested stop.

---

## Current Limitations and Extension Points

- no common public API for `ERROR_CODE`
- no common public API for `WARN_CODE`
- no common public API for `STATE_FLAGS0/1`
- `FB_SEQ` is not incremented
- no current public completion API for `DONE_BY_TIME`
- no current public completion API for `DONE_BY_CONDITION`
- completion history is not an event log
- multi-register reads are not atomic
- nominal read-only access is not enforced for every register

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Register and bit definitions | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| State transitions and command results | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| E-STOP behavior | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| Lifecycle interpretation | [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md) |

---

## Related Documents

- Register map index → [`README.md`](./README.md)
- Complete register table → [`Common_Register_Map.md`](./Common_Register_Map.md)
- Commands and setpoints → [`Command_and_Setpoint.md`](./Command_and_Setpoint.md)
- Firmware state model → [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)
- System control flow → [`../System_Architecture/Control_Flow.md`](../System_Architecture/Control_Flow.md)
