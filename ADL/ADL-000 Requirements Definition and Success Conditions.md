# ADL-000: Requirements Definition and Success Conditions

## Purpose of This Document

This document defines the high-level requirements and success conditions of the Modular Factory System.

It translates the problem awareness and architectural decision principles described in the [Architecture Decision Log overview](./README.md) into conditions that can be connected to implementation and demonstrated behavior.

This document focuses on what the system should achieve.

Specific technology selections, such as the Raspberry Pi 5, Raspberry Pi Pico, I²C, SQLite, and Grafana, are documented separately as architectural decisions.

---

## Problem-to-Requirement Flow

The project was developed from three related concerns:

1. people from different technical backgrounds may interpret an abstract system description differently
2. isolated prototypes make it difficult to examine interactions across mechanical, electrical, embedded, software, operational, and data perspectives
3. process flow and responsibility boundaries become more expensive to revise after committing to a large physical implementation

These concerns lead to three corresponding responses:

| Problem awareness                                            | Required response                                                     |
| ------------------------------------------------------------ | --------------------------------------------------------------------- |
| Different roles may interpret an abstract system differently | Create a shared and observable physical reference                     |
| Isolated prototypes limit cross-domain review                | Maintain an end-to-end implementation path                            |
| Late changes to processes and responsibilities are expensive | Validate process flow and responsibility boundaries at tabletop scale |

These responses are translated into the following system requirements:

| Required response                                         | Resulting requirement                                                                      |
| --------------------------------------------------------- | ------------------------------------------------------------------------------------------ |
| Create a shared and observable physical reference         | Physical behavior must be visible and explainable                                          |
| Maintain an end-to-end implementation path                | Hardware, firmware, orchestration, monitoring, persistence, and visualization must coexist |
| Validate process flow and responsibility boundaries early | The system must support rearrangement and multiple process configurations                  |

The system is therefore intended to function as a shared and observable architecture prototype rather than only as a machine that performs one specific task.

> **Diagram placeholder:**
> Add a diagram showing the flow from problem awareness through required responses, system requirements, success conditions, and implementation evidence.

---

## Intended Value

The primary value of the Modular Factory System is the ability to examine a small but complete physical system before committing to a larger implementation.

The platform is intended to provide a basis for discussing:

* physical process behavior
* module responsibilities
* interfaces between system layers
* process orchestration
* observable and unobservable states
* data collection requirements
* process-specific and reusable elements
* possible changes and their affected areas
* missing modules or responsibilities

The sorting and stamping processes are demonstrations of the platform.

They are not the sole purpose of the project.

Their role is to show that different physical processes can be constructed using common control, communication, monitoring, and data-handling elements.

---

## System Boundary

For this project, the system includes:

* tabletop physical mechanisms
* motors, servos, sensors, and a camera
* custom control, sensing, wiring, and power-monitoring boards
* Raspberry Pi Pico-based device nodes
* the communication interface between the host and device nodes
* Raspberry Pi 5 host-side process orchestration
* monitoring software
* persistent logging
* historical visualization
* process-specific use-case scripts and mechanisms

The system boundary does not include:

* production factory infrastructure
* industrial safety systems
* enterprise MES or ERP integration
* production-scale networking
* certified industrial control hardware
* long-term production operation
* complete cybersecurity infrastructure

These exclusions define the scope of the demonstrated proof of concept rather than suggesting that those capabilities are unnecessary in a production system.

---

## Stakeholder Viewpoints

The platform is intended to provide a common reference that can be examined from multiple viewpoints.

| Viewpoint                         | Questions supported by the platform                                          |
| --------------------------------- | ---------------------------------------------------------------------------- |
| Mechanical design                 | What moves, what interferes, and what must be held, guided, or replaced?     |
| Electronics                       | What must be powered, driven, sensed, connected, or protected?               |
| Embedded software                 | Which behavior belongs inside each local node?                               |
| Host-side software                | How are multiple modules coordinated as one process?                         |
| Monitoring and operations         | Which states can be observed, recorded, and reviewed?                        |
| Data and visualization            | Which records are available, and what can they legitimately demonstrate?     |
| Process design                    | What is the process sequence, and where are decisions made?                  |
| Architecture and project planning | Which elements are reusable, process-specific, missing, or likely to change? |

