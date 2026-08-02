# Use Cases

This directory contains process-level demonstrations built with the Modular Factory System.

Each use case shows how common hardware boards, firmware nodes, monitoring software, and visualization tools can be reused and recombined to implement a different tabletop process.

The use cases are not isolated machines.

They are case studies showing how one modular platform can support multiple physical mechanisms, control sequences, and observation workflows.

---

## Overview

The Modular Factory System separates reusable platform components from process-specific configuration.

```text
Reusable platform
├─ hardware control boards
├─ Raspberry Pi Pico firmware nodes
├─ I²C register interface
├─ Raspberry Pi 5 host
├─ I2C Debugger
├─ SQLite logging
└─ Grafana visualization

Use-case-specific configuration
├─ physical mechanism
├─ selected modules
├─ orchestration sequence
├─ sensor conditions
└─ process objective
```

The current use cases demonstrate that the same control and monitoring foundation can be applied to different tabletop processes.

---

## Purpose

The use cases are intended to demonstrate:

* modular reuse of hardware and firmware
* separation of platform functions from process logic
* replacement of physical mechanisms without redesigning the entire system
* host-side orchestration of distributed I²C nodes
* observation of process behavior through a shared monitoring layer
* persistence of events and measurements in SQLite
* retrospective analysis through Grafana
* tabletop-scale validation of architecture and process concepts

The primary objective is not to reproduce a production machine at full scale.

The objective is to validate how physical mechanisms, embedded control, orchestration, monitoring, persistence, and visualization can be connected through a reusable architecture.

---

## Use Cases as Architecture Validation

The use cases provide evidence that the architecture can support more than one fixed demonstration.

A single successful machine could be the result of application-specific integration.

Multiple processes built on the same platform provide stronger evidence of reuse.

```text
Sorting process
└─ sensing, image classification, transport, and diversion

Stamp process
└─ material feed, positioning, holding, pressing, and release
```

The physical mechanisms and process sequences differ.

The underlying platform remains largely shared.

This distinction is central to the system design.

---

## Current Use Cases

| Use Case                                                    | Process Type                      | Primary Operation                                        | Main Technologies                                              |
| ----------------------------------------------------------- | --------------------------------- | -------------------------------------------------------- | -------------------------------------------------------------- |
| [`Amazon-style Sorting Demo`](./Amazon-style_Sorting_Demo/) | Tabletop sorting and logistics    | Detects, classifies, transports, and diverts a workpiece | Camera, photo-reflector, DC motor, servo, I²C, SQLite, Grafana |
| [`Stamp Process Demo`](./Stamp_Process_Demo/)               | Simplified press-style processing | Feeds, holds, presses, releases, and advances roll paper | DC motors, servo clamp, rack-and-pinion, I²C, SQLite, Grafana  |

The two processes use different mechanisms and operation sequences while reusing the same architectural layers.

---

## Shared Platform Components

The current use cases reuse the following platform elements.

### Hardware Layer

* Raspberry Pi 5 host
* Raspberry Pi Pico-based distributed nodes
* Controller Board
* DC Motor Board
* Servo Board
* Power Monitor Board
* Pi 5 Wiring Auxiliary Board
* common I²C wiring and register interface

Sensor and mechanism requirements differ between use cases, but the control and monitoring foundation remains shared.

### Firmware Layer

The use cases reuse firmware roles such as:

* motor-node control
* servo-node control
* sensor-node access
* command validation
* lifecycle-state reporting
* feedback exposure through registers

The host does not directly implement low-level actuator timing.

Device-local behavior remains inside the corresponding firmware node.

### Software Layer

The use cases share:

* host-side orchestration
* I2C Debugger live monitoring
* state interpretation
* event and snapshot logging
* monitoring-session handling
* SQLite persistence
* Grafana historical analysis

This allows different processes to be observed using the same monitoring and data-analysis path.

---

## Mechanism Construction and Material Strategy

The physical mechanisms used in these demonstrations are not built from a dedicated industrial-machine frame or a custom structural system.

