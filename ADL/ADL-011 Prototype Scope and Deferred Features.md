ADL-011: Prototype Scope and Deferred Features

Purpose of This Document

This document defines the intended scope of the Modular Factory System proof of concept and explains why selected capabilities were intentionally deferred.

This decision follows the previously established architecture defined in:

* ADL-000: Requirements Definition and Success Conditions
* ADL-001: System Context and Goals
* ADL-002: Design and Development Constraints
* ADL-003: Host Platform Selection
* ADL-004: Distributed Node Architecture
* ADL-005: Communication Interface Selection
* ADL-006: Common Register Interface
* ADL-007: Separation of Responsibilities
* ADL-008: Reconfigurable Physical Mechanisms
* ADL-009: Monitoring and Control Separation
* ADL-010: Component Selection and Reproducibility

The earlier documents explain individual architectural choices.

This document defines the boundary around the complete proof of concept.

⸻

Decision Summary

The Modular Factory System is intentionally a tabletop proof-of-concept platform.

Its purpose is to demonstrate:

* physical actuation
* sensing
* distributed device control
* host-side orchestration
* responsibility separation
* reconfigurable process implementation
* live monitoring
* persistent logging
* historical visualization
* custom PCB implementation
* reproducible technical documentation

The project does not attempt to implement every capability that would be required for a production factory system.

Features are deferred when they:

* do not directly improve validation of the current architectural goals
* require disproportionate implementation effort
* primarily address production scale rather than tabletop scale
* require infrastructure outside the intended environment
* introduce complexity before the underlying architectural boundary has been validated

Deferral is therefore treated as an architectural scope decision rather than as an accidental absence of implementation.

⸻

Decision Context

The system spans several engineering domains:

* mechanics
* electronics
* embedded firmware
* device communication
* host software
* computer vision
* monitoring
* persistence
* visualization
* manufacturing documentation

A complete production implementation of all of these areas would require substantially more:

* engineering effort
* validation
* infrastructure
* testing
* safety analysis
* security design
* maintenance processes
* operational procedures

The project was developed as an individual tabletop proof of concept.

Attempting to implement production-level depth in every layer would have prevented completion of the end-to-end architectural demonstration.

The project therefore prioritizes:

Complete system-level architectural evidence

over:

Maximum maturity of every subsystem

⸻

Scope Goals

The scope strategy was selected to support the following goals.

ID	Goal
PS1	Demonstrate an end-to-end physical system
PS2	Validate responsibility boundaries
PS3	Validate reuse across different processes
PS4	Use real hardware rather than software-only simulation
PS5	Make system state observable
PS6	Preserve historical evidence
PS7	Demonstrate custom hardware integration
PS8	Keep the project achievable by one developer
PS9	Avoid production-level features without a current validation need
PS10	Make deferred capability explicit
PS11	Preserve extension paths where practical
PS12	Avoid implying production readiness

⸻

PS1: End-to-End System Demonstration

The prototype must demonstrate a complete path from physical process to software observation.

The current architecture includes:

Physical mechanism
        ↓
Hardware boards
        ↓
Pico firmware nodes
        ↓
I²C register interface
        ↓
Raspberry Pi 5 host
        ↓
Monitoring and persistence
        ↓
Grafana visualization

The project therefore prioritizes having each major architectural layer represented and connected.

A more sophisticated isolated subsystem is less valuable to the current objective if the complete system cannot be demonstrated.

⸻

PS2: Responsibility Boundary Validation

The prototype must provide enough implementation to evaluate the major responsibility boundaries.

Examples include:

* host orchestration versus device-local behavior
* common firmware versus node-specific firmware
* control versus monitoring
* monitoring versus historical visualization
* reusable infrastructure versus process-specific mechanisms

The purpose is not only to show that individual components operate.

The system should demonstrate that those responsibilities can interact through explicit interfaces.

⸻

PS3: Multiple Process Configurations

One process alone would provide limited evidence of reconfigurability.

The project therefore includes multiple substantially different physical demonstrations.

Current examples are:

* color-based workpiece sorting
* repeated paper stamping

The demonstrations use different:

* mechanisms
* actuator combinations
* process sequences
* physical materials
* orchestration logic