The platform does not guarantee that every concern from every stakeholder is implemented.

It provides a shared physical and technical basis on which those concerns can be identified and discussed.

---

## Requirement Categories

The high-level requirements are divided into five categories.

| ID | Requirement category                      |
| -- | ----------------------------------------- |
| R1 | Shared observable physical reference      |
| R2 | End-to-end system presence                |
| R3 | Explicit responsibility boundaries        |
| R4 | Reconfiguration across physical processes |
| R5 | Practical tabletop implementation         |

> **Diagram placeholder:**
> Add a diagram showing the five requirement categories and their relationship to the Modular Factory System.

---

## R1: Shared Observable Physical Reference

The system shall provide real physical behavior that can be observed by people with different technical backgrounds.

The demonstrated process should make it possible to identify:

* what physical object is being processed
* what motion occurs
* what triggers that motion
* where process decisions are made
* which device or software element performs each action
* which states are available to the monitoring system
* which physical outcomes are not directly confirmed by the available data

### Rationale

A visible physical process reduces ambiguity that may remain in diagrams, documents, or isolated software demonstrations.

Physical behavior provides a common reference against which assumptions from different technical viewpoints can be compared.

### Important Boundary

An observed node state does not necessarily prove that the intended mechanical result occurred.

For example:

* a motor command does not prove that the mechanism moved correctly
* a servo register value does not prove that a gate reached its physical position
* a logged event does not prove that the workpiece reached its intended destination
* an expected process time does not prove that the physical operation completed within that time
* a measured current value does not by itself identify the cause of a mechanical or electrical condition

The platform should make this boundary visible rather than hide it.

The distinction between system observation and physical verification is itself an architectural concern that can be discussed using the prototype.

---

## R2: End-to-End System Presence

The system shall include a connected implementation path from physical mechanisms to historical visualization.

The demonstrated path should contain:

```text
Physical process
        ↓
Actuator and sensor hardware
        ↓
Device-local firmware
        ↓
Host-side communication and orchestration
        ↓
Monitoring and persistent logging
        ↓
Historical visualization
```

### Rationale

The purpose is to expose interfaces and assumptions between technical domains.

A detailed implementation of only one layer would not provide the same basis for cross-domain review.

For example, a software-only model may demonstrate orchestration logic but may not expose:

* wiring constraints
* actuator behavior
* mechanical interference
* sensor placement
* timing variation
* unavailable physical feedback
* maintenance and replacement considerations

Similarly, a mechanism-only prototype may demonstrate motion while leaving control, monitoring, persistence, and data interpretation undefined.

### Required Layers

| Layer              | Required role                                                  |
| ------------------ | -------------------------------------------------------------- |
| Physical mechanism | Performs an observable process-specific action                 |
| Hardware           | Connects, powers, drives, and senses physical devices          |
| Device firmware    | Provides device-local control and exposes available node state |
| Host orchestration | Coordinates multiple modules as a process                      |
| Monitoring         | Reads and presents available node information                  |
| Persistence        | Stores observations for later review                           |
| Visualization      | Presents historical behavior independently of direct control   |

The layers do not need to represent a production-ready implementation.

They must exist sufficiently to expose their responsibilities and interfaces.

---

## R3: Explicit Responsibility Boundaries

The system shall separate responsibilities sufficiently to allow each major behavior to be located and explained.

At minimum, the architecture should distinguish between:

* process-specific physical behavior
* device-local actuator or sensor behavior
* multi-node process orchestration
* live observation
* persistent storage
* historical visualization

### Rationale

Responsibility separation supports:

* easier fault isolation
* localized changes
* clearer technical discussion
* replacement of individual modules
* comparison of alternative process arrangements
* identification of missing responsibilities
* examination of which layer should own a future feature

### Requirement Interpretation

Responsibility separation does not require every function to run on a separate computer or board.

It requires the logical ownership of each function to remain understandable.

