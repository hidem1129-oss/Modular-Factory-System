# ADL-001: System Context and Goals

## Purpose of This Document

This document defines the system context, system boundary, major elements, external relationships, and architectural goals of the Modular Factory System.

It builds on the requirements and success conditions defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)

This document focuses on the system as a whole.

Detailed technology selections and implementation decisions are documented in later ADL files.

---

## System Definition

The Modular Factory System is a reconfigurable tabletop proof-of-concept platform that connects physical process behavior with embedded control, host-side orchestration, monitoring, persistence, and visualization.

The system is intended to make the relationships between these elements visible and discussable within one working implementation.

The system is not defined by one particular process.

The Amazon-style Sorting Demo and Stamp Process Demo are process-specific configurations built on the same platform.

The reusable platform includes:

* distributed control nodes
* common communication and register interfaces
* host-side orchestration
* live monitoring
* persistent logging
* historical visualization
* reusable hardware and software responsibilities

The process-specific configuration includes:

* the physical mechanism
* the connected actuator and sensor combination
* the process sequence
* process parameters
* workpiece-handling logic
* process-specific image-processing or decision logic

---

## System Context

The system operates between a human reviewer or developer and a tabletop physical process.

The user configures, runs, observes, and reviews the system.

The host computer coordinates device nodes and collects available observations.

The device nodes control or observe physical devices.

The physical mechanism performs the process-specific behavior.

Stored records can later be examined through visualization tools.

The main interaction can be summarized as follows:

```text
Developer or reviewer
        ↓
Host-side configuration and orchestration
        ↓
Distributed control and sensing nodes
        ↓
Physical tabletop process
        ↓
Observed node states and measurements
        ↓
Monitoring and persistent storage
        ↓
Historical review and discussion
```

The system supports both forward control flow and reverse observation flow.

The control flow determines what the system attempts to do.

The observation flow provides information about the states and measurements that the implemented nodes expose.

These two flows are related but are not equivalent.

An issued command or reported node state does not necessarily confirm that the intended physical result occurred.

---

## Primary System Goals

The system has seven primary goals.

### G1: Provide a Shared Physical Reference

The system should provide a real and observable process that can be examined by people from different technical backgrounds.

The physical process should make it possible to discuss:

* what moves
* what is detected
* where decisions occur
* which component owns each responsibility
* what information is available
* what remains unobserved

The goal is to reduce ambiguity that may remain in diagrams or isolated prototypes.

---

### G2: Maintain an End-to-End Implementation Path

The system should include a connected path across:

* physical mechanisms
* hardware
* firmware
* communication
* host-side orchestration
* monitoring
* persistent logging
* historical visualization

The goal is not to maximize the sophistication of every layer.

The goal is to ensure that the interfaces and assumptions between layers can be examined.

---

### G3: Separate Reusable and Process-Specific Elements

The architecture should distinguish between reusable platform elements and process-specific elements.

Reusable elements include:

* control-node concepts
* communication interfaces
* register conventions
* host-side monitoring
* persistence
* visualization

Process-specific elements include:

* mechanisms
* workpieces
* process sequence
* actuator arrangement
* sensor placement
* image-processing conditions
* timing and motion parameters

This separation allows different use cases to reuse common infrastructure without requiring every component to be universal.

---

### G4: Make Responsibility Boundaries Understandable

The system should make it possible to identify where each major responsibility belongs.

The architecture should distinguish between:

* physical-process responsibility
* device-local control
* multi-node orchestration
* live observation
* persistent storage
* historical visualization

The goal is to support:

* localized changes
* fault isolation
* module replacement
* cross-domain review
* discussion of future feature ownership

---

### G5: Support Reconfiguration

The system should support more than one physical process through a combination of:

* mechanism replacement
* module rearrangement
* node selection
* address assignment
* orchestration changes
* parameter changes

The goal is to confirm that the platform can be adapted without complete redesign.

---

### G6: Remain Practical at Tabletop Scale

The system should remain practical for individual development and demonstration.

This includes:

* compact hardware
* accessible wiring
* replaceable modules
* commercially available mechanical parts
* inspectable mechanisms
* tools and environments that can be started without disproportionate effort

The goal is to make architectural questions testable before committing to a larger implementation.

---

### G7: Preserve Architectural Traceability

The system and repository should allow reviewers to trace:

* a project concern
* to a requirement
* to an architectural decision
* to an implementation element
* to evidence in a demonstrated use case

The goal is to show that the implementation was derived from explicit reasoning rather than assembled as unrelated technical experiments.

---

## System Boundary

The system boundary includes the elements required to create, control, observe, record, and review the tabletop process.

### Inside the System Boundary