while reusing substantial portions of the same control and monitoring architecture.

This provides stronger evidence of reuse than a single fixed machine.

⸻

PS4: Real Physical Hardware

The architectural demonstration includes actual:

* motors
* servos
* sensors
* mechanical mechanisms
* PCB hardware
* power distribution
* physical workpieces or process materials

The system is therefore not limited to:

* software simulation
* dashboard mock-ups
* abstract block diagrams

Mock operation remains useful for software testing.

However, physical implementation is required to expose integration issues that do not appear in software-only models.

⸻

PS5: System Observability

The proof of concept includes enough monitoring to inspect:

* node presence
* lifecycle state
* communication condition
* state transitions
* selected feedback
* monitoring sessions
* voltage
* current
* calculated power

This supports system-level debugging and explanation.

The monitoring architecture does not attempt to provide complete physical-process verification.

⸻

PS6: Historical Evidence

The project includes persistent monitoring records because a live demonstration alone provides limited evidence after execution.

SQLite preserves selected observations.

Grafana provides historical presentation.

This allows later review of:

* state sequences
* session behavior
* event history
* electrical trends

Historical monitoring is included because it directly supports architecture validation and debugging.

⸻

PS7: Custom Hardware Integration

The prototype includes custom PCB modules rather than relying entirely on breadboards or opaque commercial modules.

This allows the project to demonstrate:

* defined electrical responsibility boundaries
* documented connectors
* published schematics
* Gerber fabrication data
* component procurement references

The objective is not production PCB qualification.

The objective is to demonstrate that the proposed hardware architecture can be implemented as reproducible physical modules.

⸻

PS8: Individual Development Capacity

The project must remain achievable and maintainable by one developer.

This directly limits:

* number of technologies
* implementation depth
* mechanical fabrication
* test coverage
* deployment infrastructure
* documentation workload

A technically valuable feature may still be deferred when its implementation cost prevents completion of more important architectural evidence.

⸻

PS9: Production Features Are Not Default Requirements

Production-level features are not added automatically.

Examples include:

* redundancy
* failover
* large-scale deployment
* industrial communication infrastructure
* production cybersecurity
* safety certification

Such features should be implemented only when they answer a specific architectural question relevant to the proof of concept.

This avoids using production realism as an unlimited scope-expansion mechanism.

⸻

PS10: Explicit Deferred Capability

A feature that is intentionally deferred should be distinguishable from:

* an implemented feature
* an architectural requirement
* a known defect
* an undocumented omission

The project documentation therefore identifies future work and current limitations explicitly where practical.

This makes it possible to state:

Not implemented because it is outside current scope

rather than leaving the reader to infer:

Not implemented because the architecture cannot support it

⸻

PS11: Preserve Extension Paths

A deferred feature does not need to be implemented immediately for the architecture to leave space for it.

Examples include:

* new node types
* new actuator boards
* new sensor interfaces
* different host orchestration logic
* improved harnessing
* additional use cases
* alternative monitoring infrastructure

The project favors boundaries that allow these capabilities to be added without requiring complete redesign.

This does not guarantee that every future extension will fit the current architecture unchanged.

⸻

PS12: Production Readiness Boundary

The current system must not be interpreted as production-ready equipment.

The project is intended for:

* education
* prototyping
* architectural experimentation
* proof-of-concept demonstration

It is not validated as:

* industrial machinery
* safety-certified control equipment
* production automation infrastructure
* enterprise manufacturing infrastructure

This distinction is an explicit architectural boundary.

⸻

Included Prototype Scope

The current proof of concept includes the following major capabilities.

Area	Included capability
Physical process	Real tabletop mechanisms
Actuation	DC motors and servo motors
Sensing	Photo-reflector-based sensing
Vision	Camera-based color classification in the sorting demo
Distributed control	Raspberry Pi Pico device nodes
Communication	Shared I²C register interface
Host	Raspberry Pi 5
Orchestration	Use-case-specific host-side process logic
Monitoring	I2C Debugger
Persistence	SQLite
Visualization	Grafana
Electrical observation	Main and branch power monitoring
Hardware	Custom modular PCBs
Reconfiguration	Different physical process configurations
Reproducibility	Schematics, Gerbers, procurement references, documentation