For example, the Raspberry Pi 5 may run orchestration, monitoring, persistence, and visualization-related services on the same host.

These functions can still be treated as separate responsibilities if:

* their roles are documented
* their interfaces are identifiable
* changes to one responsibility do not unnecessarily require changes to every other responsibility
* their limitations can be discussed independently

---

## R4: Reconfiguration Across Physical Processes

The platform shall support more than one physical process without replacing the complete control and monitoring system.

Reconfiguration may be achieved through:

* replacement of physical mechanisms
* rearrangement of structural parts
* connection of different node combinations
* assignment of different node addresses
* changes to host-side orchestration
* changes to motor speed or direction
* changes to servo angles
* changes to sensor thresholds
* changes to timing values
* changes to camera position or image-processing parameters

### Rationale

A single fixed process would not demonstrate that the platform boundaries are reusable.

The objective is not to make every physical and software element universal.

The objective is to define reusable boundaries clearly enough that process-specific elements can be replaced without reconstructing the complete system.

### Minimum Demonstration Condition

At least two meaningfully different physical processes should be implemented using common platform elements.

The current demonstrations are:

* [Amazon-style Sorting Demo](../Use_cases/Amazon-style_Sorting_Demo/)
* [Stamp Process Demo](../Use_cases/Stamp_Process_Demo/)

The two demonstrations use different:

* physical mechanisms
* actuator arrangements
* process sequences
* decision logic
* workpiece-handling methods

They reuse architectural elements such as:

* distributed control nodes
* the register-based communication model
* Raspberry Pi 5 host-side coordination
* monitoring
* persistent logging
* historical visualization

The difference between the two use cases is therefore more than a cosmetic parameter change.

---

## R5: Practical Tabletop Implementation

The system shall remain practical for individual development, assembly, demonstration, modification, and maintenance at tabletop scale.

Relevant conditions include:

* hardware and mechanisms fit within a tabletop environment
* modules remain physically accessible
* wiring can be inspected and changed
* components can be obtained through identifiable procurement channels
* development tools and documentation are reasonably accessible
* individual parts can be tested or replaced without rebuilding the complete system
* the effort required to begin testing a new idea remains proportionate to the proof-of-concept purpose
* physical mechanisms can be rearranged without requiring industrial fabrication equipment

### Rationale

The platform is intended to support early exploration.

A solution that requires production-scale infrastructure, extensive specialist preparation, or large custom machinery would conflict with that purpose.

Practicality is evaluated in relation to the project scale.

A solution may be technically appropriate for a production system while being unnecessarily costly, large, or difficult to begin using for this tabletop prototype.

---

## Constraint Summary

The requirements above are subject to the following high-level constraints.

| Constraint                     | Architectural implication                                                                   |
| ------------------------------ | ------------------------------------------------------------------------------------------- |
| Individual development         | The system must remain understandable, buildable, and maintainable by one developer         |
| Tabletop space                 | Computing, wiring, boards, mechanisms, and workpiece movement must remain compact           |
| Limited development time       | Established tools and incremental testing are preferred                                     |
| Limited fabrication capability | Commercially available mechanical parts and manufacturable PCBs are preferred               |
| Reconfiguration requirement    | Process-specific elements must be separable from reusable platform elements                 |
| Replacement requirement        | Failure or redesign of one module should not require reconstruction of the complete system  |
| Traceable procurement          | Parts should have identifiable sources, manufacturers, or practical substitutes             |
| Proof-of-concept scope         | Production-level robustness is included only where needed to test the architectural purpose |

Detailed constraints and their effects are documented in:

* `ADL-002: Design and Development Constraints`

---

## Success Conditions

The project is considered successful when the following conditions are demonstrated.

### SC-1: A Shared Physical Reference Exists

A real tabletop process can be operated and observed.

The process can be explained in terms of:

* physical inputs
* detection
* decision-making
* actuation
* module responsibilities
* available monitoring data
* known gaps between observed states and physical confirmation

### SC-2: Multiple Technical Layers Are Connected

The demonstrated system includes an operative path across:

* physical mechanism
* electronic hardware
* embedded firmware
* host-side software
* monitoring
* persistent data
* visualization

Each layer does not need to provide production-level capability.

It must exist sufficiently to make its role and interfaces inspectable.

### SC-3: Responsibilities Can Be Traced

A reviewer can identify where the main system responsibilities are implemented.

For example:

* local motor or servo behavior is implemented in a device node
* process order is implemented in host-side orchestration
* observations are persisted separately from direct device control
* Grafana visualizes stored records rather than directly controlling the mechanism
* process-specific mechanical behavior remains outside the reusable monitoring layer

### SC-4: More Than One Process Uses the Platform

At least two different physical processes are demonstrated using reusable platform elements.

The processes must require meaningful differences in physical arrangement or orchestration rather than only a cosmetic parameter change.

### SC-5: Changes Can Remain Localized

Examples exist in which a process-specific change can be made without redesigning unrelated layers.

Possible evidence includes:

* changing a physical mechanism while retaining node firmware
* changing orchestration while retaining monitoring and persistence
* adding or replacing a node without redesigning every board
* changing Grafana views without modifying device-control firmware
* changing process parameters without replacing the complete system

This does not require every change to remain perfectly isolated.

It requires the architecture to provide identifiable boundaries that reduce unnecessary propagation.

### SC-6: System Behavior Can Be Observed and Retained

Available node states or measurements can be:

* read by host-side software
* displayed during operation
* stored persistently
* reviewed later through visualization

This condition applies only to values that the implemented hardware and firmware actually expose.

It does not imply:

* complete physical-state verification
* automatic confirmation of every process result
* detection of every mechanical failure
* identification of every fault cause

### SC-7: Implementation Is Reproducible or Inspectable

The repository provides enough information to inspect the relationship between architecture and implementation.

Relevant materials include:

* source code
* firmware
* schematics
* Gerber files
* BOM information
* register documentation
* software documentation
* use-case descriptions
* photographs of the demonstrated mechanisms

Complete duplication of every physical detail is not required for this condition.

The objective is to make the implementation and its reasoning traceable.

---

## Success Evidence Matrix

| Success condition                  | Current evidence                                                                                      | Repository location                                                                                |
| ---------------------------------- | ----------------------------------------------------------------------------------------------------- | -------------------------------------------------------------------------------------------------- |
| SC-1: Shared physical reference    | Conveyor sorting and paper-stamping mechanisms operate as visible physical processes                  | [`Use_cases/`](../Use_cases/)                                                                      |
| SC-2: Connected technical layers   | Mechanisms, PCBs, Pico firmware, Pi 5 scripts, monitoring, SQLite, and Grafana are present            | Repository-wide implementation                                                                     |
| SC-3: Traceable responsibilities   | Device nodes, orchestration scripts, I2C Debugger, SQLite, and Grafana have distinct documented roles | [`Firmware/`](../Firmware/), [`Software/`](../Software/)                                           |
| SC-4: Multiple processes           | Sorting Demo and Stamp Process Demo use different mechanisms and sequences                            | [`Use_cases/`](../Use_cases/)                                                                      |
| SC-5: Localized change             | Different physical processes reuse common control and monitoring elements                             | [`Use_cases/`](../Use_cases/)                                                                      |
| SC-6: Observable retained behavior | Node observations and power-related records can be stored and visualized                              | [`Software/I2C_Debugger/`](../Software/I2C_Debugger/), [`Software/Grafana/`](../Software/Grafana/) |
| SC-7: Inspectable implementation   | Hardware files, firmware, software, BOM information, photographs, and technical notes are published   | [`Hardware/`](../Hardware/), [`Firmware/`](../Firmware/), [`Docs/`](../Docs/)                      |

---

## Requirement Traceability

The following matrix connects the requirements to the architectural decision principles defined in the [ADL overview](./README.md).

