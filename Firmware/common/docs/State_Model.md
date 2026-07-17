# State Model

This document describes the common lifecycle and state representation used by Raspberry Pi Pico firmware nodes in the Modular Factory System.

The common core owns the lifecycle state. Node-specific firmware participates through callbacks and the public core API.

---

## State Overview

The common runtime uses three primary lifecycle states:

```text
READY
BUSY
ESTOP
```

These states are represented through bits in the `STATUS` register.

A separate `LATCHED` flag indicates whether staged parameters have been applied to the active runtime configuration.

`LATCHED` is not a fourth lifecycle state.

---

## State Representation

| Logical condition | Register representation | Meaning |
|---|---|---|
| Ready | `STATUS_READY` | The node is idle and available for a new operation |
| Busy | `STATUS_BUSY` | The node is executing an operation |
| Emergency stop | `STATUS_ESTOP` | E-STOP is latched |
| Parameters applied | `COMPLETE_FLAGS.LATCHED` | Staged values have been copied into active runtime values |

The common core is responsible for maintaining these values consistently.

Node-specific code should not manipulate the lifecycle bits directly.

---

## Simplified State Diagram

```text
                  parameter write
READY + LATCHED --------------------> READY + not LATCHED
      ^                                      |
      |                                      | LATCH_APPLY
      |                                      v
      +------------------------------ READY + LATCHED
                                             |
                                             | RUN = 1
                                             v
                                           BUSY
                                             |
                      normal completion -----+---- host stop
                                             |
                                             v
                                           READY

Any active state
      |
      | physical or bus E-STOP
      v
    ESTOP
      |
      | reset request after all E-STOP inputs are released
      v
    READY
```

---

## Startup State

After common-core initialization, the node starts in an idle condition.

The expected logical state is:

```text
READY = 1
BUSY  = 0
ESTOP = 0
RUN   = 0
```

Node-specific initialization is performed through the `init` callback.

E-STOP inputs are handled with startup filtering, arming, and debounce logic before normal latching behavior begins.

---

## Parameter Staging

Several multi-byte parameters are written into staging storage before they become active.

Examples include:

- setpoint values
- target time
- hold time
- limits

For 16-bit values, the host writes:

```text
High byte
Low byte
```

The low-byte write commits the complete staged value.

When a staged value changes, the common core:

- clears `LATCHED`
- increments `UPDATE_CNT`
- sets `DATA_READY`

This ensures that a previous active configuration cannot be mistaken for the newly written parameter set.

---

## LATCH_APPLY

`LATCH_APPLY` copies staged values into the active runtime configuration.

### Accepted when

- the node is not BUSY
- the node is not in ESTOP

### On acceptance

The common core:

1. copies staged values into active values
2. sets `LATCHED`
3. invokes `on_latch_apply`
4. records a successful command result
5. increments `UPDATE_CNT`
6. sets `DATA_READY`

### Rejected when

| Condition | Result |
|---|---|
| BUSY | `CMDRES_REJECT_BUSY` |
| ESTOP | `CMDRES_REJECT_ESTOP` |

Applying parameters does not start the operation.

---

## RUN Start

A RUN transition from 0 to 1 requests operation start.

### Accepted when

- the node is not already BUSY
- the node is not in ESTOP
- `LATCHED` is set
- `on_run_start` returns `true`

### On acceptance

The common core:

- clears previous completion state as required
- invokes the node-specific start callback
- sets BUSY
- clears READY
- records `CMDRES_OK`

### Rejected when

| Condition | Result |
|---|---|
| Already BUSY | `CMDRES_REJECT_BUSY` |
| ESTOP active | `CMDRES_REJECT_ESTOP` |
| Parameters not latched | `CMDRES_REJECT_UNLATCHED` |
| Node-specific start rejected | `CMDRES_REJECT_STATE` |

The common core validates lifecycle conditions before allowing node-specific execution to begin.

---

## BUSY State

While BUSY, the node-specific implementation performs the operation through its periodic callback.

Typical activities include:

- updating actuator output
- reading sensor input
- checking elapsed time
- checking target conditions
- publishing feedback
- deciding when the operation is complete

The common runtime calls `tick_10ms` from the cooperative main loop.

The period is approximate and is not a hard real-time guarantee.

---

## Normal Completion

When node-specific code reports target completion, the common core transitions back to READY.

The completion path updates:

- `DONE`
- `DONE_BY_TARGET`
- BUSY
- READY
- RUN
- `UPDATE_CNT`
- `DATA_READY`

The logical transition is:

```text
BUSY
  ↓ target completion
READY
```

The host can inspect completion flags to determine why the operation ended.

---

## Host-Requested Stop

A RUN transition from 1 to 0 requests a stop.

If the node is BUSY, the common core:

1. invokes `on_run_stop`
2. records aborted completion
3. clears BUSY
4. sets READY
5. clears RUN
6. increments `UPDATE_CNT`
7. sets `DATA_READY`

The logical transition is:

```text
BUSY
  ↓ RUN = 0
READY + DONE_BY_ABORT
```

If the node is already idle, the stop request is accepted as a no-op.

---

## E-STOP Entry

The common core monitors:

- shared E-STOP bus input
- local E-STOP input

When an E-STOP condition is latched, the common core:

- records the E-STOP source
- clears RUN
- marks an active operation as aborted
- enters ESTOP
- invokes `on_estop_enter`
- increments `UPDATE_CNT`
- sets `DATA_READY`

The node-specific callback is responsible for putting the connected device into the safest available local state.

The logical transition is:

```text
READY or BUSY
      ↓ E-STOP
    ESTOP
```

---

## E-STOP Reset

A software reset request cannot clear E-STOP while a physical E-STOP input remains active.

Reset is accepted only when:

- the node is currently in ESTOP
- the node is not BUSY
- all monitored E-STOP inputs are released

After a valid reset, the node returns to READY.

```text
ESTOP
  ↓ inputs released + reset accepted
READY
```

The common core, not node-specific code, owns this transition.

---

## Completion Flags

Completion information is separate from the primary lifecycle state.

Current relevant flags include:

| Flag | Meaning |
|---|---|
| `LATCHED` | Staged parameters have been applied |
| `DONE` | An operation has ended |
| `DONE_BY_TARGET` | The target condition was reached |
| `DONE_BY_ABORT` | The operation was stopped or aborted |

The register map may reserve additional completion reasons.

The current implementation and public API remain authoritative for which completion paths are actually available.

---

## DATA_READY and UPDATE_CNT

The common core uses two mechanisms to indicate updated information.

### `UPDATE_CNT`

`UPDATE_CNT` increments when relevant common state or published data changes.

Examples include:

- staged parameter change
- parameter application
- feedback publication
- completion
- stop
- E-STOP entry

### `DATA_READY`

`DATA_READY` is set when updated information is available to the host.

The bit is cleared using write-one-to-clear behavior in the `STATUS` register.

These mechanisms are update indicators. They are not lifecycle states.

---

## Command Result State

The common core records the most recent command and result separately from READY, BUSY, and ESTOP.

Current result values include:

| Result | Meaning |
|---|---|
| `CMDRES_OK` | Command accepted or completed successfully |
| `CMDRES_REJECT_BUSY` | Rejected because the node is BUSY |
| `CMDRES_REJECT_UNLATCHED` | RUN rejected because active parameters are not latched |
| `CMDRES_REJECT_ESTOP` | Rejected because E-STOP is active |
| `CMDRES_REJECT_STATE` | Rejected by current state or node-specific validation |
| `CMDRES_ACCEPTED_NOOP` | Accepted, but no state change was required |

Command results help the host distinguish a rejected request from a transport failure.

---

## Ownership Rules

### Common core owns

- READY, BUSY, and ESTOP
- RUN lifecycle handling
- `LATCHED`
- completion flags
- command result fields
- `UPDATE_CNT`
- `DATA_READY`
- E-STOP latching and reset conditions

### Node-specific firmware owns

- actuator or sensor behavior
- local device state
- device-specific validation
- feedback values
- deciding when a target is reached
- safe device shutdown in callbacks

Node-specific code reports events through the public API rather than editing common state directly.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| State and bit definitions | [`../include/node_core.h`](../include/node_core.h) |
| State transitions and command handling | [`../core/node_core.c`](../core/node_core.c) |
| Callback contract | [`../include/node_core.h`](../include/node_core.h) |
| Runtime scheduling | [`../platform/main.c`](../platform/main.c) |

---

## Related Documents

- Common firmware overview → [`../README.md`](../README.md)
- Node Extension API → [`Node_Extension_API.md`](./Node_Extension_API.md)
- Adding a new node → [`Adding_New_Node.md`](./Adding_New_Node.md)
- System control flow → [`../../../Docs/System_Architecture/Control_Flow.md`](../../../Docs/System_Architecture/Control_Flow.md)
- Register map → [`../../../Docs/Register_Map/`](../../../Docs/Register_Map/)
