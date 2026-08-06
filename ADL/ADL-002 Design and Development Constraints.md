# ADL-002: Design and Development Constraints

## Purpose of This Document

This document defines the design and development constraints that shaped the Modular Factory System.

It builds on the requirements, success conditions, system context, and goals defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)

The constraints in this document do not describe production requirements.

They describe the conditions under which the tabletop proof-of-concept platform was designed, built, tested, documented, and maintained.

The purpose is to make later architectural decisions understandable in relation to the actual development environment.

---

## Constraint Categories

The main constraints are divided into the following categories.

| ID  | Constraint category                                 |
| --- | --------------------------------------------------- |
| C1  | Individual development capacity                     |
| C2  | Limited development time                            |
| C3  | Tabletop physical and spatial limits                |
| C4  | Limited fabrication capability                      |
| C5  | Cost and replacement practicality                   |
| C6  | Supply-chain and procurement risk                   |
| C7  | Learning and startup effort                         |
| C8  | Reconfiguration and change-localization requirement |
| C9  | Debugging and fault-isolation practicality          |
| C10 | Proof-of-concept scope                              |
| C11 | Documentation and reproducibility                   |
| C12 | Safety and operating environment                    |

---

## C1: Individual Development Capacity

The system was designed and implemented by one developer.

This creates limits on:

* total implementation effort
* number of technologies that can be learned in parallel
* mechanical fabrication effort
* hardware assembly effort
* software integration effort
* documentation effort
* testing coverage
* maintenance capacity
* ability to support highly specialized tools or infrastructure

### Architectural Implication

The architecture should remain understandable and maintainable without requiring a large multidisciplinary team.

The design should avoid unnecessary specialization when a simpler approach can satisfy the proof-of-concept goals.

This constraint favors:

* clear responsibility boundaries
* reusable module patterns
* common interfaces
* familiar development environments
* incremental implementation
* inspectable physical mechanisms
* limited dependence on specialized industrial infrastructure

### Accepted Limitation

The project does not provide the same depth of validation that would be expected from a production development team with dedicated mechanical, electrical, firmware, software, safety, and operations specialists.

---

## C2: Limited Development Time

Development time was limited and had to be allocated across:

* system architecture
* circuit design
* PCB manufacturing preparation
* board assembly
* firmware implementation
* host-side software
* physical mechanism construction
* process demonstrations
* monitoring
* logging
* visualization
* documentation

The project therefore could not maximize sophistication in every layer.

### Architectural Implication

The selected implementation should provide observable system-level value before every subsystem is optimized.

This constraint favors:

* established tools
* existing libraries
* commercially available parts
* small testable increments
* reuse of common firmware concepts
* simple communication patterns
* postponement of features that do not directly validate the system goals

### Examples

* production-grade redundancy was not required before validating module boundaries
* precision custom mechanisms were not required where construction-kit parts could demonstrate the intended process
* complete security infrastructure was not required for a local tabletop prototype

### Accepted Limitation

Some implementation areas remain intentionally simple or incomplete because further refinement would have delayed validation of the complete system.

---

## C3: Tabletop Physical and Spatial Limits

The complete system had to fit within a tabletop environment.

This constraint applies to:

* host computer placement
* PCB placement
* wiring
* camera position
* actuator size
* mechanism size
* workpiece movement
* maintenance access
* visibility during demonstration
* storage and transport

### Architectural Implication

Components and mechanisms should be compact enough to coexist within a limited physical area.

The physical arrangement should also remain open enough to allow:

* visual inspection
* wiring changes
* module replacement
* mechanism adjustment
* camera repositioning
* manual recovery from jams or misalignment

This constraint favors:

* compact computing hardware
* small control nodes
* short communication distances
* small DC motors and servo motors
* lightweight construction parts
* low-mass workpieces
* simple mechanical paths

### Accepted Limitation

The tabletop implementation does not represent the dimensions, loads, cable lengths, environmental conditions, or access restrictions of a production factory.

---

## C4: Limited Fabrication Capability

The project did not assume access to industrial machining, precision tooling, injection molding, welding, or custom production equipment.

Available fabrication methods were limited to practical individual-development techniques such as:

* PCB manufacturing through an external board manufacturer
* manual soldering
* commercially available construction parts
* simple mechanical assembly
* drilling, fastening, and adjustment
* temporary or replaceable mounting methods
* limited hand fabrication

### Architectural Implication

Mechanical functions should be realizable with commercially available components and simple assembly methods.

Where possible, the design should avoid:

* precision-machined custom frames
* process-specific permanent fixtures
* mechanisms that require tight manufacturing tolerances
* fabrication methods that are difficult to reproduce individually

This constraint favors:

* modular construction plates
* commercially available gearboxes
* standard motors and servos
* reusable arms and brackets
* rubber bands or simple fasteners where appropriate for prototyping
* external PCB manufacturing using published Gerber data

