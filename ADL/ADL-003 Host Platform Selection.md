# ADL-003: Host Platform Selection

## Purpose of This Document

This document explains the selection of the Raspberry Pi 5 as the host platform for the Modular Factory System.

The host platform coordinates multiple distributed device nodes and supports the software required to operate, observe, record, and review the tabletop process.

This decision is evaluated in relation to the requirements, system context, and development constraints defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)

This document focuses on the host platform.

The selection of distributed Raspberry Pi Pico nodes, I²C communication, and the common register interface is documented separately.

---

## Decision Summary

The Raspberry Pi 5 was selected as the host platform.

The host is responsible for supporting:

* communication with distributed device nodes
* multi-node process orchestration
* process-specific decision logic
* camera-based image processing
* live monitoring
* persistent SQLite logging
* Grafana-based historical visualization
* local development, testing, and debugging

The Raspberry Pi 5 provides sufficient general-purpose computing capability while remaining compact and practical for an individually developed tabletop proof of concept.

The selection prioritizes complete system integration over maximum performance in any single function.

---

## Decision Context

The system required a host capable of connecting physical processes to higher-level software.

The host had to operate between:

* distributed actuator and sensor nodes
* process-specific orchestration
* camera input
* monitoring software
* persistent data storage
* historical visualization

The host therefore required more capability than a simple device controller.

At the same time, the host did not need to provide:

* certified industrial control
* hard real-time behavior
* plant-wide deployment
* production-grade redundancy
* safety-rated operation
* large-scale data processing
* enterprise-system integration

The selected platform had to fit the scale and purpose of the tabletop proof of concept.

---

## Required Host Responsibilities

### Node Communication

The host must communicate with multiple device nodes through the selected local communication interface.

The host must be able to:

* address individual nodes
* read exposed registers
* write commands and configuration values
* coordinate multiple node types
* inspect node responses
* support debugging of communication and register behavior

---

### Process Orchestration

The host must coordinate multiple physical modules as one process.

This includes:

* sequencing actions
* applying timing values
* reading sensor information
* selecting process branches
* issuing actuator commands
* repeating defined process cycles
* applying use-case-specific parameters

The orchestration layer does not need to provide hard real-time guarantees.

It must provide sufficient timing and coordination for the demonstrated tabletop processes.

---

### Process-Specific Computation

Some processes require computation that is not appropriate for a small device-local control node.

The sorting demonstration includes camera-based visible-color classification.

The host therefore needs to support:

* camera access
* image acquisition
* image processing
* process-specific decision logic
* integration of the resulting decision with physical orchestration

---

### Monitoring and Debugging

The host must support software that presents available node states and measurements during development and operation.

This includes:

* node inspection
* register reading
* state display
* event inspection
* snapshot inspection
* power-related measurement display
* troubleshooting of node and communication behavior

The monitoring responsibility is logically separate from process orchestration even when both run on the same host.

---

### Persistent Logging

The host must store selected system observations for later review.

The persistence layer must support:

* event records
* state snapshots
* measurement records
* local operation
* inspection without a separate database server
* use by later visualization tools

---

### Historical Visualization

The host must support historical visualization of persisted records.

This includes:

* state timelines
* event history
* power-related records
* comparison of recorded behavior over time

Visualization must remain outside the direct device-control path.

A failure or change in visualization should not be required for basic physical control to continue.

---

## Selection Criteria

The host platform was evaluated against the following criteria.

| ID  | Selection criterion                                     |
| --- | ------------------------------------------------------- |
| H1  | Sufficient general-purpose computing capability         |
| H2  | Support for Python-based development                    |
| H3  | Support for camera and image processing                 |
| H4  | Support for local persistence and visualization         |
| H5  | Access to local hardware interfaces                     |
| H6  | Compact tabletop form factor                            |
| H7  | Low learning and startup effort                         |
| H8  | Practical cost and replacement                          |
| H9  | Availability of documentation and community examples    |
| H10 | Ability to integrate multiple software responsibilities |
| H11 | Independence from production infrastructure             |
| H12 | Suitability for supervised proof-of-concept operation   |

---

## Comparison Summary

The alternatives were compared according to their suitability for the current tabletop proof of concept.

