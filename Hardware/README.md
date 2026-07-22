# Hardware

This directory contains the hardware design files and documentation for the Modular Factory System.

The hardware layer is organized as a set of modular PCBs that connect the Raspberry Pi 5 host, Raspberry Pi Pico-based control nodes, actuator and sensor interfaces, and monitored 5 V power distribution.

---

## Purpose

The hardware layer provides the physical foundation of the system.

It is designed to support:

- distributed I²C-connected control nodes
- replaceable actuator and sensor interface boards
- centralized wiring from the Raspberry Pi 5
- monitored 5 V power distribution
- repeatable tabletop factory demonstrations
- easier maintenance through standardized connectors
- reuse of the same electrical modules across multiple physical processes

The current implementation is intended for educational, prototyping, and proof-of-concept use rather than industrial production deployment.

---

## System-Level Hardware Flow

### Control and signal path

```text
Raspberry Pi 5
      ↓ I²C and host-side wiring
Pi5_Wiring_Auxiliary
      ↓ shared bus and control connections
Controller_Board
      ↓ local control signals
DC_Motor_Board / Servo_Board / Sensor_Board
      ↓
motor / servo / photo-reflector
```

### Power path

```text
5 V source
      ↓
Power_Monitor_Board
      ↓ monitored power branches
controller and peripheral boards
      ↓
physical loads
```

The control path and power path are documented separately because they have different electrical and operational responsibilities.

---

## Board Categories

| Category | Board | Main role |
|---|---|---|
| Host wiring | [`Pi5_Wiring_Auxiliary/`](./Pi5_Wiring_Auxiliary/) | Organizes Raspberry Pi 5-side I²C and module wiring |
| Local controller | [`Controller_Board/`](./Controller_Board/) | Hosts the Raspberry Pi Pico node and exposes local control connections |
| Actuator interface | [`DC_Motor_Board/`](./DC_Motor_Board/) | Drives DC motors |
| Actuator interface | [`Servo_Board/`](./Servo_Board/) | Connects and powers servo actuators |
| Sensor interface | [`Sensor_Board/`](./Sensor_Board/) | Connects the selected photo-reflector circuit and related sensing signals |
| Power infrastructure | [`Power_Monitor_Board/`](./Power_Monitor_Board/) | Distributes 5 V and monitors branch voltage/current |

Each board has a narrow responsibility so that electrical functions can be replaced, tested, and explained independently.

---

## Responsibility Boundary

### Hardware owns

- electrical connectivity
- connector selection
- signal routing
- power routing
- actuator-driver circuitry
- sensor-interface circuitry
- local protection and decoupling
- PCB layout and manufacturability
- physical access for setup and maintenance

### Firmware owns

- I²C register behavior
- lifecycle state
- command validation
- PWM and GPIO behavior
- sensor sampling
- feedback publication
- E-STOP software behavior

### Host software owns

- orchestration
- module coordination
- logging
- calibration and conversion
- dashboards
- process-level decisions

A hardware board may make a function electrically possible without implementing the corresponding control behavior by itself.

---

## Common Design Requirements

### Distributed I²C architecture

The system uses:

- Raspberry Pi 5 as the central host
- Raspberry Pi Pico-based distributed nodes
- a shared register-based I²C interface
- modular actuator and sensor boards connected to local controllers

### Connector and cable policy

The design prefers standardized and replaceable connections where possible.

Goals include:

- reducing wiring mistakes
- improving replacement and maintenance
- keeping I²C and power wiring organized
- supporting repeated setup and teardown
- making signal paths visible for explanation

Typical connection methods include:

- ribbon cables
- board-mounted headers
- keyed or pitch-consistent connectors
- screw terminals or equivalent power connectors

### Power distribution

The power architecture is designed around monitored 5 V distribution.

Goals include:

- supplying multiple branches
- observing voltage and current
- separating logic/control wiring from higher-current paths where practical
- exposing electrical behavior to the software layer

### Tabletop and demonstration constraints

The system prioritizes:

- accessible connectors
- visible signal flow
- practical hand assembly
- compatibility with small actuators
- compatibility with LEGO, Tamiya, and simple custom mechanisms
- repeatable demonstrations
- easy replacement over highly optimized packaging

These choices are deliberate trade-offs for a proof-of-concept platform.

---

## Implemented Hardware

The repository currently includes implemented or prepared design data for:

- distributed controller boards
- DC motor interface boards
- servo interface boards
- photo-reflector sensor boards
- power monitoring and distribution boards
- Raspberry Pi 5 wiring support
- Gerber manufacturing data
- schematic files
- BOM references
- implementation and manufacturing notes

Board-level status and limitations should be confirmed in each board README.

---

## Directory Structure

