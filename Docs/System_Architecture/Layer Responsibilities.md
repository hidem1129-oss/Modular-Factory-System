# Layer Responsibilities

This document defines the responsibility of each architectural layer in the Modular Factory System and clarifies what each layer should and should not do.

---

## Responsibility Overview

| Layer | Responsible for | Not responsible for |
|---|---|---|
| Physical mechanism | Material movement, clamping, pressing, sorting, and other physical operations | Communication protocols, state logging, dashboard rendering |
| Hardware | Electrical connection, power distribution, signal routing, actuator and sensor interfaces | Process orchestration, application workflows, dashboard logic |
| Firmware | Local device control, command validation, state handling, feedback publication, and E-STOP response | Cross-node process planning, persistent logging, Grafana rendering |
| Host software | Process orchestration, I²C polling, node monitoring, event recording, and session management | Direct low-level actuator driving, electrical protection |
| Storage | Persistent event, snapshot, session, and power-monitoring records | Hardware control and real-time decision making |
| Visualization | Human-readable timelines, tables, and power trends | Hardware control, command generation, safety handling |

---

## Physical Mechanism Layer

The physical mechanism performs the visible process.

Examples include:

- conveyor movement
- servo-driven sorting gates
- paper feeding
- clamping
- rack-and-pinion press motion
- workpiece detection positions

The mechanism is intentionally simple and accessible for tabletop prototyping.

Its purpose is to provide a physical model that makes control behavior and monitoring results observable.

---

## Hardware Layer

The hardware layer provides the electrical foundation of the system.

Primary responsibilities:

- connect Raspberry Pi Pico nodes to actuator and sensor boards
- distribute power to multiple modules
- expose I²C, control, sensor, and power connections
- support repeated assembly, replacement, and maintenance
- monitor branch voltage and current
- provide physical interfaces to motors, servos, and sensors

Typical components:

- Controller Board
- DC Motor Board
- Servo Board
- Sensor Board
- Power Monitor Board
- Pi 5 Wiring Auxiliary

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
- driving a motor, servo, or sensor interface
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

Orchestration scripts coordinate multiple nodes to perform a process.

Examples:

- start a conveyor
- wait for workpiece detection
- capture and classify an image
- command a sorting gate
- feed and clamp paper
- execute a press sequence

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

A failure of the visualization layer should not directly change actuator behavior.

---

## Visualization Layer

Grafana reads the SQLite database and visualizes recorded information.

Typical views include:

- node state timelines
- recent events
- ERROR and E-STOP events
- monitoring sessions
- voltage, current, and power trends
- average and maximum power comparisons

Grafana does not send control commands to nodes.

This separation prevents the dashboard layer from becoming part of the actuator control path.

See [`../../Software/Grafana/`](../../Software/Grafana/).

---

## Cross-Layer Interfaces

| Boundary | Interface |
|---|---|
| Physical mechanism ↔ Hardware | Mechanical mounting, actuator shafts, sensor positions, wiring |
| Hardware ↔ Firmware | GPIO, PWM, I²C, power, actuator and sensor signals |
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

This allows one layer to change without requiring every other layer to be rewritten.

For example, the same monitoring and logging software can be reused when the physical process changes from sorting to stamping.

---

## Related Documents

- System Architecture → [`README.md`](./README.md)
- Control Flow → [`Control_Flow.md`](./Control_Flow.md)
- Monitoring Data Flow → [`Monitoring_Data_Flow.md`](./Monitoring_Data_Flow.md)
- Register Map → [`../Register_Map/`](../Register_Map/)
- Known Constraints and Extension Points → [`../Constraints_and_Extension_Points.md`](../Constraints_and_Extension_Points.md)
