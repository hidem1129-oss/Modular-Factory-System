# Common Register Map

This document lists the current common I²C register window used by Raspberry Pi Pico firmware nodes in the Modular Factory System.

It is a browser-readable reference derived from the current firmware implementation and the supplementary bit-pattern mapping spreadsheet.

---

## Scope

The common register window occupies:

```text
0x00–0x3F
```

The I²C register index wraps within this 64-byte window.

The register interface is shared across node profiles, while the interpretation of operation modes, setpoints, feedback values, and profile-extension fields may differ by node type.

---

## Access Legend

| Marking | Meaning |
|---|---|
| `RO` | Intended for host read access |
| `RW` | Host-readable and host-writable |
| `W1C` | Write one to clear the specified bit |
| `staged` | High and low bytes are collected before the value is committed |
| `commit` | Writing the low byte commits the staged 16-bit value |
| `staged-active` | Register value becomes part of the active runtime configuration after `LATCH_APPLY` |
| `Profile-defined` | Meaning or default depends on the selected node profile |
| `Reserved` | Address exists in the window but has no supported common behavior |

### Current enforcement note

The access labels describe the intended public interface.

The current generic I²C write handler does not enforce read-only protection for every nominally `RO` register. Except for specially handled addresses such as `STATUS`, `CMD_FLAGS`, and staged 16-bit fields, an incoming write may update the backing register array.

Host software should follow the documented access rules. Firmware-side access enforcement remains an extension point.

---

## Byte Order and 16-bit Commit Behavior

Common 16-bit values use big-endian register order:

```text
High byte at the lower address
Low byte at the next address
```

For staged command parameters:

1. writing the high byte updates temporary staging storage
2. writing the low byte commits both bytes into the public register window
3. if the committed value changed, the core clears `LATCHED`
4. the core increments `UPDATE_CNT`
5. the core sets `DATA_READY`

The following values use this staged commit behavior:

- `SETPOINT0`
- `SETPOINT1`
- `TARGET_TIME`
- `HOLD_TIME`
- `LIMIT0`
- `LIMIT1`

`OP_MODE` is stored directly in the register window and copied into active runtime state when `LATCH_APPLY` is accepted.

---

## Complete Register Table

