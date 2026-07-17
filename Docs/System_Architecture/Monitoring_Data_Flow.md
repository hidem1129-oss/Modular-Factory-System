# Monitoring Data Flow

This document describes how node state, event, session, and power-monitoring data move from the physical system into SQLite and Grafana.

Monitoring is intentionally separated from hardware control.

---

## Monitoring Path

```text
Firmware nodes
        ↓ I²C polling
I2C Debugger
        ↓
SQLite database
        ↓
Grafana dashboard
```

The I2C Debugger is the bridge between the live hardware state and persistent monitoring data.

Grafana reads stored data and does not control hardware.

---

## Data Sources

The monitoring software receives two main categories of data.

### Node data

Examples:

- I²C address
- node presence
- READY, BUSY, WARN, ERROR, and ESTOP state
- signal loss
- command and status values
- state transitions
- periodic snapshots
- monitoring session information

### Power data

Power-monitoring data is collected from INA219 sensors through TCA9548A I²C multiplexers.

Examples:

- port ID
- TCA address
- TCA channel
- INA219 address
- bus voltage
- shunt value
- current
- power
- session ID

---

## I2C Debugger Responsibilities

The I2C Debugger:

- polls configured node addresses
- reads common status and feedback registers
- decodes node states
- detects state changes
- distinguishes a missing device from a previously seen device with lost communication
- collects power-monitoring values
- displays current information in a PyQt5 desktop UI
- records data in SQLite

The current default node range is:

```text
0x10 to 0x19
```

This range is statically configured in the current prototype.

---

## Live State Interpretation

The monitoring UI displays states such as:

| Display state | Meaning |
|---|---|
| READY | Node is available for a new operation |
| BUSY | Node is executing an operation |
| WARN | Node reports a warning |
| ERROR | Node reports an error |
| E-STOP | Emergency stop is active |
| Signal Lost | The node was previously detected but communication was lost |
| No Device | No node is detected at the address |
| UNKNOWN | Temporary or undefined state |

The monitoring layer interprets register values but does not decide low-level actuator behavior.

---

## SQLite Storage

The default database path is:

```text
/var/lib/i2c_debugger/i2c_debugger_events.sqlite3
```

The current application creates tables including:

| Table | Purpose |
|---|---|
| `event_logs` | State change and event records |
| `node_snapshots` | Periodic node-state snapshots |
| `run_sessions` | Operation-related run records |
| `state_segments` | Time segments for node states |
| `monitor_sessions` | Monitoring application sessions |
| `power_port_snapshots` | Voltage, current, and power measurements |

The application is the writer for these tables.

Grafana reads the same database for visualization.

---

## Event and Snapshot Flow

```text
Node register values
        ↓
Polling model
        ↓
State decoding
        ↓
Change detection
        ↓
Event log
```

In parallel:

```text
Node register values
        ↓
Periodic sampling
        ↓
Node snapshot
```

This allows the system to preserve both:

- discrete changes
- periodic state observations

---

## Power Monitoring Flow

```text
Power branch
        ↓
INA219
        ↓
TCA9548A I²C multiplexer
        ↓
Power monitoring service
        ↓
Raw-value conversion
        ↓
UI and SQLite
        ↓
Grafana
```

This makes it possible to compare physical operation with electrical load.

Examples include:

- current increase while a motor is active
- power differences between branches
- maximum and average power by port
- voltage, current, and power trends over time

---

## Grafana Responsibilities

Grafana visualizes stored monitoring data.

Current dashboard content includes:

- I²C address state timeline
- latest events
- ERROR and E-STOP events
- recent sessions
- voltage, current, and power for selected ports
- average and maximum power rankings
- current and power statistics

Grafana is not used for:

- sending I²C commands
- starting or stopping actuators
- resetting E-STOP
- making real-time safety decisions

---

## Separation from the Control Loop

The control path and monitoring path are related but distinct.

### Control path

```text
Host orchestration
→ I²C commands
→ Firmware
→ Actuator or sensor behavior
```

### Monitoring path

```text
Firmware state and power values
→ I2C Debugger
→ SQLite
→ Grafana
```

This separation means the visualization layer can be changed or temporarily unavailable without becoming part of the local device-control logic.

---

## Reader Modes

The I2C Debugger supports:

| Mode | Purpose |
|---|---|
| `real` | Read actual hardware through SMBus |
| `mock` | Simulate node and power data for UI and logic testing |

Mock mode supports development without connected physical hardware.

It does not validate the electrical behavior of the actual I²C bus, sensors, actuators, or power-monitoring hardware.

---

## Current Constraints

- node addresses are monitored over a fixed configured range
- power port mapping is configuration-based
- SQLite is used as the current storage backend
- Grafana dashboards depend on the present database schema
- dynamic node discovery and automatic dashboard generation are not implemented
- monitoring is intended for tabletop prototyping rather than hard real-time control

---

## Authoritative Sources

- I2C Debugger overview → [`../../Software/I2C_Debugger/`](../../Software/I2C_Debugger/)
- Grafana overview → [`../../Software/Grafana/`](../../Software/Grafana/)
- Node status definitions → [`../../Firmware/common/include/node_core.h`](../../Firmware/common/include/node_core.h)

---

## Related Documents

- System Architecture → [`README.md`](./README.md)
- Layer Responsibilities → [`Layer_Responsibilities.md`](./Layer_Responsibilities.md)
- Control Flow → [`Control_Flow.md`](./Control_Flow.md)
- Software overview → [`../../Software/`](../../Software/)