⸻

Explicitly Deferred Production Capabilities

The following capabilities are outside the required scope of the current proof of concept.

Industrial Functional Safety

The project does not provide:

* certified safety functions
* safety-rated actuator shutdown
* safety-rated position monitoring
* certified guarding
* validated machinery-safety compliance

The current operating assumption is supervised, low-energy tabletop use.

⸻

Hard Real-Time Guarantees

The architecture does not provide formal hard real-time guarantees.

The current host and monitoring environment uses general-purpose Linux and polling-based software.

The firmware architecture is sufficient for the demonstrated tabletop processes but is not presented as a deterministic industrial motion-control platform.

⸻

Long-Distance Industrial Communication

The current I²C communication model is intended for short-distance local communication.

The project does not currently require:

* fieldbus networking
* plant-wide Ethernet
* long-distance differential communication
* distributed industrial network infrastructure

A different communication architecture should be reconsidered if physical scale changes substantially.

⸻

Full Redundancy and Automatic Failover

The current prototype does not provide:

* redundant hosts
* redundant buses
* redundant controllers
* redundant databases
* automatic process failover
* hot standby infrastructure

The current goal is architecture validation, not production availability.

⸻

Production-Grade Cybersecurity

The current system does not implement a production cybersecurity architecture.

The proof of concept assumes a local or trusted tabletop environment.

It does not currently provide a complete design for:

* identity management
* role-based access control
* network segmentation
* certificate lifecycle
* secure remote management
* production security monitoring

These capabilities require a deployment context that is outside the present scope.

⸻

MES and ERP Integration

The prototype does not currently integrate with:

* Manufacturing Execution Systems
* Enterprise Resource Planning systems
* production scheduling infrastructure
* plant-wide traceability systems

The current data path terminates at local persistence and visualization.

Enterprise integration would represent a separate architectural concern.

⸻

Full Autonomous Fault Recovery

The current system does not attempt to diagnose and recover automatically from every fault.

Monitoring may help identify:

* communication loss
* firmware states
* electrical behavior
* event sequences

Human investigation may still be required.

Automatic recovery would need explicit rules for:

* fault classification
* safe retry
* restart behavior
* process recovery
* physical state validation

Those behaviors are not required for the current proof of concept.

⸻

Formal Industrial Certification

The project does not currently target formal certification for:

* machinery safety
* electromagnetic compatibility
* industrial environmental qualification
* production regulatory compliance

The hardware and mechanisms are prototype implementations.

⸻

Current Future Work

The repository identifies several possible future improvements.

These currently include:

* flow-editor style orchestration UI
* more reusable module slots
* more standardized harnesses
* additional use cases using the existing control modules
* recruiter-oriented technical walkthrough material
* more detailed troubleshooting and setup documentation

These items are not all equivalent architectural priorities.

Some improve the technical platform.

Others improve usability, documentation, or communication.

They remain future work because the existing implementation already demonstrates the core architectural path.

⸻

Flow-Editor Style Orchestration UI

A graphical flow editor could allow process logic to be assembled through higher-level visual composition rather than editing process scripts directly.

Potential value includes:

* easier sequence editing
* clearer process visualization
* lower entry cost for users unfamiliar with the orchestration source code
* possible reuse of process blocks

However, implementing a useful flow editor requires decisions about:

* available node discovery
* module capability representation
* sequence semantics
* branching
* looping
* validation
* error handling
* execution state

The current architecture can demonstrate host-side orchestration without requiring this additional abstraction.

The flow editor is therefore deferred.

⸻

Standardized Harnesses and Module Slots

The current hardware already uses modular boards and organized connectors.

Further standardization could improve:

* setup
* replacement
* connector consistency
* wiring clarity
* repeated reconfiguration

Possible future work includes:

* more standardized harnesses
* more consistent mounting
* reusable module slots
* stronger connector keying

These improvements may reduce setup effort but are not necessary to validate the current responsibility and communication boundaries.

⸻

Additional Use Cases

Additional process demonstrations could provide more evidence of reuse.

However, each additional use case introduces:

* mechanism construction
* integration
* orchestration
* calibration
* testing
* documentation

