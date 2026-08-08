# Architecture Decision Log

This directory documents the architectural reasoning behind the Modular Factory System.

The repository describes the implemented hardware, firmware, software, and physical demonstrations.

The Architecture Decision Log focuses on:

- the problems the project was intended to address
- the principles used to evaluate design choices
- the requirements and constraints that shaped the system
- the alternatives considered during development
- the trade-offs resulting from each decision
- the relationship between architectural reasoning and implementation

---

## Purpose

The purpose of this directory is to explain why the Modular Factory System has its current structure.

The hardware boards, firmware nodes, host-side software, monitoring tools, and physical mechanisms were not developed as isolated components.

They were derived from a common set of project goals and architectural decision principles.

The general reasoning flow is:

```text
Problem awareness
        ↓
Requirements and success conditions
        ↓
Architectural decision principles
        ↓
Individual technology and design decisions
        ↓
Hardware, firmware, software, and physical implementation
        ↓
Demonstrated use cases
```

This makes it possible to evaluate each implementation choice in relation to the conditions and objectives under which it was made.

---

## Relationship to the Project

The Modular Factory System is a reconfigurable tabletop proof-of-concept platform that connects:

- physical mechanisms
- embedded control
- host-side process orchestration
- system monitoring
- persistent logging
- historical visualization

The system was developed as an end-to-end platform rather than as a collection of unrelated prototypes.

Its physical processes can be replaced or rearranged while reusing common control, communication, monitoring, and data-management elements.

The current implementation includes:

- Raspberry Pi Pico-based distributed control nodes
- custom control, sensing, wiring, and power-monitoring boards
- a Raspberry Pi 5 host
- a common register-based I²C interface
- host-side orchestration scripts
- I²C monitoring and SQLite persistence
- Grafana visualization
- multiple process-specific tabletop use cases

The ADL connects these implementation choices to the higher-level reasoning from which they were derived.

--- 

## Problem Awareness

The project was created in response to three related problems.

### Establishing a Shared Observable Reference

Abstract descriptions of factory systems, DX initiatives, or system architectures can be interpreted differently depending on a person's technical background and responsibilities.

A system diagram may be understood differently by people working in:

- mechanical design
- electronics
- embedded software
- application software
- operations
- data analysis
- process design
- project or business planning

The project therefore brings system behavior down to observable physical motion.

This allows participants to look at the same process and discuss:

- what moves
- what is detected
- where decisions are made
- which component owns each responsibility
- what data is available
- what is not currently observable

### Supporting Cross-Domain Review

An isolated hardware prototype or software application provides only a limited basis for reviewing the complete system.

The project therefore retains an end-to-end implementation path from physical motion through data visualization.

This creates a basis for people with different viewpoints to examine the same system and identify:

- missing responsibilities
- unsuitable interfaces
- incorrect assumptions
- operational concerns
- data requirements
- opportunities for reconfiguration

### Validating Processes and Responsibility Boundaries Early

Process flow and responsibility boundaries become expensive to change after a large physical system has been built.

The project therefore uses tabletop-scale mechanisms to examine different process arrangements at an earlier stage.

The tabletop system can be used to investigate:

- process sequence
- device boundaries
- orchestration responsibilities
- monitoring responsibilities
- required interfaces
- missing modules
- alternative physical arrangements

The goal is not to reproduce the final production system at small scale.

The goal is to identify architectural questions before committing to a larger implementation.

---

## Architectural Decision Principles

The following principles guide the architectural and implementation decisions documented in this directory.

### 1. Scale Fit and Limited Change Propagation

A selected approach should fit the scale and purpose of the tabletop proof of concept.

The system should not introduce complexity that provides little value at the demonstrated scale.

At the same time, changes should remain localized to the layer or module responsible for the changed requirement whenever practical.

Examples include:

- changing a physical process without redesigning every control board
- adding a sensor without modifying unrelated motor nodes
- changing visualization without modifying device-control firmware
- changing orchestration without replacing the monitoring and persistence layers
- replacing one module without reconstructing the complete system

This principle leads to modular boundaries, common interfaces, and separation of responsibilities.

### 2. Supply-Chain Risk Reduction

Components should be obtainable through traceable and reasonably repeatable procurement channels.

Preferred procurement paths include:

- established local electronic-component suppliers such as [Akizuki Denshi](https://akizukidenshi.com/catalog/default.aspx)
- authorized or well-established distributors such as [DigiKey](https://www.digikey.jp/)
- manufacturers with identifiable product information and documentation
- commercially available mechanical components that can be replaced or substituted

When ordering electronic components through a distributor, the project generally avoids unnecessary dependence on obscure or poorly documented vendors when established alternatives are available.

Relevant considerations include:

- manufacturer identification
- product lifecycle visibility
- datasheet availability
- repeatability of procurement
- availability of equivalent parts
- replacement effort
- risk of unannounced specification changes

The objective is not to eliminate every supply-chain risk.

The objective is to avoid making the system unnecessarily dependent on parts that are difficult to identify, replace, or reproduce.

### 3. Low Learning and Startup Cost

The time and effort required to learn, integrate, debug, and begin using a technology are treated as part of its architectural cost.

A technically capable approach may still be unsuitable if it requires disproportionate preparation for the intended proof of concept.

Relevant considerations include:

- availability of documentation
- availability of development tools
- familiarity of the programming environment
- ease of creating a minimal test
- debugging support
- compatibility with existing tools
- integration effort
- time required before observable results can be produced

The preferred solution is not necessarily the most technically advanced one.

It is the solution that provides sufficient capability while allowing the project to progress toward its system-level goals.

### 4. Tabletop Physical and Spatial Fit

The complete demonstrated system should fit within the physical and spatial limits of a tabletop environment.

This applies to:

- computing hardware
- control boards
- wiring
- actuators
- sensors
- cameras
- structural parts
- workpiece paths
- moving mechanisms
- maintenance and adjustment space

Component selection and mechanism design therefore consider:

- dimensions
- mounting position
- required movement
- possible interference
- structural loading
- cable routing
- accessibility
- ease of disassembly
- portability

A component that is appropriate in a larger installation may be unsuitable if it introduces unnecessary size, weight, wiring, or mounting complexity at tabletop scale.

### 5. Reconfiguration Through Replacement and Configuration

The system should support different physical behaviors through replacement, rearrangement, orchestration changes, and parameter adjustment rather than complete redesign.

Reconfiguration may include:

- replacing a physical mechanism
- changing connected modules
- assigning different node addresses
- modifying process orchestration
- changing motor speed or direction
- changing servo angles
- changing sensor thresholds
- changing camera position or image-processing parameters
- changing timing values
- rearranging structural components

The Amazon-style Sorting Demo and Stamp Process Demo provide evidence that the same platform can support different physical processes.

This principle does not require every component to be universally reusable.

It requires the reusable boundaries to remain clear enough that process-specific elements can be changed without replacing the entire system.

---

## Documents

| ID      | Document                                       | Purpose                                                                                                                 |
| ------- | ---------------------------------------------- | ----------------------------------------------------------------------------------------------------------------------- |
| [ADL-000](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md) | Requirements Definition and Success Conditions | Defines the project motivation, intended value, high-level requirements, non-goals, and success conditions              |
| [ADL-001](./ADL-001%20System%20Context%20and%20Goals.md) | System Context and Goals                       | Defines the system boundary, major elements, external relationships, intended use, and architectural scope              |
| ADL-002 | Design and Development Constraints             | Records the physical, cost, schedule, implementation, maintenance, and prototype constraints                            |
| ADL-003 | Host Platform Selection                        | Explains the selection of the Raspberry Pi 5 and compares it with alternative host platforms                            |
| ADL-004 | Distributed Node Architecture                  | Explains the decision to use separate Raspberry Pi Pico-based control nodes                                             |
| ADL-005 | Communication Interface Selection              | Explains the selection of I²C for the demonstrated tabletop system and its limitations                                  |
| ADL-006 | Common Register Interface                      | Explains the shared register-based communication model used by the distributed nodes                                    |
| ADL-007 | Separation of Responsibilities                 | Defines the boundaries between physical mechanisms, firmware, orchestration, monitoring, persistence, and visualization |
| ADL-008 | Reconfigurable Physical Mechanisms             | Explains the separation of reusable control infrastructure from replaceable process-specific mechanisms                 |
| ADL-009 | Monitoring and Control Separation              | Explains why monitoring, persistence, and Grafana visualization remain outside the direct device-control path           |
| ADL-010 | Component Selection and Reproducibility        | Records the principles used for sourcing, replacement, manufacturing-data publication, and reproduction                 |
| ADL-011 | Prototype Scope and Deferred Features          | Records intentionally omitted or deferred production-level capabilities and the reasons for those boundaries            |

---

## Possible Document Contents

Depending on the subject, an ADL document may include:

- background and system context
- goals and requirements
- success conditions
- physical and development constraints
- decision criteria
- candidate approaches
- comparison of alternatives
- the selected approach
- rationale for the selection
- resulting design principles
- positive consequences
- trade-offs and limitations
- features intentionally excluded or deferred
- mapping from architectural reasoning to implementation
- links to related repository documents

For a technology or component selection, candidate approaches may be compared directly.

Example:

| Candidate   | Advantages | Limitations | Relevance to this project |
| ----------- | ---------- | ----------- | ------------------------- |
| Candidate A | ...        | ...         | ...                       |
| Candidate B | ...        | ...         | ...                       |
| Candidate C | ...        | ...         | ...                       |

For a broader architectural principle, the document may show how one requirement or constraint influenced multiple system layers.

Example:

| Requirement or principle                      | Resulting implementation                                                                 |
| --------------------------------------------- | ---------------------------------------------------------------------------------------- |
| Limit the propagation of changes              | Separate motor, servo, and sensor nodes                                                  |
| Reuse the platform across different processes | Process-specific orchestration and replaceable physical mechanisms                       |
| Reduce supply-chain uncertainty               | Traceable electronic components and published BOM information                            |
| Fit within tabletop constraints               | Compact host, local nodes, short-distance communication, and construction-kit mechanisms |
| Reduce startup effort                         | Established tools, languages, libraries, and development platforms                       |

---

## Relationship to Repository Areas

The ADL provides architectural context for the implementation documented elsewhere in the repository.

| Repository area               | Relationship to the ADL                                                                 |
| ----------------------------- | --------------------------------------------------------------------------------------- |
| [`Hardware/`](../Hardware/)   | Implements the physical boundaries for control, sensing, wiring, and power monitoring   |
| [`Firmware/`](../Firmware/)   | Implements device-local responsibilities derived from the distributed-node architecture |
| [`Software/`](../Software/)   | Implements host-side monitoring, persistence, visualization, and supporting tools       |
| [`Use_cases/`](../Use_cases/) | Demonstrates reuse of the common platform across different physical processes           |
| [`Docs/`](../Docs/)           | Provides protocol references and supplementary technical documentation                  |
| [`README.md`](../README.md)   | Provides the repository-level overview and entry point to the system                    |

The ADL does not replace component-level documentation.

It explains why the components, interfaces, responsibility boundaries, and implementation approaches documented in those areas exist.