| Alternative               | Main strengths                                                                    | Main limitations                                                                                            | Fit for current PoC |
| ------------------------- | --------------------------------------------------------------------------------- | ----------------------------------------------------------------------------------------------------------- | :-----------------: |
| Raspberry Pi 5            | Linux, Python, OpenCV, SQLite, Grafana, local hardware access, compact size       | No hard real-time guarantees or industrial robustness                                                       |        Strong       |
| Desktop or laptop         | High performance and broad software compatibility                                 | Less compact and less self-contained                                                                        |       Moderate      |
| Microcontroller-only host | Compact, low power, and direct hardware control                                   | Weak fit for Grafana, SQLite, camera processing, and graphical monitoring                                   |         Weak        |
| Industrial PC             | Robust general-purpose platform suitable for industrial environments              | High cost and capabilities beyond current requirements                                                      |         Weak        |
| PLC                       | Deterministic industrial control and established maintenance model                | Higher cost, greater setup effort, and weak fit for Python-based image and data workflows                   |         Weak        |
| AI-focused edge computer  | Strong image-processing and machine-learning capability                           | Additional cost and unnecessary acceleration for current classification needs                               |       Moderate      |
| FPGA-based host           | Deterministic parallel processing, precise timing, and custom hardware interfaces | High learning and integration effort; general-purpose services require an additional processing environment |         Weak        |

The ratings describe suitability for this project.

They are not general performance rankings.

A different system scale or production requirement could lead to a different selection.

---

## Evaluation of Selection Criteria

### H1: General-Purpose Computing Capability

The host must support multiple software responsibilities on one platform.

These responsibilities include:

* orchestration
* communication
* camera processing
* graphical monitoring
* database access
* visualization services
* development and debugging tools

A small microcontroller could perform selected control functions but would not provide the same practical environment for the complete software stack.

The selected host therefore needed to provide a general-purpose operating system and sufficient computing resources for concurrent development services.

---

### H2: Python-Based Development

Python was suitable for the host-side project responsibilities.

It supports:

* rapid implementation
* hardware-interface libraries
* process orchestration
* OpenCV integration
* SQLite access
* PyQt-based monitoring software
* readable use-case scripts
* incremental testing

The host platform therefore needed a practical Python environment with access to the local hardware interface.

---

### H3: Camera and Image Processing

The sorting use case requires camera input and visible-color classification.

The host therefore needed to support:

* camera connectivity
* image capture
* OpenCV
* sufficient processing for the selected image size and classification method
* integration between classification results and node commands

A platform that required a separate computer for image processing would increase wiring, configuration, and responsibility-boundary complexity.

---

### H4: Persistence and Visualization

The system uses SQLite for persistent records and Grafana for historical visualization.

The host therefore needed to support:

* local file storage
* SQLite database generation
* background services
* Grafana installation and execution
* local network access to dashboards
* long-running supervised operation during demonstrations

A Linux-capable general-purpose host provides a practical environment for these responsibilities.

---

### H5: Local Hardware Interfaces

The host must communicate with nearby distributed device nodes.

The system uses short-distance local communication within a tabletop environment.

The host therefore needed accessible hardware interfaces suitable for direct integration with the prototype.

The Raspberry Pi platform provides local hardware connectivity while also supporting general-purpose Linux software.

---

### H6: Tabletop Form Factor

The host had to fit beside:

* physical mechanisms
* custom PCBs
* wiring
* camera equipment
* actuators and sensors

A conventional desktop computer would provide sufficient computing capability but would not fit the physical concept of a compact self-contained tabletop platform as well.

A laptop could be used during development, but relying on it as the permanent host would make the demonstrated system less self-contained.

---

### H7: Learning and Startup Effort

The host platform needed to provide a short path from initial setup to observable results.

Relevant factors included:

* operating-system availability
* Python support
* GPIO and I²C examples
* camera support
* package management
* remote access
* debugging information
* community documentation
* availability of compatible accessories

The Raspberry Pi ecosystem reduces the need to create a custom Linux platform or learn a specialized industrial environment before validating the architecture.

---

### H8: Cost and Replacement

The host had to remain financially practical for individual development.

The selection had to consider:

* initial purchase
* replacement in case of failure
* continued availability
* reuse in later projects
* cost relative to the complete tabletop system

A production industrial computer would provide greater robustness but would introduce cost and infrastructure beyond the current proof-of-concept requirements.

---

### H9: Documentation and Ecosystem

The selected platform needed sufficient documentation for:

* operating-system setup
* local hardware access
* Python development
* camera integration
* remote operation
* package installation
* troubleshooting
* storage configuration

The Raspberry Pi ecosystem provides established documentation and a large body of practical examples.

This lowers integration risk for an individually developed system.

---

### H10: Software Integration

The host needed to run several logically separate responsibilities.

These include:

* orchestration scripts
* monitoring software
* SQLite logging
* Grafana
* camera-processing code

Using one Linux-capable host allows these responsibilities to coexist without requiring several separate computers.

Logical separation is maintained through software boundaries, even though the responsibilities share one physical platform.

---

### H11: Independence from Production Infrastructure

The host needed to operate without requiring production factory infrastructure.

The platform should support local development and demonstration using:

* local storage
* local device communication
* locally executed orchestration
* locally available monitoring
* locally hosted or accessible visualization

The Raspberry Pi 5 allows the complete proof-of-concept host environment to operate without requiring an MES, industrial network, external database server, or plant-wide computing infrastructure.

---

### H12: Suitability for Supervised Proof-of-Concept Operation

The host was intended for supervised development and demonstration rather than unattended production operation.

The platform therefore needed to support:

* direct developer access
* incremental software changes
* manual startup and shutdown
* interactive debugging
* local observation
* recovery through developer intervention

The Raspberry Pi 5 is suitable for this operating model.

It does not provide the reliability, safety, environmental protection, or lifecycle support expected from a production industrial controller.

---

## Considered Alternatives

### Alternative A: Conventional Desktop or Laptop Computer

A conventional computer could support:

* Python
* OpenCV
* SQLite
* Grafana
* graphical development tools
* high computing performance

#### Advantages

* strong general-purpose performance
* familiar development environment
* easy graphical interaction
* broad software compatibility
* practical for initial software development

#### Disadvantages

* less compact as a permanent system component
* less self-contained as a tabletop control platform
* direct hardware integration may require additional adapters
* the development computer becomes a required part of the demonstration setup
* replacement or relocation can affect the demonstration setup

#### Evaluation

A desktop or laptop is useful as a development terminal.

It was not selected as the primary embedded host because the Raspberry Pi 5 provides a more compact and dedicated platform for the complete demonstration.

---

### Alternative B: Microcontroller-Only Host

A higher-capability microcontroller could coordinate multiple device modules.

#### Advantages

* low power consumption
* compact size
* direct hardware-interface access
* predictable control behavior
* low operating-system overhead

#### Disadvantages

* less suitable for Grafana
* less suitable for SQLite-based host workflows
* limited graphical monitoring support
* camera and OpenCV integration would be more constrained
* development of higher-level services would require more custom implementation
* orchestration, monitoring, persistence, and visualization would likely need to be divided across additional systems

#### Evaluation

A microcontroller-only host would be appropriate for a narrower embedded controller.

It was not selected because the project required one platform to connect physical control with general-purpose software, logging, and visualization.

---

### Alternative C: Industrial PC

An industrial PC could provide a Linux or Windows environment with stronger mechanical and electrical robustness.

#### Advantages

* higher robustness
* industrial mounting options
* greater storage and interface flexibility
* better suitability for long-term industrial deployment
* potential support for industrial expansion hardware

#### Disadvantages

* higher cost
* larger physical footprint
* greater setup and sourcing burden
* capabilities beyond the tabletop proof-of-concept requirements
* weaker alignment with individual-development cost constraints

#### Evaluation

An industrial PC would be a reasonable candidate for a production-oriented system.

It was not selected because its robustness and deployment capabilities were not required to validate the current architecture.

---

### Alternative D: PLC-Based Host

A PLC could coordinate physical modules using an industrial control environment.

#### Advantages

* established industrial control model
* robust digital and analog I/O
* deterministic control options
* industrial maintenance familiarity
* potential integration with safety and production equipment