### Accepted Limitation

Mechanical precision, stiffness, alignment repeatability, and long-term durability are lower than those of purpose-built industrial mechanisms.

---

## C5: Cost and Replacement Practicality

The project had to remain financially practical for individual development.

Cost was considered not only at initial purchase but also in relation to:

* replacement
* failed experiments
* damaged components
* spare parts
* repeated PCB manufacturing
* redesign
* use in multiple demonstrations

### Architectural Implication

A component should not become a disproportionate single point of financial risk.

The design should prefer components that are:

* replaceable
* available individually
* reusable across experiments
* inexpensive enough to keep spares where practical
* separable from unrelated system elements

This constraint favors:

* modular control nodes
* replaceable actuators
* separate process-specific mechanisms
* reusable host and monitoring infrastructure
* avoiding unnecessary integration of many unrelated functions into one irreplaceable assembly

### Example

A failed low-cost servo can be replaced without redesigning the complete system.

The practical response may be replacement rather than destructive failure analysis when:

* the failed part is inexpensive
* an equivalent replacement is available
* the failure does not indicate a wider system risk
* further investigation would delay a more important integration task

### Accepted Limitation

Not every failure mechanism is fully identified or characterized.

The level of investigation is selected according to its effect on the system goals.

---

## C6: Supply-Chain and Procurement Risk

Components had to be obtainable through identifiable and reasonably repeatable procurement channels.

Preferred sources include:

* established local suppliers such as Akizuki Denshi
* established distributors such as DigiKey
* manufacturers with accessible documentation
* commercially available mechanical products
* parts with practical substitutes

The project generally avoids unnecessary dependence on:

* unidentified modules
* undocumented products
* sellers with inconsistent specifications
* products whose manufacturer or lifecycle cannot be traced
* components that cannot be replaced without redesign

### Architectural Implication

Component selection should consider:

* manufacturer identity
* distributor availability
* datasheet availability
* product lifecycle visibility
* practical substitutes
* lead time
* minimum order quantity
* replacement effort
* risk of silent specification changes

This constraint favors:

* manufacturer-identified electronic components
* DigiKey-managed BOM information
* publication of board design files
* use of known commercial mechanical parts
* functional substitution where exact mechanical products are unavailable

### Accepted Limitation

No procurement path can eliminate discontinuation, regional availability changes, or market disruption.

The objective is risk reduction rather than guaranteed permanent availability.

---

## C7: Learning and Startup Effort

The time required to learn, install, configure, integrate, and debug a technology was treated as part of its architectural cost.

A technology may be technically capable but still unsuitable if its adoption effort is disproportionate to the intended proof of concept.

### Architectural Implication

Technology selection should consider:

* documentation quality
* community and example availability
* compatibility with existing knowledge
* development-tool availability
* debugging support
* ease of creating a minimal working test
* integration with other system layers
* time before observable results can be produced

This constraint favors:

* Python for host-side development
* Raspberry Pi platforms
* familiar embedded-development workflows
* SQLite for local persistence
* Grafana for historical visualization
* OpenCV for camera-based image processing
* I²C for short-distance local node communication

These technologies are not selected only because they are familiar.

Their suitability must still be evaluated against system scale, required capability, and known limitations.

### Accepted Limitation

A different technology may provide better performance, determinism, scalability, or industrial compatibility.

The selected technology only needs to provide sufficient capability for the architectural purpose.

---

## C8: Reconfiguration and Change-Localization Requirement

The system had to support different physical processes without complete redesign.

Changes should remain localized to the responsible mechanism, module, configuration, or software layer whenever practical.

### Architectural Implication

The architecture should separate:

* reusable control infrastructure
* process-specific physical mechanisms
* device-local behavior
* host-side orchestration
* monitoring
* persistence
* visualization

The system should support reconfiguration through:

* node replacement
* node-address changes
* mechanism replacement
* process-script changes
* parameter changes
* actuator and sensor rearrangement

This constraint favors:

* distributed nodes
* common register conventions
* modular hardware
* replaceable mechanisms
* independent monitoring and persistence
* use-case-specific orchestration

### Accepted Limitation

Not every new process can be implemented without hardware, firmware, or software changes.

The goal is to reduce unnecessary change propagation, not to guarantee universal compatibility.

---

## C9: Debugging and Fault-Isolation Practicality

The system includes multiple interacting physical, electrical, firmware, software, and mechanical elements.

Faults may arise from:

* wiring
* power
* communication
* firmware
* host-side software
* actuator failure
* sensor placement
* mechanical interference
* timing
* configuration
* damaged components

### Architectural Implication

The system should support practical isolation of problems.

This favors:

* separate nodes
* explicit addresses
* register inspection
* live state monitoring
* replaceable modules
* accessible wiring
* persistent event and snapshot records
* independent testing of hardware and mechanisms
* use of measurement and debugging tools where necessary