They are assembled from readily available construction kits, general-purpose mechanical parts, reused components, and simple temporary fixtures.

The objective is to create a mechanism that is sufficient for process and architecture validation without requiring a fully custom machine structure.

---

### Reuse of Commercial Construction Kits

Some mechanical elements originate from commercially available educational construction products.

For example, parts from the Tamiya Educational Construction Series may be reused as mechanical building blocks.

A product such as the Tamiya Tracked Vehicle Chassis Kit is originally intended to assemble a small battery-powered tracked vehicle. Its components include a wooden chassis, tracks, wheels, sprockets, a gearbox, and a Type 130 motor.

In this project, a construction product does not have to be used only in the form described by its original assembly instructions.

Its parts can instead be treated as general-purpose mechanical elements.

```text
Original product role
└─ Small tracked vehicle

Possible prototype roles
├─ conveyor or feed mechanism
├─ rotating shaft support
├─ motor and gearbox source
├─ wheel or sprocket source
├─ structural base
└─ temporary mechanism frame
```

This allows existing parts to be rearranged according to the requirements of the demonstration process.

The project does not claim that these modified configurations are supported or intended by the original product manufacturer.

---

### No Dedicated Structural System

The current tabletop mechanisms do not rely on a dedicated machine frame, custom jig system, or standardized industrial structural material.

Depending on the demonstration, components may be positioned or held using:

* screws already available during assembly
* nuts, washers, and spacers
* wooden boards
* plastic plates
* cardboard
* adhesive tape
* double-sided tape
* cable ties
* reused brackets
* parts taken from construction kits
* other readily available workshop materials

These materials are selected according to the immediate mechanical requirement rather than a fixed structural standard.

The resulting mechanism should therefore be interpreted as a functional prototype rather than a finished machine assembly.

---

### Temporary Fixtures Are Intentional

Temporary attachment methods are acceptable when they are sufficient to validate the intended operation.

For example, tape or an improvised screw arrangement may be used to confirm:

* component position
* actuator travel
* sensor placement
* workpiece guidance
* interference between mechanisms
* required mounting angle
* approximate mechanical load
* whether the process sequence is physically feasible

```text
Initial prototype
└─ Position parts quickly and test the mechanism

After validation
└─ Improve only the portions that require accuracy, rigidity, or repeatability
```

A dedicated fixture is not created before its necessity has been demonstrated.

This avoids spending substantial time designing a precise structure for a mechanism that may later be changed or rejected.

---

### Function Before Mechanical Finish

The mechanical construction follows a function-first approach.

The initial questions are:

* Can the workpiece be moved?
* Can the actuator reach the required position?
* Can the sensor detect the intended event?
* Can the process sequence be completed?
* Can the monitoring system observe the operation?
* Which mechanical requirements actually affect the result?

Cosmetic finish and structural refinement are secondary during the initial proof-of-concept stage.

This does not mean that rigidity, accuracy, durability, or safety are unimportant.

It means that they are added according to demonstrated requirements rather than assumed in advance.

---

### Replaceability of the Mechanical Structure

The mechanisms shown in the current demonstrations are examples of one possible physical implementation.

They are not mandatory parts of the Modular Factory System architecture.

A user could replace the current mechanism with:

* a different commercial construction kit
* aluminum extrusion
* laser-cut plates
* machined components
* a custom wooden structure
* 3D-printed parts
* an existing laboratory mechanism
* a small industrial training device

The control nodes, register interface, orchestration structure, monitoring software, and data-visualization path can remain conceptually similar.

```text
Replaceable
└─ Physical structure and attachment method

Reusable
├─ distributed control interface
├─ firmware-node roles
├─ host-side orchestration
├─ monitoring
├─ SQLite logging
└─ Grafana analysis
```

This separation is important because the project is intended to demonstrate system architecture rather than prescribe one mechanical construction method.

---

### Possible Use of 3D-Printed Parts

A 3D printer may be used when a custom part provides clear value.

Suitable examples include:

* sensor brackets
* motor mounts
* servo holders
* workpiece guides
* shaft supports
* alignment features
* cable-routing parts
* adapters between incompatible hole patterns