#### Disadvantages

* higher hardware and software cost
* greater learning and setup effort
* camera processing may require separate equipment
* Grafana and SQLite integration may require additional systems
* less convenient for rapidly changing Python-based experimental logic
* production-oriented capabilities exceed the current scope

#### Evaluation

A PLC would be appropriate where industrial reliability, deterministic behavior, standardized maintenance, or production integration is a primary requirement.

It was not selected because the current project prioritizes rapid architectural exploration and integration across control, software, data, and visualization.

---

### Alternative E: AI-Focused Edge Computer

An AI-focused edge computer could provide stronger image-processing or machine-learning capability.

#### Advantages

* higher capability for advanced image processing
* hardware acceleration for selected workloads
* suitability for future machine-learning experiments

#### Disadvantages

* higher cost
* additional platform-specific learning
* increased software and driver complexity
* unnecessary capability for simple visible-color classification
* weaker fit with the goal of minimizing startup effort

#### Evaluation

An AI-focused platform would become relevant if advanced vision or machine-learning workloads became central to the system.

It was not selected because the current image-processing requirement can be satisfied without specialized acceleration.

---

### Alternative F: FPGA-Based Host

An FPGA-based platform could implement selected communication, control, and image-processing functions directly in programmable logic.

Depending on the device and board configuration, an FPGA platform may also include an embedded general-purpose processor capable of running software.

#### Advantages

* deterministic and highly parallel processing
* very low-latency handling of selected communication and control functions
* precise timing behavior
* potential implementation of custom communication interfaces
* potential acceleration of image-processing pipelines
* compact board-level implementation compared with a conventional desktop or laptop
* close integration between hardware interfaces and processing logic

#### Disadvantages

* VHDL, Verilog, or another hardware description language would need to be learned
* development and debugging workflows would require additional preparation
* availability and compatibility of the required IDE and toolchain had not been confirmed
* image processing would require substantial custom implementation or a suitable processor-equipped FPGA platform
* SQLite, Grafana, and other Linux-oriented services would likely require a separate processor or operating environment
* integration between programmable logic and general-purpose software would increase system complexity
* available timing performance would exceed the requirements of the current tabletop processes

#### Evaluation

An FPGA could provide lower latency and more deterministic behavior than the Raspberry Pi 5 for selected communication, control, and image-processing functions.

However, these capabilities were not required by the current tabletop proof of concept.

The project also required:

* Python-based orchestration
* SQLite persistence
* Grafana visualization
* camera-processing integration
* graphical monitoring and debugging

Implementing the complete host role with an FPGA would therefore require one of the following:

* an FPGA platform with an embedded general-purpose processor
* a soft processor implemented in programmable logic
* a separate computer for persistence and visualization

This would increase learning, development, debugging, and integration effort.

The FPGA option was therefore not selected as the primary host platform.

It may become relevant if future requirements include:

* deterministic high-speed control
* custom communication timing
* parallel signal processing
* hardware-accelerated image processing
* timing requirements that cannot be satisfied by a general-purpose operating system

---

## Selected Platform

The Raspberry Pi 5 was selected because it provides a balanced combination of:

* general-purpose Linux capability
* Python support
* OpenCV support
* local hardware-interface access
* SQLite compatibility
* Grafana compatibility
* compact size
* practical procurement
* established documentation
* manageable learning and startup effort
* sufficient performance for the demonstrated processes

The decision does not identify the Raspberry Pi 5 as the most capable alternative in every category.

Instead, it provides the strongest overall fit for the complete host responsibility within the current project constraints.

---

## Role of the Raspberry Pi 5

The Raspberry Pi 5 acts as the central host but not as the owner of every system responsibility.

Its physical platform hosts several logically distinct software responsibilities.

| Responsibility        | Host-side role                                                   |
| --------------------- | ---------------------------------------------------------------- |
| Communication         | Reads and writes distributed-node registers                      |
| Orchestration         | Defines process order and coordinates multiple nodes             |
| Process decisions     | Applies camera or sensor information to process-specific logic   |
| Monitoring            | Displays available node states and measurements                  |
| Persistence           | Stores selected events, snapshots, and measurements              |
| Visualization support | Runs or supplies stored data to Grafana                          |
| Development support   | Provides a platform for testing and debugging host-side behavior |