| Address | Name | Access | Width | Reset/default | Description |
|---|---|---:|---:|---|---|
| `0x00` | `SIGNATURE0` | RO | 8 | 0xC0 | Protocol signature byte 0. |
| `0x01` | `SIGNATURE1` | RO | 8 | 0xDE | Protocol signature byte 1. |
| `0x02` | `MODULE_TYPE` | RO | 8 | Profile-defined | Node module category. |
| `0x03` | `MODULE_ID_L` | RO | 8 | 0x00 | Module ID, low byte. Currently not populated by the common core. |
| `0x04` | `MODULE_ID_H` | RO | 8 | 0x00 | Module ID, high byte. Currently not populated by the common core. |
| `0x05` | `HW_REV` | RO | 8 | Profile-defined | Hardware revision reported by the selected profile. |
| `0x06` | `FW_VER` | RO | 8 | Profile-defined | Firmware revision reported by the selected profile. |
| `0x07` | `CAP_FLAGS0` | RO | 8 | Profile-defined | Primary capability flags. |
| `0x08` | `PROFILE_ID` | RO | 8 | Profile-defined | Selected node interpretation profile. |
| `0x09` | `CAP_FLAGS1` | RO | 8 | Profile-defined | Extended capability flags. |
| `0x0A` | `UNIT_FLAGS0` | RO | 8 | 0x00 | Unit, sign, or resolution metadata. Reserved by current profiles. |
| `0x0B` | `UNIT_FLAGS1` | RO | 8 | 0x00 | Extended unit metadata. Reserved by current profiles. |
| `0x0C` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x0D` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x0E` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x0F` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x10` | `STATUS` | RO / W1C | 8 | READY | Lifecycle and update flags. Writing DATA_READY=1 clears DATA_READY. |
| `0x11` | `ERROR_CODE` | RO | 8 | 0x00 | Error code. No public setter is implemented in the current common API. |
| `0x12` | `WARN_CODE` | RO | 8 | 0x00 | Warning code. No public setter is implemented in the current common API. |
| `0x13` | `UPDATE_CNT_H` | RO | 8 | 0x00 | Update counter, high byte. |
| `0x14` | `UPDATE_CNT_L` | RO | 8 | 0x00 | Update counter, low byte. |
| `0x15` | `ESTOP_SOURCE` | RO | 8 | 0x00 | Latched E-STOP source bits. |
| `0x16` | `STATE_FLAGS0` | RO | 8 | 0x00 | Profile-specific state flags. Unused by the current common core. |
| `0x17` | `STATE_FLAGS1` | RO | 8 | 0x00 | Extended profile-specific state flags. Unused by the current common core. |
| `0x18` | `LAST_CMD` | RO | 8 | 0x00 | Most recently handled common command bit. |
| `0x19` | `LAST_CMD_RESULT` | RO | 8 | 0x00 | Result of the most recently handled common command. |
| `0x1A` | `FB_SEQ_H` | RO | 8 | 0x00 | Feedback sequence, high byte. Reserved but not incremented by current implementation. |
| `0x1B` | `FB_SEQ_L` | RO | 8 | 0x00 | Feedback sequence, low byte. Reserved but not incremented by current implementation. |
| `0x1C` | `COMPLETE_FLAGS` | RO | 8 | 0x00 | Completion reason and LATCHED flag. |
| `0x1D` | `OP_MODE` | RW / staged-active | 8 | 0x00 | Common operation-mode value copied to active state by LATCH_APPLY. |
| `0x1E` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x1F` | `RESERVED` | — | 8 | 0x00 | Reserved. |
| `0x20` | `FB_VALUE0_H` | RO | 8 | 0x00 | Feedback value 0, high byte. |
| `0x21` | `FB_VALUE0_L` | RO | 8 | 0x00 | Feedback value 0, low byte. |
| `0x22` | `FB_VALUE1_H` | RO | 8 | 0x00 | Feedback value 1, high byte. |
| `0x23` | `FB_VALUE1_L` | RO | 8 | 0x00 | Feedback value 1, low byte. |
| `0x24` | `FB_VALUE2_H` | RO | 8 | 0x00 | Feedback value 2, high byte. |
| `0x25` | `FB_VALUE2_L` | RO | 8 | 0x00 | Feedback value 2, low byte. |
| `0x26` | `FB_FLAGS0` | RO | 8 | 0x00 | Profile-specific feedback flags. No public setter is implemented. |
| `0x27` | `FB_FLAGS1` | RO | 8 | 0x00 | Extended feedback flags. No public setter is implemented. |
| `0x28` | `CFG_VALUE0_H` | RW | 8 | 0x00 | Auxiliary configuration value 0, high byte. Not staged by current core. |
| `0x29` | `CFG_VALUE0_L` | RW | 8 | 0x00 | Auxiliary configuration value 0, low byte. Not staged by current core. |
| `0x2A` | `CFG_VALUE1_H` | RW | 8 | 0x00 | Auxiliary configuration value 1, high byte. Not staged by current core. |
| `0x2B` | `CFG_VALUE1_L` | RW | 8 | 0x00 | Auxiliary configuration value 1, low byte. Not staged by current core. |
| `0x2C` | `CFG_FLAGS0` | RW | 8 | 0x00 | Auxiliary configuration flags. |
| `0x2D` | `CFG_FLAGS1` | RW | 8 | 0x00 | Extended auxiliary configuration flags. |
| `0x2E` | `EXT_VALUE0` | Profile-defined | 8 | 0x00 | Profile extension byte 0. |
| `0x2F` | `EXT_VALUE1` | Profile-defined | 8 | 0x00 | Profile extension byte 1. |
| `0x30` | `CMD_FLAGS` | RW | 8 | 0x00 | RUN and level flags plus one-shot LATCH_APPLY and ESTOP_RESET requests. |
| `0x31` | `SETPOINT0_H` | RW / staged | 8 | 0x00 | Setpoint 0, high byte. |
| `0x32` | `SETPOINT0_L` | RW / commit | 8 | 0x00 | Setpoint 0, low byte. Writing commits the 16-bit staged value. |
| `0x33` | `SETPOINT1_H` | RW / staged | 8 | 0x00 | Setpoint 1, high byte. |
| `0x34` | `SETPOINT1_L` | RW / commit | 8 | 0x00 | Setpoint 1, low byte. Writing commits the 16-bit staged value. |
| `0x35` | `TARGET_TIME_H` | RW / staged | 8 | 0x00 | Target time, high byte. |
| `0x36` | `TARGET_TIME_L` | RW / commit | 8 | 0x00 | Target time, low byte. Writing commits the 16-bit staged value. |
| `0x37` | `HOLD_TIME_H` | RW / staged | 8 | 0x00 | Hold time, high byte. |
| `0x38` | `HOLD_TIME_L` | RW / commit | 8 | 0x00 | Hold time, low byte. Writing commits the 16-bit staged value. |
| `0x39` | `CMD_IMMEDIATE` | Reserved RW | 8 | 0x00 | Immediate-command field reserved by the map; no dispatcher is implemented. |
| `0x3A` | `CMD_PARAM0` | RW | 8 | 0x00 | Immediate or auxiliary command parameter 0. |
| `0x3B` | `CMD_PARAM1` | RW | 8 | 0x00 | Immediate or auxiliary command parameter 1. |
| `0x3C` | `LIMIT0_H` | RW / staged | 8 | 0x00 | Limit 0, high byte. |
| `0x3D` | `LIMIT0_L` | RW / commit | 8 | 0x00 | Limit 0, low byte. Writing commits the 16-bit staged value. |
| `0x3E` | `LIMIT1_H` | RW / staged | 8 | 0x00 | Limit 1, high byte. |
| `0x3F` | `LIMIT1_L` | RW / commit | 8 | 0x00 | Limit 1, low byte. Writing commits the 16-bit staged value. |

---

## Address Regions

### `0x00–0x0F`: Identification and capabilities

This region identifies the node and selected firmware profile.

Profile-owned values are populated during common-core initialization.

Current capability bits in `CAP_FLAGS0` are:

| Bit | Symbol | Meaning |
|---:|---|---|
| 0 | `NODE_CAP_STAGED_16BIT` | Staged 16-bit parameter interface |
| 1 | `NODE_CAP_RUN_STOP` | RUN and STOP lifecycle |
| 2 | `NODE_CAP_ESTOP` | Common E-STOP handling |
| 3 | `NODE_CAP_FB_U16_3CH` | Three 16-bit feedback channels |
| 4–7 | Reserved | Not currently defined |

Current profile IDs:

| Value | Profile |
|---:|---|
| `0x00` | Dummy |
| `0x01` | Motor Basic |
| `0x02` | Sensor Basic |
| `0x03` | Servo Basic |

Current module types:

| Value | Module type |
|---:|---|
| `0x00` | Generic |
| `0x02` | Motor |
| `0x03` | Sensor |
| `0x04` | Servo |

---

### `0x10–0x1F`: Runtime state and completion

This region contains lifecycle state, command results, E-STOP information, completion flags, update indicators, and the current operation mode.

Bit-level definitions are described in:

- [`Status_and_Completion.md`](./Status_and_Completion.md)

---

### `0x20–0x2F`: Feedback and profile configuration

`FB_VALUE0` through `FB_VALUE2` are published by node-specific firmware through the common API.

Their units and semantics are profile-specific.

The remaining configuration, feedback-flag, and extension fields are reserved for profile use. Several are present in the register window but do not yet have dedicated public common-core APIs.

---

### `0x30–0x3F`: Commands and staged parameters

This region contains:

- common command flags
- setpoints
- target and hold time
- immediate-command placeholders
- limits

Command sequencing and bit-level details are described in:

- [`Command_and_Setpoint.md`](./Command_and_Setpoint.md)

---

## Reset and Profile Defaults

At common-core initialization:

- the full backing register array is cleared to zero
- `SIGNATURE0` becomes `0xC0`
- `SIGNATURE1` becomes `0xDE`
- `STATUS` becomes `READY`
- selected profile metadata is written
- the selected profile may apply additional defaults
- staged and active 16-bit values start at zero
- `UPDATE_CNT` starts at zero

A profile-specific default may therefore override a zero value after the common initialization step.

---

## Sequential I²C Access

The first received byte selects the register index.

Subsequent bytes read from or write to the current index, then automatically advance the index.

The index wraps with a `0x3F` mask:

```text
0x3F → 0x00
```

Host software should avoid accidentally crossing the end of the register window during a multi-byte transaction.

---

## Implemented vs Reserved Fields

The register window intentionally contains fields that are not fully exposed by the current public API.

Examples include:

- `UNIT_FLAGS0/1`
- `STATE_FLAGS0/1`
- `FB_SEQ`
- `FB_FLAGS0/1`
- `CFG_VALUE0/1`
- `CFG_FLAGS0/1`
- `EXT_VALUE0/1`
- `CMD_IMMEDIATE`
- `CMD_PARAM0/1`

Their presence reserves a stable address layout for future profiles and host tooling.

A reserved address should not be treated as implemented merely because it can be read or written in the backing array.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Register addresses and common bit definitions | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Initialization, staging, and write behavior | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| Profile IDs, module types, capabilities, and operation modes | [`../../Firmware/common/platform/node_profile.h`](../../Firmware/common/platform/node_profile.h) |
| Lifecycle behavior | [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md) |

The attached spreadsheet is supplementary design material.

Where this document, the spreadsheet, and the current source differ, the current source code takes precedence.

---

## Related Documents

- Register Map index → [`README.md`](./README.md)
- Status and completion bits → [`Status_and_Completion.md`](./Status_and_Completion.md)
- Commands and setpoints → [`Command_and_Setpoint.md`](./Command_and_Setpoint.md)
- Firmware state model → [`../../Firmware/common/docs/State_Model.md`](../../Firmware/common/docs/State_Model.md)
- Node Extension API → [`../../Firmware/common/docs/Node_Extension_API.md`](../../Firmware/common/docs/Node_Extension_API.md)
- System control flow → [`../System_Architecture/Control_Flow.md`](../System_Architecture/Control_Flow.md)
