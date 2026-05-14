# Software

This directory contains host-side software for monitoring, logging, and visualizing the Modular Factory System.

The software layer connects the physical hardware and firmware nodes to time-series logs and dashboards.

---

## Purpose

The software layer is responsible for:

- monitoring I²C-connected hardware nodes
- displaying node status in a desktop UI
- recording state transitions and snapshots
- collecting power monitor values
- storing logs in SQLite
- providing data for Grafana dashboards
- supporting hardware debugging and tabletop demonstrations

---

## Software Components

| Component | Role | Description |
|---|---|---|
| `I2C_Debugger/` | Monitoring application | PyQt-based monitor for I²C nodes and power ports |
| `Grafana/` | Visualization notes | SQL queries and dashboard notes for SQLite log visualization |

---

## Data Flow

```text
Hardware nodes
      ↓ I²C
Monitoring software
      ↓
SQLite database
      ↓
Grafana dashboard
```

The monitoring software records both node states and power measurements.

This makes it possible to inspect:

- node state transitions
- detected errors or warnings
- active monitoring sessions
- branch current / voltage / power values
- relationship between physical motion and electrical load

---

## Related Documents

- Hardware overview → [`../Hardware/`](../Hardware/)
- Firmware overview → [`../Firmware/`](../Firmware/)
- Use cases → [`../Use_cases/`](../Use_cases/)
- Japanese overview → [`../日本語版/`](../日本語版/) 
