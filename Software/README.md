# Software

This directory contains the host-side software used to observe, record, and visualize the Modular Factory System.

The software layer connects the physical hardware and distributed firmware nodes to operator-facing monitoring tools and persistent system logs.

It is responsible for observing the system and presenting its condition.

Device-local control remains in firmware. Process-level orchestration,
where required, is kept separate from the monitoring and visualization
components documented here.

---

## Overview

The current software layer provides:

* real-time monitoring of I²C-connected nodes
* interpretation of register values into operator-facing states
* display of node and power conditions
* recording of state transitions and periodic snapshots
* session-based logging in SQLite
* historical visualization through Grafana
* mock operation for testing without connected hardware

The software is primarily designed for tabletop prototyping, debugging, demonstrations, and architecture validation.

It is not intended to replace an industrial PLC, SCADA platform, or safety controller.

---

## Software Structure

| Directory                          | Responsibility                                                                                     |
| ---------------------------------- | -------------------------------------------------------------------------------------------------- |
| [`I2C_Debugger/`](./I2C_Debugger/) | Real-time monitoring, state interpretation, power monitoring, session handling, and SQLite logging |
| [`Grafana/`](./Grafana/)           | Historical visualization and analysis of data recorded in SQLite                                   |

The current structure separates immediate system observation from historical analysis.

```text
Firmware nodes and power monitors
                ↓
             I²C bus
                ↓
         I2C Debugger
        ├─ live status UI
        ├─ state interpretation
        ├─ event detection
        └─ SQLite logging
                ↓
        SQLite database
                ↓
             Grafana
        └─ historical analysis
```

---

## Software Responsibilities

The software layer owns:

* polling host-accessible hardware state
* interpreting raw register values for human operators
* distinguishing operational states such as READY, BUSY, WARN, ERROR, and ESTOP
* detecting changes in observed node state
* recording events, snapshots, sessions, and power measurements
* displaying the current condition of the tabletop system
* providing persisted data for later analysis

The software layer does not own:

* low-level actuator timing
* device-local safety shutdown
* electrical protection
* authoritative firmware lifecycle state
* hardware emergency-stop behavior
* direct implementation of motor, servo, or sensor behavior

These responsibilities remain in the hardware and firmware layers.

---

## Component Responsibilities

### I2C Debugger

The I2C Debugger is the real-time observation layer.

It owns:

* communication with I²C nodes
* collection of node register values
* collection of branch and main-line power values
* conversion of raw protocol values into display states
* identification of missing and previously lost nodes
* display of the current system condition
* detection and recording of state transitions
* creation and management of monitoring sessions
* persistence of observations in SQLite

The debugger observes node behavior but does not define the node's device-local control logic.

Detailed application documentation:

* [`I2C_Debugger/README.md`](./I2C_Debugger/README.md)

### Grafana

Grafana is the historical visualization layer.

It owns:

* visualization of recorded node states
* visualization of power measurements
* comparison of monitoring sessions
* inspection of state duration and transition history
* trend and dashboard-level analysis

Grafana does not communicate directly with hardware and is not part of the control path.

It reads data that has already been recorded by the monitoring application.

---

## Boundary with Firmware

Firmware is the authoritative source of device-local state and behavior.

Each firmware node owns:

* local GPIO and peripheral operation
* actuator or sensor behavior
* command execution
* local parameter validation
* feedback generation
* completion decisions
* safe local shutdown
* E-STOP handling

The software layer reads the state exposed by firmware and converts it into information suitable for monitoring and analysis.

```text
Host command or orchestration
              ↓
          Firmware node
    ├─ validates command
    ├─ controls local device
    ├─ manages local lifecycle
    └─ exposes status and feedback
              ↓
       Monitoring software
    ├─ observes current state
    ├─ detects state changes
    └─ records observations
```

This boundary allows device-local behavior to continue independently from the desktop monitoring UI.

Firmware architecture:

* [`../Firmware/`](../Firmware/)

---

## Runtime Data Flow

### Current-state monitoring

```text
I²C registers
      ↓
Register reader
      ↓
Node and power models
      ↓
State interpretation
      ↓
PyQt monitoring UI
```

This path is used when an operator needs to inspect the current condition of the physical system.

### Persistent observation

```text
Node states and power values
              ↓
       Event and snapshot logger
              ↓
          SQLite database
```

The database preserves information that would otherwise disappear when the UI changes or closes.

### Historical visualization

```text
SQLite database
       ↓
    Grafana
       ↓
Dashboards and historical analysis
```

This path is intentionally separate from real-time hardware access.

---

## Observation Model

The software records more than the latest displayed value.

The current observation model includes:

* monitoring sessions
* node snapshots
* state-transition events
* state-duration segments
* power-port snapshots
* reader mode and polling configuration

This supports both immediate debugging and later investigation.

For example, a user can inspect not only whether a node is currently in an error state, but also:

* when the error started
* which state preceded it
* whether communication was previously established
* how long the condition continued
* whether power behavior changed during the same period

---

## Real and Mock Operation

The monitoring application supports two reader modes.

| Mode   | Purpose                                                         |
| ------ | --------------------------------------------------------------- |
| `real` | Reads actual I²C nodes and power-monitor devices                |
| `mock` | Simulates node states, communication failures, and power values |

Mock mode allows UI, state interpretation, logging, and failure handling to be tested without requiring the complete physical system.

The mock reader is treated as a replaceable data source rather than a separate application architecture.

---

## Runtime Characteristics

The current software runtime:

* is primarily deployed on a Raspberry Pi 5
* uses Python and PyQt5 for the desktop monitoring application
* accesses the I²C bus through SMBus
* stores monitoring data in SQLite
* supports configurable polling intervals
* supports both real and simulated register readers
* provides data for external Grafana dashboards
* is designed for a single tabletop system rather than distributed production deployment

The monitoring UI is intended for operational visibility and debugging, not hard real-time control.

---

## Design Intent

The software architecture separates:

* observation from device control
* current-state display from historical analysis
* raw hardware access from state interpretation
* monitoring logic from visualization tools
* physical hardware access from mock testing

This makes it possible to:

* test software behavior without the complete hardware system
* modify dashboards without changing firmware
* preserve observations when the UI is closed
* investigate events after a demonstration or test run
* distinguish monitoring failures from device-control failures
* add future software components without requiring direct hardware access

The detailed reasoning behind system-wide responsibility boundaries and technology choices is documented separately in the Architecture Decision Log.

---

## Current Scope and Limitations

The current implementation is optimized for a tabletop prototype.

It currently assumes:

* a local Raspberry Pi host
* a known I²C address range
* a single SQLite database
* a single monitoring application instance
* local dashboard access
* no hard real-time guarantees
* no redundant monitoring path
* no production-grade authentication or access control

These constraints are intentional for the current proof-of-concept scope.

---

## Related Documentation

* I2C Debugger → [`./I2C_Debugger/`](./I2C_Debugger/)
* Firmware overview → [`../Firmware/`](../Firmware/)
* Hardware overview → [`../Hardware/`](../Hardware/)
* System architecture → [`../Docs/System_Architecture/`](../Docs/System_Architecture/)
* Register map → [`../Docs/Register_Map/`](../Docs/Register_Map/)
* Use cases → [`../Use_cases/`](../Use_cases/)
* Japanese overview → [`../日本語版/`](../日本語版/)
* Architecture Decision Log → [`../ADL/`](../ADL/)