Device-local actuator and sensor behavior remains assigned to the distributed Raspberry Pi Pico nodes.

The Raspberry Pi 5 does not directly replace device-local responsibilities.

---

## Consequences

### Positive Consequences

The selection provides:

* one compact host for multiple software responsibilities
* a practical Python development environment
* direct integration with distributed nodes
* support for camera-based processing
* local logging and visualization
* remote or local software access
* a self-contained tabletop demonstration
* reuse of the same host across multiple physical processes
* a short path from software changes to physical testing

---

### Negative Consequences

The selection also introduces limitations.

The Raspberry Pi 5:

* is not an industrial controller
* does not provide certified functional safety
* does not guarantee hard real-time behavior
* depends on a general-purpose operating system
* may require operating-system and package maintenance
* is not designed for harsh industrial environments
* may require careful power, storage, and shutdown management
* can become a shared failure point for orchestration, monitoring, and logging
* may allow logically separate responsibilities to become tightly coupled if software boundaries are not maintained

---

### Context-Dependent Consequences

Running multiple responsibilities on one host simplifies the physical system but increases the importance of logical separation.

For example:

* orchestration and monitoring can share one computer
* SQLite and Grafana can operate locally
* camera processing can be integrated without a second platform

However:

* one host failure can affect several services
* software resource usage may influence other responsibilities
* changes to the host environment may affect multiple applications
* responsibility boundaries must be documented rather than inferred from physical separation

These consequences are acceptable within the supervised proof-of-concept environment.

---

## Risks and Mitigations

| Risk                                  | Current mitigation                                                                   |
| ------------------------------------- | ------------------------------------------------------------------------------------ |
| Host becomes a shared failure point   | Device-local behavior remains separated into distributed nodes                       |
| General-purpose OS timing variation   | Processes are designed without hard real-time guarantees                             |
| Software dependency conflicts         | Responsibilities are documented and tested incrementally                             |
| Storage corruption or unsafe shutdown | Supervised operation and controlled power removal                                    |
| Excessive host-side responsibility    | Logical separation between orchestration, monitoring, persistence, and visualization |
| Resource competition                  | Current workloads remain limited to tabletop PoC scale                               |
| Platform replacement                  | Host-facing interfaces and responsibilities are documented                           |
| Production misinterpretation          | Industrial reliability and safety are explicitly outside scope                       |

---

## Replacement Boundary

The Raspberry Pi 5 is the selected implementation, but the architecture should not depend on its identity more than necessary.

A replacement host would need to provide:

* access to the selected node communication interface
* support for host-side orchestration
* support for the required camera-processing workload
* persistent local storage
* compatibility with monitoring and visualization responsibilities
* sufficient computing resources
* practical operation within the physical environment

Possible future replacement platforms may include:

* another Linux-capable single-board computer
* a conventional computer
* an industrial PC
* a processor-equipped FPGA platform
* a divided architecture using separate control and visualization hosts

Replacement may require software and interface adaptation.

The goal is not transparent platform interchangeability.

The goal is to keep the host role understandable enough that a replacement can be evaluated without redesigning the complete physical system.

---

## Validation Through Current Implementation

The current implementation demonstrates that the Raspberry Pi 5 can support the required host role.

Evidence includes:

* host-side coordination of distributed I²C nodes
* camera-based sorting decisions
* process-specific orchestration scripts
* I²C monitoring software
* SQLite event and snapshot logging
* Grafana historical visualization
* reuse across the sorting and stamping demonstrations

This evidence confirms suitability for the current tabletop proof of concept.

It does not establish suitability for production-scale deployment.

---

## Decision Boundary

The selection of the Raspberry Pi 5 establishes the host implementation for the current platform.

This decision does not determine:

* the internal architecture of distributed device nodes
* the detailed communication protocol
* the common register layout
* the ownership of every software function
* production deployment hardware
* future host-replacement strategy

Those questions are handled in later architectural decisions.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [Repository overview](../README.md)
* [System Architecture](../Docs/System_Architecture/)
* [Software](../Software/)
* [Use Cases](../Use_cases/)