The architecture should make it possible to ask:

* which node received a command
* which value was exposed
* which software layer issued the action
* whether communication was available
* whether the physical result was independently observed

### Accepted Limitation

The current system does not automatically diagnose every fault.

Monitoring data may narrow the investigation without identifying the final physical cause.

---

## C10: Proof-of-Concept Scope

The system was intended to validate architectural concepts rather than provide a production-ready factory system.

### Included Scope

The proof of concept includes:

* real physical actuation
* sensing
* distributed device nodes
* host orchestration
* monitoring
* persistence
* visualization
* multiple process configurations
* custom PCB implementation
* published technical documentation

### Excluded or Deferred Scope

The proof of concept does not require:

* industrial functional safety
* certified guarding
* hard real-time guarantees
* long-distance industrial communication
* plant-wide networking
* complete redundancy
* automatic failover
* validated production uptime
* production-grade cybersecurity
* MES or ERP integration
* formal industrial certification
* full autonomous fault recovery

### Architectural Implication

Production-level features should be added only where they help validate a current architectural question.

Features that significantly expand scope without improving the intended proof are deferred.

### Accepted Limitation

The demonstrated system must not be interpreted as ready for production deployment.

---

## C11: Documentation and Reproducibility

The project had to remain understandable to reviewers who were not present during development.

The repository also serves as evidence of the relationship between architectural reasoning and implementation.

### Architectural Implication

The project should publish or describe:

* system architecture
* design decisions
* responsibility boundaries
* firmware
* software
* schematics
* Gerber data
* BOM information
* register conventions
* use cases
* physical component roles
* known limitations

Documentation should distinguish between:

* implemented behavior
* intended behavior
* inferred physical behavior
* observed node state
* deferred capability
* production-level requirement

### Accepted Limitation

Reproducibility does not require exact duplication of every mechanical detail.

Some mechanical parts may be replaced with functionally equivalent alternatives.

The objective is to preserve the reasoning, interfaces, roles, and sufficient implementation information.

---

## C12: Safety and Operating Environment

The system operates as a low-energy tabletop prototype under direct supervision.

The operating environment is assumed to provide:

* local physical access
* short wiring distances
* controlled indoor conditions
* manual shutdown capability
* direct observation during testing
* low-power actuators
* limited stored mechanical energy

### Architectural Implication

The project should still apply practical safety measures such as:

* accessible power removal
* reasonable current limitation
* careful wiring
* avoidance of exposed hazardous voltages
* inspection before operation
* supervision during mechanism movement
* keeping hands clear of moving parts

However, the system does not implement a certified industrial safety architecture.

### Accepted Limitation

The prototype does not provide:

* certified emergency-stop functions
* safety-rated position monitoring
* safety-rated motor shutdown
* formal hazard analysis
* validated protective guarding
* production machinery compliance

The system is intended for supervised educational and proof-of-concept use only.

---

## Constraint Interaction

The constraints do not operate independently.

Several architectural decisions result from the interaction of multiple constraints.

| Architectural direction | Contributing constraints |
|---|---|
| Compact Raspberry Pi host | Tabletop limits, individual development, startup effort, cost |
| Distributed Pico nodes | Change localization, fault isolation, replacement practicality, reconfiguration |
| I²C communication | Tabletop scale, short wiring, startup effort, implementation time |
| Common register interface | Reconfiguration, change localization, documentation, fault isolation |
| Commercial construction parts | Fabrication capability, cost, tabletop limits, supply-chain practicality |
| Custom PCBs | Wiring clarity, reproducibility, fault isolation, system integration |
| SQLite persistence | Startup effort, local operation, individual maintainability |
| Grafana visualization | Startup effort, historical review, separation of concerns |
| Multiple use cases | Reconfiguration requirement, architectural validation, traceability |
| No production safety architecture | Proof-of-concept scope, tabletop environment, development capacity |

---

## Constraint Prioritization

When constraints conflict, the following general order is used.

### 1. Preserve the Intended Architectural Demonstration

A decision should first support the system-level purpose:

* shared physical reference
* end-to-end implementation
* responsibility separation
* reconfiguration
* inspectability

### 2. Keep the System Practical to Build and Modify

A technically stronger option may be rejected if it prevents the individual tabletop prototype from being completed or changed.

### 3. Localize Risk and Replacement

A failure or redesign should affect the smallest practical part of the system.

### 4. Preserve Traceability

The reason for the selection and its consequences should remain explainable.

### 5. Defer Production-Level Optimization

Performance, robustness, certification, and scale are increased only where they support the current proof-of-concept purpose.

This prioritization is not absolute.

Specific decisions may assign different weight to each constraint depending on the role being selected.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [Repository overview](../README.md)
* [Hardware](../Hardware/)
* [Firmware](../Firmware/)
* [Software](../Software/)
* [Use Cases](../Use_cases/)
* [Technical Documentation](../Docs/)