The existing sorting and stamping demonstrations already provide two substantially different process configurations.

Additional use cases are therefore useful extensions rather than requirements for validating the current architecture.

⸻

Documentation and Presentation Improvements

Possible future documentation work includes:

* more detailed troubleshooting guidance
* more detailed setup guidance
* recruiter-oriented technical walkthrough material

These improvements increase accessibility.

They do not fundamentally alter the architecture.

They can therefore continue independently after the principal architecture documentation is complete.

⸻

Deferred Feature Decision Rule

A feature should normally remain deferred when:

Implementation cost
        >
Architectural evidence gained

for the current project stage.

A deferred feature should be reconsidered when at least one of the following becomes true:

* it is required to test a new architectural hypothesis
* an existing limitation blocks a relevant use case
* repeated manual work becomes a meaningful bottleneck
* the project moves toward a larger deployment scale
* the operating environment changes
* a production requirement becomes part of the actual goal
* the feature materially improves fault isolation, safety, or reproducibility for a new intended use

⸻

Deferred Does Not Mean Rejected

A deferred feature may still be technically valuable.

The project distinguishes:

Not selected for the current architecture

from:

Not required yet

For example:

* industrial Ethernet may be appropriate at greater distance
* stronger physical separation may be appropriate at production scale
* safety-rated control may be mandatory for real industrial machinery
* an external database may be appropriate for multiple systems
* a flow editor may become valuable after module discovery and orchestration semantics are more mature

The current decision only states that these capabilities are not required to validate the present proof of concept.

⸻

Risks and Mitigations

Risk	Current mitigation
Prototype scope expands indefinitely	Require clear relation to current architectural goals
Deferred feature is mistaken for forgotten work	Document future work and exclusions
PoC is mistaken for production-ready equipment	Explicitly state production boundaries
Too little implementation weakens architectural evidence	Maintain real end-to-end physical demonstrations
Too much subsystem refinement prevents completion	Prioritize system-level validation
New use cases require large redesign	Preserve modular boundaries and extension points
Production assumptions leak into current design	Evaluate features against actual tabletop requirements
Future work list becomes a commitment	Treat items as candidates rather than guaranteed roadmap
Simplified implementation is mistaken for inability	Record why higher-complexity alternatives were deferred

⸻

Comparison Summary

Alternative	Main strengths	Main limitations	Fit for current PoC
Build only a minimal isolated demo	Lowest implementation effort	Weak evidence of end-to-end architecture	Weak
Implement end-to-end PoC with explicit limits	Strong architectural evidence with manageable scope	Some production capabilities remain deferred	Strong
Add selected production features during PoC	Provides more realism	High risk of scope expansion	Moderate
Attempt production-ready implementation	Broad capability and robustness	Disproportionate effort and validation requirements	Weak

The ratings describe suitability for the current project.

They are not general rankings of development strategies.

⸻

Considered Alternatives

Alternative A: Minimal Isolated Demonstration

The project could demonstrate only one subsystem.

Examples include:

* one motor controlled from a Raspberry Pi
* one I²C node
* one monitoring dashboard
* one mechanical process without monitoring

Advantages

* low development effort
* fast initial result
* easy debugging
* limited hardware requirement

Disadvantages

* weak evidence of integration
* responsibility boundaries remain largely theoretical
* reuse cannot be demonstrated convincingly
* monitoring and control separation cannot be evaluated end to end
* cross-domain issues remain hidden

Evaluation

This approach is useful during early subsystem development.

It is insufficient as the final architectural proof of concept.

⸻

Alternative B: End-to-End Proof of Concept with Explicit Limits

The selected approach implements enough of each major layer to demonstrate the complete architecture.

Advantages

* demonstrates real system integration
* exposes cross-domain issues
* validates responsibility boundaries
* demonstrates reuse across processes
* remains achievable at tabletop scale
* allows production features to remain deferred explicitly

Disadvantages

* individual subsystems remain less mature than production equivalents
* some limitations require manual supervision
* architecture must clearly document what is and is not demonstrated

Evaluation

This provides the strongest fit for the current project.

It was selected.

⸻

Alternative C: Add Selected Production Features During the PoC

The project could progressively add features such as:

* redundant communication
* stronger cybersecurity
* safety interlocks
* industrial networking
* automatic recovery

Advantages

* closer to production behavior
* more failure scenarios can be evaluated
* selected production concerns become visible earlier

Disadvantages

* significantly larger scope
* more dependencies
* longer integration time
* harder to determine when the prototype is complete
* implementation effort may exceed the architectural evidence gained

Evaluation

Selected production features may be appropriate when they answer a specific future validation question.

They are not required by default.

⸻

Alternative D: Production-Ready System as the Initial Goal

The project could attempt to satisfy production requirements from the beginning.

Advantages

* stronger robustness
* stronger safety
* greater deployment realism
* clearer path toward industrial operation

Disadvantages

* formal requirements would need to be defined first
* much larger multidisciplinary effort
* significant testing and validation
* certification and security concerns
* higher cost
* much longer development time
* reduced ability to rapidly change the architecture

Evaluation

This does not match the current purpose or development environment.

It was not selected.

⸻

Selected Scope Strategy

The selected strategy is:

Define architectural question
        ↓
Implement minimum meaningful physical evidence
        ↓
Integrate end to end
        ↓
Validate responsibility boundaries
        ↓
Document limitations
        ↓
Defer features that do not improve the current proof
        ↓
Reconsider when requirements change

This provides the strongest balance between:

* implementation depth
* architectural breadth
* available development capacity
* practical completion
* system-level evidence

⸻

Consequences

Positive Consequences

The selected scope provides:

* completed end-to-end implementation
* multiple physical demonstrations
* meaningful architecture evidence
* manageable individual-development scope
* visible responsibility boundaries
* explicit extension paths
* clear distinction between PoC and production concerns

⸻

Negative Consequences

The selected scope also means:

* some capabilities remain deliberately simple
* manual setup remains necessary
* production robustness is not demonstrated
* some fault handling requires human investigation
* some reconfiguration requires recalibration
* production deployment would require additional architectural work

⸻

Context-Dependent Consequences

The scope should change when the project’s purpose changes.

New goal	Possible scope expansion
Production machinery	Safety architecture, guarding, certification
Long-distance installation	Industrial communication transport
High availability	Redundancy and failover
Multi-machine deployment	Centralized monitoring and external storage
Remote operation	Cybersecurity and access control
Factory-system integration	MES / ERP interfaces
Complex orchestration	Flow-editor or higher-level orchestration model
Faster physical setup	Standardized harnesses and module slots
Autonomous recovery	Fault classification and recovery logic

⸻

Validation Through Current Implementation

The current system demonstrates the selected scope through:

* two different physical process configurations
* real motors, servos, sensors, and mechanisms
* distributed Pico control nodes
* common register interface
* Raspberry Pi 5 orchestration
* live monitoring
* SQLite persistence
* Grafana visualization
* power monitoring
* custom PCBs
* published manufacturing information
* architecture and implementation documentation

These elements provide sufficient evidence for the current architectural purpose without requiring production-level completion of every subsystem.

⸻

Decision Boundary

This decision establishes:

* tabletop proof-of-concept scope
* end-to-end architectural validation as the priority
* explicit deferral of production-level capabilities
* future work as optional extensions rather than current requirements
* reconsideration of deferred features when requirements change
* clear separation between demonstrated capability and production readiness

This decision does not establish:

* a production product roadmap
* guaranteed implementation of every Future Work item
* industrial deployment readiness
* certified safety architecture
* production cybersecurity
* guaranteed scalability to factory-wide deployment
* formal production reliability targets

⸻

Related Documents

* Architecture Decision Log overview
* ADL-000: Requirements Definition and Success Conditions
* ADL-001: System Context and Goals
* ADL-002: Design and Development Constraints
* ADL-003: Host Platform Selection
* ADL-004: Distributed Node Architecture
* ADL-005: Communication Interface Selection
* ADL-006: Common Register Interface
* ADL-007: Separation of Responsibilities
* ADL-008: Reconfigurable Physical Mechanisms
* ADL-009: Monitoring and Control Separation
* ADL-010: Component Selection and Reproducibility
* Repository Overview
* Hardware
* Firmware
* Software
* Use Cases