| Area                  | Included elements                                                                                             |
| --------------------- | ------------------------------------------------------------------------------------------------------------- |
| Physical process      | Tabletop mechanisms, workpieces, guides, gates, clamps, conveyors, and stamping structures                    |
| Actuation and sensing | DC motors, servo motors, photo sensors, current and voltage monitoring, and camera input                      |
| Hardware              | Custom control boards, sensor boards, wiring boards, power-monitoring boards, connectors, and harnesses       |
| Device control        | Raspberry Pi Pico-based motor, servo, and sensor firmware                                                     |
| Communication         | I²C connections, node addresses, and register-based interfaces                                                |
| Host control          | Raspberry Pi 5, process orchestration scripts, and process-specific decision logic                            |
| Monitoring            | I2C Debugger and available live node-state views                                                              |
| Persistence           | SQLite event, snapshot, and measurement records                                                               |
| Visualization         | Grafana dashboards and historical views                                                                       |
| Documentation         | Schematics, Gerber files, BOM information, firmware, software, register references, and use-case descriptions |

### Outside the System Boundary

| Area                         | Excluded element                                                                                         |
| ---------------------------- | -------------------------------------------------------------------------------------------------------- |
| Production infrastructure    | Full factory lines, production utilities, and plant-wide deployment                                      |
| Industrial safety            | Certified safety controllers, safety relays, guarding, and formal risk-reduction systems                 |
| Enterprise systems           | MES, ERP, production scheduling, and business-system integration                                         |
| Industrial networking        | Long-distance field networks, hardened network infrastructure, and plant-wide communication              |
| Production reliability       | Redundancy, failover, validated uptime, and long-term unattended operation                               |
| Production cybersecurity     | Authentication, authorization, network segmentation, secure provisioning, and complete threat mitigation |
| Certification                | Industrial product certification and regulatory compliance                                               |
| Commercial product operation | Customer support, production maintenance contracts, and field deployment processes                       |

The excluded elements may become necessary in a production system.

They are outside the boundary of the current architecture prototype.

---

## External Actors and Elements

The system interacts with several external actors and elements.

| External actor or element            | Relationship to the system                                                                  |
| ------------------------------------ | ------------------------------------------------------------------------------------------- |
| Developer                            | Designs, assembles, configures, programs, tests, and modifies the system                    |
| Technical reviewer                   | Examines architecture, responsibilities, interfaces, and implementation choices             |
| Recruiter or non-specialist reviewer | Uses the demonstrations and documentation to understand the scope and intent of the project |
| Workpiece                            | Enters the process and is moved, classified, stamped, or otherwise handled                  |
| Component suppliers                  | Provide electronic and mechanical parts used to build or repair the system                  |
| PCB manufacturer                     | Manufactures custom PCBs from published design data                                         |
| GitHub repository                    | Stores implementation files, design documentation, and architectural reasoning              |
| Development tools                    | Support firmware development, host-side software, data inspection, and debugging            |
| Video content                        | Provides a guided explanation of the system behavior and architectural story                |

The video is not part of the direct control system.

It is an external communication medium used to explain the implementation and its reasoning.

---

## Major System Elements

### Physical Mechanisms

Physical mechanisms define the process-specific motion and workpiece handling.

Examples include:

* conveyor movement
* sorting gates
* workpiece guides
* paper clamping
* stamp movement
* paper take-up

These mechanisms may be replaced or rearranged without redefining the complete control and monitoring platform.

---

### Actuators and Sensors

Actuators and sensors connect the physical process to the control architecture.

Examples include:

* DC motors
* servo motors
* photo sensors
* camera input
* current and voltage sensing

The availability of a device does not automatically imply that every physical outcome is verified.

The system can only observe what the installed sensing and firmware expose.

---

### Distributed Device Nodes

Raspberry Pi Pico-based nodes provide device-local control and sensing responsibilities.

The nodes expose available commands, configuration values, and observations through a register-based interface.

The distributed-node architecture is intended to:

* isolate device-local responsibilities
* support module replacement
* reduce change propagation
* allow the host to coordinate multiple device types through a common model

Detailed reasoning is documented in later ADL files.

---

### Host Computer

The Raspberry Pi 5 acts as the host platform.

Its responsibilities include:

* process orchestration
* node communication
* process-specific decision logic
* camera processing where required
* monitoring support
* persistent logging
* visualization-related services

The host may execute several responsibilities on the same computer.

These responsibilities remain logically separate even when they share one physical platform.

---

### Monitoring and Persistence

The monitoring layer reads available node states and measurements.

The persistence layer records selected observations for later review.

Their role is to support:

* live inspection
* debugging
* state-history review
* comparison between runs
* identification of missing observations

They do not directly confirm every physical process result.

---

### Historical Visualization

Grafana presents historical records from persistent storage.

Its role is to support:

* timeline review
* state comparison
* power-related observation
* discussion of process behavior over time

Grafana is not part of the direct device-control path.

---

## Control and Observation Relationships

The system contains two main directional relationships.

### Control Relationship

```text
Process definition
        ↓
Host-side orchestration
        ↓
Node commands and configuration
        ↓
Device-local behavior
        ↓
Physical actuation
```

### Observation Relationship