However, custom printing is not required for every component.

Before designing a dedicated part, the project may first use an improvised fixture to determine:

* the required dimensions
* the correct mounting position
* the necessary rigidity
* whether adjustment is required
* whether the mechanism will remain in the final design

This prevents the project from producing precise custom parts before the underlying mechanism has been validated.

---

### Design Intent

This construction strategy supports the goals of a tabletop proof of concept.

It provides:

* rapid physical experimentation
* low initial construction cost
* easy modification
* reuse of readily available parts
* reduced dependence on specialized fabrication equipment
* earlier discovery of mechanical and integration requirements
* a clear path from temporary fixture to refined component

The mechanism is therefore developed progressively.

```text
Available parts
      ↓
Temporary physical arrangement
      ↓
Functional test
      ↓
Requirement discovery
      ↓
Selective structural improvement
```

The purpose is not to demonstrate that tape, improvised screws, or educational kits are appropriate for production machinery.

The purpose is to demonstrate that a process concept and system architecture can be tested before committing to dedicated mechanical construction.

---

### Scope and Limitations

The current mechanism-construction approach is suitable for:

* tabletop demonstrations
* architecture validation
* process-sequence experiments
* sensor and actuator integration
* communication between technical and non-technical participants
* early proof-of-concept work

It is not intended to provide:

* production-machine rigidity
* certified guarding
* controlled mechanical tolerances
* validated fatigue life
* industrial cycle-time performance
* formal maintainability
* repeatable production assembly
* safety-rated mechanical construction

If the system were developed beyond the proof-of-concept stage, the physical mechanism and fixtures would need to be redesigned according to the required accuracy, load, lifetime, environment, and safety standards.

---

## Use-Case-Specific Elements

Each process defines its own combination of:

* physical mechanism
* module arrangement
* actuator assignment
* sensor conditions
* process sequence
* image-processing or decision logic
* timing assumptions
* completion criteria

For example:

```text
Sorting Demo
├─ conveyor
├─ workpiece sensor
├─ camera classification
└─ servo diversion gate

Stamp Process Demo
├─ paper-feed mechanism
├─ clamp
├─ rack-and-pinion press
└─ repeated feed-and-press sequence
```

These elements can change without replacing the entire hardware, firmware, monitoring, and visualization stack.

---

## Common System Flow

The use cases follow the same broad architecture.

```text
Process objective
      ↓
Physical mechanism
      ↓
Hardware control boards
      ↓
Firmware nodes
      ↓
I²C register interface
      ↓
Raspberry Pi 5 orchestration
      ↓
I2C Debugger observation and SQLite logging
      ↓
Grafana historical visualization
```

Not every use case must use every available component.

The system allows each process to select only the modules and observations it requires.

---

## Orchestration Boundary

The orchestration layer coordinates multiple nodes to implement the process sequence.

Typical responsibilities include:

* issuing commands to distributed nodes
* waiting for node readiness or completion
* evaluating sensor conditions
* selecting the next process step
* coordinating multiple actuators
* handling process-level sequencing

The orchestration layer does not replace device-local firmware control.

```text
Orchestration
└─ decides what operation should happen next

Firmware node
└─ validates and performs the local operation
```

This separation allows process sequences to change without moving all behavior into one central program.

---

## Observation and Validation

Each use case is evaluated through both physical behavior and persisted observations.

### Physical Validation

The mechanism should complete the intended process flow.

Examples include:

* transporting a workpiece
* detecting a workpiece
* moving a sorting gate
* feeding paper
* clamping material
* performing a press motion

### Monitoring Validation

The I2C Debugger can show:

* current node states
* command and feedback values
* detected state transitions
* current electrical measurements
* communication failures
* active monitoring-session context

### Historical Validation

Grafana can be used to inspect:

* node-state timelines
* transition events
* ERROR and ESTOP history
* current, voltage, and calculated-power trends
* average and maximum recorded calculated-power values
* recorded maximum, minimum, and maximum-to-minimum ranges

