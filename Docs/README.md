# Docs

This directory contains the cross-cutting technical documentation for the Modular Factory System.

It explains how the hardware, firmware, software, monitoring, and visualization layers work together as one system.

---

## Role of this Directory

The implementation files remain in their respective directories:

- [`Hardware/`](../Hardware/) — PCB design, schematics, Gerber data, BOMs, and manufacturing information
- [`Firmware/`](../Firmware/) — Raspberry Pi Pico firmware and shared node runtime
- [`Software/`](../Software/) — host-side monitoring, logging, and visualization software
- [`Use_cases/`](../Use_cases/) — example processes built with the system

`Docs/` contains specifications and design information that span more than one implementation layer.

---

## Documentation Index

| Topic | Description |
|---|---|
| [`System_Architecture/`](./System_Architecture/) | Overall system structure, layer responsibilities, control flow, and monitoring data flow |
| [`Register_Map/`](./Register_Map/) | Common I²C register map, status bits, command bits, completion flags, and access rules |
| [`Constraints_and_Extension_Points.md`](./Constraints_and_Extension_Points.md) | Known constraints, implementation boundaries, and possible extension points |

---

## System Boundaries

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

Each layer has a distinct responsibility:

- **Hardware** connects actuators, sensors, power lines, and controllers.
- **Firmware** converts common register commands into device-specific behavior.
- **Host software** orchestrates nodes, monitors state, and records events.
- **SQLite** stores event, snapshot, session, and power-monitoring data.
- **Grafana** visualizes recorded data and does not control hardware.

---

## Source of Truth

The Markdown documents in this directory provide human-readable specifications.

Where implementation details differ from older spreadsheets or notes, the current source code is authoritative:

| Information | Authoritative source |
|---|---|
| Register addresses and bit definitions | [`Firmware/common/include/node_core.h`](../Firmware/common/include/node_core.h) |
| Register write behavior and state transitions | [`Firmware/common/core/node_core.c`](../Firmware/common/core/node_core.c) |
| Public firmware API and callbacks | [`Firmware/common/include/node_core.h`](../Firmware/common/include/node_core.h) |
| Node profile definitions | [`Firmware/common/platform/node_profile.h`](../Firmware/common/platform/node_profile.h) |
| Build profile selection | [`Firmware/common/platform/node_build_config.h`](../Firmware/common/platform/node_build_config.h) |

The spreadsheet in [`Register_Map/`](./Register_Map/) is retained as a supplementary design reference.

---

## Related Entry Points

- Project overview → [`../README.md`](../README.md)
- Hardware overview → [`../Hardware/`](../Hardware/)
- Firmware overview → [`../Firmware/`](../Firmware/)
- Software overview → [`../Software/`](../Software/)
- Use cases → [`../Use_cases/`](../Use_cases/)
