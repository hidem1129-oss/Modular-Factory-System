# Register Map

This directory contains the human-readable specification for the register-based I²C interface used by Raspberry Pi Pico firmware nodes in the Modular Factory System.

The register interface is shared across motor, servo, sensor, and future node types.

---

## Purpose

The register map provides a common host-side interface for:

- node identification
- capability discovery
- lifecycle state
- command results
- E-STOP status
- completion flags
- feedback values
- active parameters
- staged command parameters
- RUN, STOP, LATCH_APPLY, and reset requests

The host software can use the same access model without depending on the internal implementation of each actuator or sensor.

---

## Register Window

The current common register window is:

```text
0x00 to 0x3F
```

| Address range | Purpose |
|---|---|
| `0x00–0x0F` | Identification, revision, and capability information |
| `0x10–0x1F` | Status, command result, completion, E-STOP, and update information |
| `0x20–0x2F` | Feedback values and configuration-related fields |
| `0x30–0x3F` | Command flags, operation mode, setpoints, timing, and limits |

---

## Access Model

### Read access

The host reads identification, status, completion, feedback, and active values through I²C.

### Write access

The host writes command and parameter registers through I²C.

Some status bits use write-one-to-clear behavior.

### 16-bit values

Sixteen-bit values are transferred in this order:

```text
High byte
Low byte
```

The common firmware stages both bytes and commits the value when the low byte is written.

When a staged parameter changes:

- `LATCHED` is cleared
- `UPDATE_CNT` is incremented
- `DATA_READY` is set

The host must issue `LATCH_APPLY` before starting an operation with the new parameters.

---

## Typical Command Sequence

```text
1. Write operation parameters
2. Send LATCH_APPLY
3. Confirm command result
4. Set RUN
5. Confirm BUSY
6. Read feedback and completion
7. Confirm return to READY
```

Detailed behavior:

- [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)
- [`../System_Architecture/Control_Flow.md`](../System_Architecture/Control_Flow.md)

---

## Register Categories

### Identification and capabilities

Typical information includes:

- signature
- module type
- module ID
- hardware revision
- firmware revision
- capability bits

### Status and command result

Typical information includes:

- READY
- BUSY
- WARN
- ERROR
- ESTOP
- DATA_READY
- last command
- last command result
- update counter

### Completion and E-STOP

These registers report:

- whether active parameters are latched
- whether an operation has completed
- the completion reason
- E-STOP state and source

### Feedback

The common interface provides three 16-bit feedback values:

- `FB_VALUE0`
- `FB_VALUE1`
- `FB_VALUE2`

Their exact meaning is defined by the selected node profile.

### Command and parameter fields

The command region includes fields such as:

- command flags
- operation mode
- setpoint values
- target time
- hold time
- limits

Not every node type uses every field.

---

## DATA_READY and UPDATE_CNT

`DATA_READY` indicates that relevant state, feedback, completion, or parameter information has changed.

The current implementation clears this bit using write-one-to-clear behavior in the `STATUS` register.

`UPDATE_CNT` increments when relevant common state or published data changes.

These are update indicators, not lifecycle states.

---

## Command Results

| Result | Meaning |
|---|---|
| `CMDRES_OK` | Command accepted or completed successfully |
| `CMDRES_REJECT_BUSY` | Rejected because the node is BUSY |
| `CMDRES_REJECT_UNLATCHED` | RUN rejected because active parameters are not latched |
| `CMDRES_REJECT_ESTOP` | Rejected because E-STOP is active |
| `CMDRES_REJECT_STATE` | Rejected by current state or node-specific validation |
| `CMDRES_ACCEPTED_NOOP` | Accepted, but no state change was required |

This lets the host distinguish a rejected request from an I²C transport failure.

---

## Source of Truth

The Markdown files in this directory are human-readable reference material.

The current firmware source is authoritative for implemented behavior.

| Topic | Authoritative source |
|---|---|
| Register addresses and bit definitions | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Register write behavior | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| State transitions and command validation | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| Public node API | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Node profile and capability definitions | [`../../Firmware/common/platform/node_profile.h`](../../Firmware/common/platform/node_profile.h) |

Where older spreadsheets or notes differ from the current source code, the source code takes precedence.

---

## Files

| File | Description |
|---|---|
| [`bit_pattern_mapping.xlsx`](./bit_pattern_mapping.xlsx) | Supplementary spreadsheet containing earlier register and bit-pattern design information |

The spreadsheet is retained as a design reference and should not be treated as the authoritative definition of the current public firmware API.

---

## References

| Document | Purpose |
|---|---|
| `Common_Register_Map.md` | Register-by-register address, width, access, reset value, and meaning |
| `Status_and_Completion.md` | STATUS, command result, completion, E-STOP, DATA_READY, and UPDATE_CNT |
| `Command_and_Setpoint.md` | Command flags, operation modes, setpoints, timing, limits, and write sequence |

These files should be checked against the current implementation rather than copied directly from the spreadsheet.

---

## Related Documents

- Documentation index → [`../README.md`](../README.md)
- System architecture → [`../System_Architecture/`](../System_Architecture/)
- Control flow → [`../System_Architecture/Control_Flow.md`](../System_Architecture/Control_Flow.md)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Firmware state model → [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)
- Node Extension API → [`../../Firmware/common/docs/Node_Extension_API.md`](../../Firmware/common/docs/Node_Extension_API.md)
- I2C Debugger → [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/)
