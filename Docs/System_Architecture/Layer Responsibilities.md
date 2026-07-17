# Layer Responsibilities

This document defines the responsibility of each architectural layer in the Modular Factory System and clarifies what each layer should and should not do.

---

## Responsibility Overview

| Layer | Responsible for | Not responsible for |
|---|---|---|
| Physical mechanism | Performing the physical process represented by the prototype | Communication protocols, state logging, dashboard rendering |
| Hardware | Electrical connection, power distribution, signal routing, and actuator or sensor interfacing | Process orchestration, application workflows, dashboard logic |
| Firmware | Local device control, command validation, state handling, feedback publication, and E-STOP response | Cross-node process planning, persistent logging, visualization |
| Host software | Process orchestration, I²C polling, node monitoring, event recording, and session management | Direct low-level output generation and electrical protection |
| Storage | Persistent event, snapshot, session, and power-monitoring records | Hardware control and real-time decision making |
| Visualization | Human-readable timelines, tables, and power trends | Hardware control, command generation, and safety handling |

---

## Physical Mechanism Layer

The physical mechanism represents the real-world process being prototyped.

Its responsibilities are to:

- convert actuator output into observable physical behavior
- provide measurable or detectable process states
- expose mechanical constraints that affect control behavior
- make the relationship between commands, movement, sensing, and monitoring visible

The mechanism is replaceable.

The architecture does not depend on a specific process, machine type, or demonstration configuration.

---

## Hardware Layer

The hardware layer provides the electrical foundation of the system.

Primary responsibilities:

- connect Raspberry Pi Pico nodes to actuator and sensor interfaces
- distribute power to multiple modules
- expose I²C, control, sensor, and power connections
- support repeated assembly, replacement, and maintenance
- monitor branch voltage and current
- provide standardized connection points between modules

Typical components include:

- Controller Board
- actuator interface boards
- sensor interface boards
- Power Monitor Board
- Raspberry Pi 5 wiring support

The hardware layer does not decide the process sequence. It provides the interfaces required by firmware and host software.

See [`../../Hardware/`](../../Hardware/).

---

## Firmware Layer

The firmware layer runs on Raspberry Pi Pico nodes.

It is divided into:

```text
common firmware core
        +
node-specific implementation
```

### Common firmware core

The common core is responsible for:

- shared I²C register behavior
- command validation
- READY, BUSY, and ESTOP handling
- staged parameter writes
- LATCH_APPLY processing
- RUN start and stop behavior
- completion flags
- DATA_READY and UPDATE_CNT handling
- E-STOP latching and reset conditions
- invocation of node-specific callbacks

### Node-specific firmware

Node-specific firmware is responsible for:

- interpreting active setpoints and operation modes
- controlling the connected device
- acquiring device-specific input
- publishing device-specific feedback
- reporting operation completion
- stopping output safely
- reacting to E-STOP entry

Node-specific code should use the public API in [`../../Firmware/common/include/`](../../Firmware/common/include/) and should not access core internals directly.

See [`../../Firmware/`](../../Firmware/).

---

## Host Software Layer

The host software runs primarily on Raspberry Pi 5.

It has two related responsibilities.

### Orchestration

Orchestration logic coordinates multiple nodes to perform a process.

Its responsibilities include:

- issuing commands to nodes
- applying parameters
- waiting for state transitions or completion
- evaluating process conditions
- coordinating dependencies between nodes
- handling process-level sequencing

The host software defines the workflow, while the individual nodes remain responsible for local device behavior.

### Monitoring and logging

The I2C Debugger:

- polls configured I²C node addresses
- decodes status and command values
- displays node states
- records transitions and snapshots
- collects power measurements
- stores data in SQLite

Host software communicates with nodes through the common register interface. It should not depend on device-specific internal firmware implementation.

See [`../../Software/`](../../Software/).

---

## Storage Layer

SQLite stores monitoring data produced by the host software.

The current implementation records data such as:

- event logs
- node snapshots
- run sessions
- state segments
- monitor sessions
- power port snapshots

SQLite is part of the monitoring path, not the hardware control loop.

A failure of the storage or visualization layer should not directly change local actuator behavior.

---

## Visualization Layer

Grafana reads the SQLite database and visualizes recorded information.

Typical views include:

- node state timelines
- recent events
- warning, error, and E-STOP events
- monitoring sessions
- voltage, current, and power trends
- comparisons between monitored branches

Grafana does not send control commands to nodes.

This separation prevents the dashboard layer from becoming part of the actuator control path.

See [`../../Software/Grafana/`](../../Software/Grafana/).

---

## Cross-Layer Interfaces

| Boundary | Interface |
|---|---|
| Physical mechanism ↔ Hardware | Mechanical mounting, actuator coupling, sensor placement, and wiring |
| Hardware ↔ Firmware | GPIO, PWM, I²C, power, actuator signals, and sensor signals |
| Firmware ↔ Host software | Common I²C register interface |
| Host software ↔ Storage | SQLite writes and queries |
| Storage ↔ Visualization | Grafana SQLite data source and SQL queries |

---

## Design Principle

The system separates:

- physical behavior
- electrical interfacing
- local device control
- multi-node orchestration
- persistent monitoring
- visualization

This allows the physical process or mechanism to change while the shared control, monitoring, logging, and visualization layers remain reusable.

---

## Related Documents

- System Architecture → [`README.md`](./README.md)
- Control Flow → [`Control_Flow.md`](./Control_Flow.md)
- Monitoring Data Flow → [`Monitoring_Data_Flow.md`](./Monitoring_Data_Flow.md)
- Register Map → [`../Register_Map/`](../Register_Map/)
- Known Constraints and Extension Points → [`../Constraints_and_Extension_Points.md`](../Constraints_and_Extension_Points.md)