The monitoring data supports later investigation but does not independently prove the cause of a physical event.

---

## Relationship Between the Current Use Cases

The current demonstrations were selected to exercise different process characteristics.

### Amazon-style Sorting Demo

The sorting demonstration emphasizes:

* event-driven sensing
* image-based classification
* transport control
* actuator selection based on a recognition result
* coordination between sensing, software decisions, and physical diversion

### Stamp Process Demo

The stamp demonstration emphasizes:

* sequential multi-actuator operation
* repeated process cycles
* material feed
* holding and release
* conversion of rotational motion into linear press movement
* reuse of the same monitoring stack with a different mechanism

Together, the two demonstrations show that the platform is not limited to one process category.

---

## Reconfiguration Model

A new use case can be created by changing a limited set of elements.

```text
Keep
├─ common control boards
├─ firmware-node interface
├─ I²C communication
├─ monitoring software
├─ SQLite logging
└─ Grafana analysis

Change
├─ mechanism
├─ module arrangement
├─ orchestration program
├─ sensor conditions
└─ process-specific validation
```

This model supports rapid tabletop proof-of-concept development.

It also makes it easier to identify whether a requirement belongs to:

* reusable platform architecture
* device-local implementation
* process orchestration
* physical mechanism design
* monitoring or analysis

---

## Documentation Format

Each use-case directory should document the process as a small architecture case study.

Recommended sections include:

1. Overview
2. Process Goal
3. System Boundary
4. Reused Platform Components
5. Use-Case-Specific Components
6. Hardware Used
7. Firmware Used
8. Software and Orchestration
9. Operation Sequence
10. Observation and Validation
11. Results
12. Limitations
13. Reconfiguration Lessons
14. Possible Future Improvements
15. Related Documents

This structure distinguishes implementation details from the architectural evidence provided by the use case.

---

## What the Use Cases Demonstrate

The current use cases demonstrate that:

* the distributed I²C architecture can coordinate multiple physical modules
* common firmware roles can support different process configurations
* process logic can remain separate from device-local control
* the same monitoring software can observe different mechanisms
* the same SQLite and Grafana path can analyze different process runs
* physical reconfiguration does not require redesigning every software layer
* tabletop models can expose architecture, integration, and operational questions before larger-scale implementation

---

## Current Scope and Limitations

The current use cases are tabletop proof-of-concept demonstrations.

They currently assume:

* local operation
* controlled demonstration conditions
* manually assembled mechanisms
* low-voltage prototype hardware
* a local Raspberry Pi host
* a shared I²C bus
* manually developed orchestration scripts
* no production-grade safety certification
* no industrial cycle-time guarantee
* no formal reliability or maintainability qualification

The demonstrations are not intended to replace:

* industrial PLC systems
* production SCADA platforms
* certified safety controllers
* guarded industrial machinery
* validated production equipment
* formal manufacturing-process qualification

Their purpose is architecture validation, communication, experimentation, and early process prototyping.

---

## Possible Future Use Cases

Possible future demonstrations include:

* multi-axis positioning
* inspection and rejection
* buffer and queue control
* simple assembly processes
* process recovery after a simulated failure
* module discovery and Plug & Play configuration
* configurable routing through a flow editor
* abnormal-load investigation using monitoring data
* comparison of two process configurations
* hardware or software fault-injection exercises

New use cases should continue to distinguish reusable platform functions from process-specific implementation.

---

## Related Documents

* Repository overview → [`../README.md`](../README.md)
* Hardware overview → [`../Hardware/`](../Hardware/)
* Firmware overview → [`../Firmware/`](../Firmware/)
* Software overview → [`../Software/`](../Software/)
* I2C Debugger → [`../Software/I2C_Debugger/`](../Software/I2C_Debugger/)
* Grafana → [`../Software/Grafana/`](../Software/Grafana/)
* System architecture → [`../Docs/System_Architecture/`](../Docs/System_Architecture/)
* Register map → [`../Docs/Register_Map/`](../Docs/Register_Map/)
* Architecture Decision Log → [`../ADL/`](../ADL/)