```text
Hardware/
├─ README.md
├─ Controller_Board/
├─ DC_Motor_Board/
├─ Servo_Board/
├─ Sensor_Board/
├─ Power_Monitor_Board/
├─ Pi5_Wiring_Auxiliary/
└─ Manufacturing/
```

Typical contents:

| Path | Purpose |
|---|---|
| `README.md` | Board role, interfaces, design decisions, limitations, and links |
| `schematic/` | Circuit schematic source or exported files |
| `gerber/` | PCB manufacturing data |
| `Manufacturing/` | Cross-board production, BOM, and assembly guidance |

---

## Manufacturing

Gerber files are provided as prototype manufacturing data.

Before ordering, verify:

- board dimensions
- board outline
- layer count
- drill recognition
- board thickness
- copper weight
- quantity
- connector footprint orientation
- mounting-hole position
- manufacturing preview

Detailed guidance:

- [`Manufacturing/`](./Manufacturing/)

---

## BOM and Procurement

Where available, BOM data is managed through board documentation and DigiKey MyList links.

Procurement goals include:

- use of identifiable parts
- easier reordering
- avoidance of undocumented module substitutions
- repeatability for reproduction and review
- preference for commonly available components

A BOM link does not replace checking package, footprint, ratings, tolerance, availability, and lifecycle status.

---

## Relationship to Firmware

| Hardware board | Firmware relationship |
|---|---|
| Controller Board | Runs the common Raspberry Pi Pico node firmware |
| DC Motor Board | Used by [`Firmware/motor_node`](../Firmware/motor_node/) |
| Servo Board | Used by [`Firmware/servo_node`](../Firmware/servo_node/) |
| Sensor Board | Used by [`Firmware/sensor_node`](../Firmware/sensor_node/) |
| Power Monitor Board | Provides electrical data to host-side monitoring software |
| Pi5 Wiring Auxiliary | Connects the Raspberry Pi 5 host to distributed nodes |

The public control contract is documented in:

- [`../Docs/Register_Map/`](../Docs/Register_Map/)
- [`../Firmware/common/`](../Firmware/common/)

---

## Relationship to Software

The software layer uses hardware-exposed values for:

- I²C node monitoring
- orchestration
- event logging
- voltage/current monitoring
- SQLite storage
- Grafana visualization

```text
physical board
      ↓ electrical signal
firmware register
      ↓
host software
      ↓
database and dashboard
```

See:

- [`../Software/`](../Software/)

---

## Current Constraints

- designed for tabletop proof-of-concept use
- not qualified for industrial safety or regulatory compliance
- I²C wiring distance is limited by the current bus design
- 5 V distribution is specific to the present system
- connectors are standardized within the project, not against an external industrial standard
- some boards are tailored to the selected motors, servos, and photo-reflector circuit
- protection, redundancy, and environmental tolerance are limited
- board revision and assembly status may differ by module
- physical mechanisms use prototype-grade construction

---

## Extension Points

Possible future hardware extensions include:

- more standardized harnesses
- improved connector keying
- differential or buffered communication
- additional actuator boards
- additional sensor front ends
- configurable sensor excitation
- dedicated protection and fault isolation
- improved power-domain separation
- branch fusing
- current-limited outputs
- more compact module packaging
- mounting standards
- automatic board identification
- dedicated test fixtures

---

## Recommended Reading Order

To follow the hardware from the system center outward:

1. [`Pi5_Wiring_Auxiliary/`](./Pi5_Wiring_Auxiliary/)
2. [`Controller_Board/`](./Controller_Board/)
3. [`DC_Motor_Board/`](./DC_Motor_Board/)
4. [`Servo_Board/`](./Servo_Board/)
5. [`Sensor_Board/`](./Sensor_Board/)
6. [`Power_Monitor_Board/`](./Power_Monitor_Board/)
7. [`Manufacturing/`](./Manufacturing/)

---

## Source of Truth

| Topic | Authoritative source |
|---|---|
| Overall hardware architecture | This README and board READMEs |
| Board electrical design | Board schematic files |
| PCB production data | Board Gerber directories |
| Part selection | Board BOM documentation and linked lists |
| Firmware behavior | [`../Firmware/`](../Firmware/) |
| Register behavior | [`../Docs/Register_Map/`](../Docs/Register_Map/) |
| Manufacturing guidance | [`Manufacturing/`](./Manufacturing/) |

---

## Related Documents

- Repository overview → [`../README.md`](../README.md)
- Firmware overview → [`../Firmware/`](../Firmware/)
- Software overview → [`../Software/`](../Software/)
- System architecture → [`../Docs/System_Architecture/`](../Docs/System_Architecture/)
- Register map → [`../Docs/Register_Map/`](../Docs/Register_Map/)
- Use cases → [`../Use_cases/`](../Use_cases/)
- Japanese overview → [`../日本語版/`](../日本語版/)
- Manufacturing → [`Manufacturing/`](./Manufacturing/)
