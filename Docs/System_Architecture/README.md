# System Architecture

This directory documents the system-level architecture of the Modular Factory System.

It focuses on responsibilities, interfaces, control flow, monitoring data flow, and the boundaries between hardware, firmware, host software, storage, and visualization.

---

## Overall Architecture

```text
Physical mechanism
        ↓
Hardware boards
        ↓
Raspberry Pi Pico firmware nodes
        ↓ I²C register interface
Raspberry Pi 5 host software
        ↓
SQLite logs
        ↓
Grafana visualization
```

The system is intentionally divided into layers so that physical mechanisms, low-level control, orchestration, logging, and visualization can evolve independently.

---

## Layer Responsibilities

| Layer | Main responsibility | Typical components |
|---|---|---|
| Physical mechanism | Perform the actual tabletop process | Conveyor, gate, clamp, rack-and-pinion mechanism, paper feed |
| Hardware | Connect controllers, actuators, sensors, and power lines | Controller Board, motor board, servo board, sensor board, power monitor board |
| Firmware | Convert common register commands into device-specific behavior | Shared node core, motor node, servo node, sensor node |
| Host software | Coordinate nodes, monitor status, and record events | Raspberry Pi 5 orchestration scripts, I2C Debugger |
| Storage | Persist node states, events, sessions, and power measurements | SQLite |
| Visualization | Present recorded state and power data | Grafana |

Detailed responsibility boundaries are described in [`Layer_Responsibilities.md`](./Layer_Responsibilities.md).

---

## Control Flow

Commands originate from host-side software and move downward through the system.

```text
Host-side orchestration
        ↓
Write command and parameter registers
        ↓ I²C
Common firmware core
        ↓
Validate command and update node state
        ↓
Invoke node-specific callback
        ↓
Drive actuator or read sensor
        ↓
Publish feedback and completion state
        ↓ I²C
Host-side monitoring
```

The common firmware core controls when device-specific logic may run.

A typical operation follows this sequence:

```text
Write parameters
→ Apply staged values with LATCH_APPLY
→ Start operation with RUN
→ Node enters BUSY
→ Node-specific firmware performs the operation
→ Firmware publishes feedback and completion result
→ Node returns to READY
```

Detailed command handling and rejection conditions are described in [`Control_Flow.md`](./Control_Flow.md).

The common register specification is available in [`../Register_Map/`](../Register_Map/).

---

## Monitoring Data Flow

Monitoring data flows separately from control responsibilities.

```text
Firmware node state / feedback
        ↓ I²C polling
I2C Debugger
        ↓
SQLite database
        ↓
Grafana dashboard
```

The I2C Debugger records:

- node state transitions
- periodic node snapshots
- monitoring sessions
- event logs
- voltage, current, and power measurements

Grafana reads the SQLite database and visualizes the recorded data.

Grafana does not issue hardware control commands.

Detailed storage and visualization flow is described in [`Monitoring_Data_Flow.md`](./Monitoring_Data_Flow.md).

---

## Main Components

| Component | Role |
|---|---|
| Raspberry Pi 5 | Host-side orchestration, monitoring, and logging |
| Raspberry Pi Pico node | Local device control through a shared register interface |
| Actuator / sensor board | Electrical interface to the physical mechanism |
| Power Monitor Board | Power distribution and branch voltage/current monitoring |
| I2C Debugger | Node monitoring, state decoding, and SQLite logging |
| SQLite | Persistent event, snapshot, session, and power data storage |
| Grafana | Visualization and analysis of stored monitoring data |

---

## Architectural Boundaries

### Hardware and Firmware

Hardware defines the electrical interface. Firmware defines how a node interprets commands and controls the connected device.

### Firmware and Host Software

Firmware exposes a common register interface. Host software does not directly depend on the internal actuator implementation.

### Host Software and Grafana

Host software performs monitoring and logging. Grafana reads stored data only and is not part of the control loop.

### Common Core and Node-Specific Firmware

The common firmware core defines lifecycle, state handling, command validation, register behavior, and E-STOP handling.

Node-specific firmware defines the device behavior through callbacks and the public core API.

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Register addresses and bit definitions | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Command handling and state transitions | [`../../Firmware/common/core/node_core.c`](../../Firmware/common/core/node_core.c) |
| Node callback contract | [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h) |
| Node profile definitions | [`../../Firmware/common/platform/node_profile.h`](../../Firmware/common/platform/node_profile.h) |
| Monitoring application structure | [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/) |
| Database and Grafana flow | [`../../Software/`](../../Software/) |

---

## Known Constraints and Extension Points

The current implementation is optimized for tabletop prototyping and demonstration.

Known constraints and possible extensions are documented in:

- [`../Constraints_and_Extension_Points.md`](../Constraints_and_Extension_Points.md)

---

## Related Documents

- Documentation index → [`../README.md`](../README.md)
- Register map → [`../Register_Map/`](../Register_Map/)
- Hardware overview → [`../../Hardware/`](../../Hardware/)
- Firmware overview → [`../../Firmware/`](../../Firmware/)
- Software overview → [`../../Software/`](../../Software/)
- Use cases → [`../../Use_cases/`](../../Use_cases/)