| Requirement                              | Scale fit and limited propagation | Supply-chain risk reduction | Low learning and startup cost | Tabletop fit | Reconfiguration |
| ---------------------------------------- | :-------------------------------: | :-------------------------: | :---------------------------: | :----------: | :-------------: |
| R1: Shared observable physical reference |                 ✓                 |                             |               ✓               |       ✓      |                 |
| R2: End-to-end system presence           |                 ✓                 |                             |               ✓               |       ✓      |                 |
| R3: Explicit responsibility boundaries   |                 ✓                 |                             |               ✓               |              |        ✓        |
| R4: Reconfiguration across processes     |                 ✓                 |                             |               ✓               |       ✓      |        ✓        |
| R5: Practical tabletop implementation    |                 ✓                 |              ✓              |               ✓               |       ✓      |        ✓        |

The principles do not function as independent requirements.

They provide the evaluation criteria used when selecting implementations that satisfy the requirements.

---

## Non-Goals

The project does not define success as:

* reproducing production-line throughput
* meeting industrial functional-safety requirements
* obtaining industrial certification
* providing hard real-time guarantees
* supporting long-distance industrial communication
* operating reliably in electrically harsh industrial environments
* providing complete redundancy or automatic failover
* providing production-grade authentication or cybersecurity
* automatically detecting every mechanical failure
* automatically recovering from every hardware or software fault
* integrating with a production MES, ERP, or SCADA environment
* replacing an industrial PLC or safety controller
* reproducing one specific commercial factory system

These capabilities may be necessary in a production implementation.

They are outside the minimum conditions required to validate the architectural purpose of this tabletop proof of concept.

The reasons for deferring specific capabilities are documented in:

* `ADL-011: Prototype Scope and Deferred Features`

---

## Requirement-to-Evidence Summary

| Requirement                                   | Primary implementation evidence                                                                                       |
| --------------------------------------------- | --------------------------------------------------------------------------------------------------------------------- |
| R1: Shared observable physical reference      | Sorting Demo and Stamp Process Demo                                                                                   |
| R2: End-to-end system presence                | Physical mechanisms, Hardware, Firmware, host orchestration, SQLite, and Grafana                                      |
| R3: Explicit responsibility boundaries        | Distributed firmware nodes, orchestration scripts, I2C Debugger, SQLite, and Grafana                                  |
| R4: Reconfiguration across physical processes | Two use cases with different mechanisms, sequences, and node arrangements                                             |
| R5: Practical tabletop implementation         | Compact computing hardware, custom PCBs, commercially available construction parts, and published implementation data |

> **Diagram placeholder:**
> Add a diagram connecting the five requirements to the main architectural elements and implementation evidence.

---

## Influence on Later Decisions

The requirements and success conditions in this document provide the basis for later architectural decisions.

| Later document                                   | Requirement connection                                                              |
| ------------------------------------------------ | ----------------------------------------------------------------------------------- |
| ADL-001: System Context and Goals                | Defines the system boundary required to support R1 and R2                           |
| ADL-002: Design and Development Constraints      | Expands the practical conditions affecting R4 and R5                                |
| ADL-003: Host Platform Selection                 | Selects a host capable of orchestration, monitoring, persistence, and visualization |
| ADL-004: Distributed Node Architecture           | Supports R3, R4, and localized change                                               |
| ADL-005: Communication Interface Selection       | Selects a communication method appropriate to the demonstrated scale                |
| ADL-006: Common Register Interface               | Supports replaceable nodes and common host-side access                              |
| ADL-007: Separation of Responsibilities          | Formalizes the boundaries required by R3                                            |
| ADL-008: Reconfigurable Physical Mechanisms      | Supports R4 and multiple process demonstrations                                     |
| ADL-009: Monitoring and Control Separation       | Supports R2, R3, and independent historical review                                  |
| ADL-010: Component Selection and Reproducibility | Supports the procurement and inspectability aspects of R5 and SC-7                  |
| ADL-011: Prototype Scope and Deferred Features   | Defines the boundary between success conditions and production-level requirements   |

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [Repository overview](../README.md)
* [Use Cases](../Use_cases/)
* [Hardware](../Hardware/)
* [Firmware](../Firmware/)
* [Software](../Software/)
* [Technical Documentation](../Docs/)