```text
Physical or electrical condition
        ↓
Sensor or node state
        ↓
Register exposure
        ↓
Host-side monitoring
        ↓
Persistent record
        ↓
Historical visualization
```

The two relationships do not provide identical information.

For example:

* the control path may show that a command was issued
* the observation path may show that a node reported a state
* neither path necessarily proves the final mechanical outcome unless an appropriate sensor exists

This distinction is part of the system context and influences later monitoring and responsibility decisions.

---

## Reusable Platform and Process-Specific Configuration

The architecture separates a reusable platform from process-specific configurations.

| Reusable platform                | Process-specific configuration          |
| -------------------------------- | --------------------------------------- |
| Raspberry Pi 5 host              | Use-case orchestration sequence         |
| Distributed node model           | Selected node combination               |
| Common register interface        | Node addresses used by the process      |
| Monitoring software              | Process-specific states of interest     |
| SQLite persistence               | Selected records and events             |
| Grafana visualization capability | Process-specific dashboards or queries  |
| Control and sensing boards       | Connected motors, servos, and sensors   |
| Common wiring approach           | Mechanism-specific cable routing        |
| Documentation structure          | Use-case-specific operating explanation |

This boundary is intentionally practical rather than absolute.

Some reuse may require configuration changes, additional nodes, or small implementation changes.

The goal is to avoid complete redesign, not to guarantee zero-change reuse.

---

## Current System Configurations

### Amazon-style Sorting Demo

The sorting configuration demonstrates:

* conveyor movement
* workpiece detection
* camera-based visible-color classification
* host-side decision-making
* servo-gate selection
* distributed actuator and sensor nodes

The process uses a conveyor and multiple sorting gates to route colored wooden workpieces.

---

### Stamp Process Demo

The stamping configuration demonstrates:

* paper clamping
* bidirectional stamp movement
* physical stamping
* clamp release
* paper take-up
* a repeated host-side process sequence

The process uses different physical mechanisms and actuator combinations from the sorting demo.

---

### Architectural Significance

The two configurations provide evidence that:

* the system is not limited to one physical process
* process-specific mechanisms can be replaced
* different node combinations can be coordinated
* the same monitoring and persistence concepts can remain available
* host-side orchestration can define different process behavior

---

## System Goals and Evidence

| Goal                                                     | Current evidence                                                                   |
| -------------------------------------------------------- | ---------------------------------------------------------------------------------- |
| G1: Shared physical reference                            | Visible sorting and stamping mechanisms                                            |
| G2: End-to-end implementation path                       | Hardware, firmware, host software, monitoring, persistence, and Grafana            |
| G3: Separation of reusable and process-specific elements | Common platform used by two different use cases                                    |
| G4: Understandable responsibility boundaries             | Separate device nodes, orchestration, monitoring, storage, and visualization roles |
| G5: Reconfiguration                                      | Sorting and stamping configurations                                                |
| G6: Practical tabletop scale                             | Compact host, custom PCBs, and commercially available construction parts           |
| G7: Architectural traceability                           | ADL, repository documentation, implementation files, and use-case evidence         |

---

## Architectural Questions Supported by the System

The system provides a basis for examining questions such as:

* Should a behavior belong in device firmware or host orchestration?
* Which physical outcomes require additional sensing?
* Which modules can be reused without modification?
* Which changes affect only one process configuration?
* Which changes propagate across multiple layers?
* What information should be retained for later review?
* What does a monitoring value actually prove?
* Which components are difficult to replace or reproduce?
* Which production requirements are intentionally outside the current scope?
* What should be validated before building a larger system?

The system does not automatically answer every question.

It makes the questions concrete enough to investigate using one shared implementation.

---

## Relationship to Later Decisions

This system context provides the basis for later architectural decisions.

| Later document                                   | Context provided by this document                                         |
| ------------------------------------------------ | ------------------------------------------------------------------------- |
| ADL-002: Design and Development Constraints      | Defines the environment in which the system must be built and maintained  |
| ADL-003: Host Platform Selection                 | Defines the responsibilities required from the host                       |
| ADL-004: Distributed Node Architecture           | Defines the device-local responsibilities and reusable module boundary    |
| ADL-005: Communication Interface Selection       | Defines the communication scale and participants                          |
| ADL-006: Common Register Interface               | Defines the need for common host-to-node access                           |
| ADL-007: Separation of Responsibilities          | Refines the logical ownership described in this document                  |
| ADL-008: Reconfigurable Physical Mechanisms      | Defines the relationship between platform and process-specific structures |
| ADL-009: Monitoring and Control Separation       | Refines the control and observation relationships                         |
| ADL-010: Component Selection and Reproducibility | Applies the practical and replacement goals to sourcing and publication   |
| ADL-011: Prototype Scope and Deferred Features   | Expands the excluded system areas and production-level boundaries         |

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [Repository overview](../README.md)
* [Use Cases](../Use_cases/)
* [Hardware](../Hardware/)
* [Firmware](../Firmware/)
* [Software](../Software/)
* [Technical Documentation](../Docs/)
