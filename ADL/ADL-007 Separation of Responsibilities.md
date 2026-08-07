# ADL-007: Separation of Responsibilities

## Purpose of This Document

This document explains how responsibilities are separated across the Modular Factory System and why those boundaries were selected.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)

The earlier decisions establish:

* the Raspberry Pi 5 host
* distributed Raspberry Pi Pico nodes
* I²C communication
* a common register interface

This document defines how control, observation, persistence, visualization, and physical behavior are divided across the resulting system.

---

## Decision Summary

The system separates responsibilities across the following major layers:

| Layer                  | Main responsibility                                                      |
| ---------------------- | ------------------------------------------------------------------------ |
| Physical mechanism     | Perform the actual tabletop process                                      |
| Hardware               | Provide electrical connection, power distribution, and device interfaces |
| Node-specific firmware | Execute local actuator or sensor behavior                                |
| Common firmware core   | Provide shared register, lifecycle, command, and state behavior          |
| Host orchestration     | Coordinate multiple nodes and process-level logic                        |
| Monitoring and logging | Observe exposed system information and persist selected records          |
| Storage                | Retain historical observations                                           |
| Visualization          | Present stored observations for later review                             |

Conceptually:

```text
Process-level decisions
        ↓
Host orchestration
        ↓
Common register interface
        ↓
Common firmware lifecycle
        ↓
Node-specific behavior
        ↓
Hardware interface
        ↓
Physical mechanism
```

Observation follows a separate path:

```text
Node state / feedback
        ↓
Monitoring software
        ↓
SQLite
        ↓
Grafana
```

The architecture intentionally avoids assigning every responsibility to one controller or one software process.

The goal is to reduce coupling, improve fault isolation, and make individual layers replaceable or modifiable without requiring unnecessary changes elsewhere.

---

## Decision Context

The project combines multiple domains:

* mechanical mechanisms
* custom electronics
* embedded firmware
* device communication
* process orchestration
* camera processing
* monitoring
* persistent storage
* visualization

A single implementation could technically combine many of these responsibilities.

For example, the Raspberry Pi 5 could:

* directly control actuators
* read sensors
* decide process sequence
* store records
* display dashboards
* interpret device state
* manage all error behavior

Similarly, individual Pico nodes could be given process-level knowledge rather than only local device responsibilities.

However, concentrating responsibilities would increase coupling.

A change in one area could then require modification in unrelated areas.

The architecture therefore treats responsibility placement as a deliberate design decision rather than an accidental result of implementation convenience.

---

## Separation Goals

The responsibility model was selected to support the following goals.

| ID   | Goal                                                                         |
| ---- | ---------------------------------------------------------------------------- |
| SR1  | Separate physical behavior from process-level orchestration                  |
| SR2  | Separate shared firmware behavior from device-specific behavior              |
| SR3  | Separate control from observation                                            |
| SR4  | Separate observation from historical visualization                           |
| SR5  | Keep persistent storage outside device-local control                         |
| SR6  | Reduce change propagation across layers                                      |
| SR7  | Improve fault localization                                                   |
| SR8  | Allow individual responsibilities to evolve independently                    |
| SR9  | Keep interfaces explicit                                                     |
| SR10 | Avoid unnecessary physical separation where logical separation is sufficient |

---

## SR1: Physical Behavior and Process Orchestration

Physical devices should not decide the complete process sequence.

A local motor node may know:

* how to start the motor
* how to apply a local mode
* how to stop safely
* when its local operation is complete

It should not need to know:

* which actuator operates next
* which color was detected by the camera
* which complete use case is currently running
* whether a separate process branch should be selected

These process-level decisions remain on the host.

Conceptually:

```text
Host:
"What should happen next?"

Node:
"How do I perform my local operation?"
```

This allows the same local device behavior to participate in different process configurations.

---

## SR2: Common Firmware and Node-Specific Firmware

The firmware separates behavior that should remain consistent across node types from behavior that depends on the connected physical device.

### Common Firmware Core

The common core owns:

* register behavior
* parameter staging
* lifecycle state
* command validation
* RUN and STOP handling
* E-STOP lifecycle handling
* completion-state handling
* update signaling
* invocation of node-specific callbacks

### Node-Specific Firmware

Node-specific firmware owns:

* GPIO and peripheral initialization
* actuator or sensor behavior
* device-specific parameter interpretation
* device-specific validation
* local feedback values
* local completion criteria
* safe local output shutdown

Conceptually:

```text
Common firmware
      ↓ defines when an operation may occur
Node-specific firmware
      ↓ defines how that operation is physically performed
```

Node-specific code should not directly take ownership of common lifecycle state.

The common core should not contain actuator-specific behavior unless that behavior genuinely becomes a shared architectural requirement.

---

## SR3: Control and Observation

Monitoring should not become a hidden part of process control.

The control path is:

```text
Host orchestration
        ↓
Node command
        ↓
Node firmware
        ↓
Physical device
```

The observation path is:

```text
Node state / feedback
        ↓
Monitoring software
        ↓
Recorded observation
```

The monitoring system may observe:

* lifecycle state
* node identification
* feedback
* command results
* events
* snapshots
* power-related measurements

Observation does not itself establish the physical result of an operation.

For example, a node reporting completion does not independently prove:

* the workpiece reached the intended destination
* the required force was applied
* the mechanism moved through the expected physical trajectory

Those conclusions require the appropriate physical evidence.

---

## SR4: Observation and Visualization

Historical visualization is separated from data acquisition.

The current monitoring flow is:

```text
Observed node state / measurement
        ↓
I2C Debugger
        ↓
SQLite
        ↓
Grafana
```

The I2C Debugger acquires and stores selected observations.

Grafana reads persisted records and presents them visually.

Grafana does not issue physical control commands.

A dashboard therefore remains outside the direct control path.

This boundary allows:

* visualization to change without modifying device control
* dashboards to fail without becoming a required actuator dependency
* stored records to be inspected through tools other than Grafana
* acquisition and presentation logic to evolve independently

---

## SR5: Device Control and Persistent Storage

Device nodes do not own historical storage.

They expose current state and feedback through the common interface.

The host-side software determines which information is persisted.

SQLite currently stores selected:

* node states
* events
* snapshots
* monitoring sessions
* power-related measurements

This avoids requiring every embedded node to manage:

* database files
* long-term history
* storage rotation
* visualization-oriented schemas

The embedded nodes remain focused on local device behavior.

---

## SR6: Change Localization

Responsibility boundaries should reduce the amount of the system affected by a change.

Examples include:

| Change                        | Intended main impact                                    |
| ----------------------------- | ------------------------------------------------------- |
| Change conveyor mechanism     | Physical mechanism and possibly local hardware/firmware |
| Change motor-control behavior | Motor-node implementation                               |
| Change lifecycle rule         | Common firmware core                                    |
| Change process sequence       | Host orchestration                                      |
| Change monitoring UI          | Monitoring software                                     |
| Change database schema        | Logging/storage layer                                   |
| Change Grafana dashboard      | Visualization layer                                     |
| Change I²C transport          | Communication boundary and affected adapters            |

The architecture does not guarantee that every change remains inside one layer.

Interfaces may need coordinated updates.

The objective is to avoid unrelated propagation where a stable boundary can contain the change.

---

## SR7: Fault Localization

The responsibility model provides several investigation boundaries.

If a process fails, questions can be separated by layer.

### Physical Layer

* Did the mechanism physically move?
* Is something mechanically blocked?
* Did the workpiece follow the intended path?

### Hardware Layer

* Is power present?
* Is the actuator electrically connected?
* Is the expected signal available?

### Node Firmware Layer

* Did the node receive the command?
* Did it enter BUSY?
* Did local validation accept the request?
* Did it publish feedback or completion?

### Communication Layer

* Is the node reachable?
* Is the expected address present?
* Can registers be read and written?

### Host Layer

* Was the correct node selected?
* Was the correct sequence executed?
* Was process-specific logic correct?

### Monitoring Layer

* Was the expected state observed?
* Was the record stored?
* Is the historical view consistent with the stored data?

This structure does not automatically identify the fault.

It provides a way to narrow the investigation.

---

## SR8: Independent Evolution

Responsibility separation allows individual areas to evolve at different rates.

For example:

* a new mechanism can reuse existing host orchestration concepts
* a new node profile can reuse the common firmware core
* a monitoring interface can change without changing motor firmware
* a dashboard can change without changing SQLite acquisition logic
* process-specific scripts can change without redesigning the register interface

This is particularly useful for a proof of concept where architecture and implementation are refined incrementally.

---

## SR9: Explicit Interfaces

Separation is useful only when communication between responsibilities remains explicit.

Current important interfaces include:

| Boundary                                 | Interface                             |
| ---------------------------------------- | ------------------------------------- |
| Host ↔ node                              | I²C communication                     |
| Host logic ↔ device abstraction          | Common register semantics             |
| Common firmware ↔ node-specific firmware | Callback and public core API          |
| Firmware ↔ hardware                      | GPIO, PWM, ADC, and local peripherals |
| Monitoring ↔ persistence                 | SQLite write operations               |
| Visualization ↔ persistence              | SQLite data read by Grafana           |

The architecture should avoid hidden dependencies that bypass these boundaries without a deliberate reason.

---

## SR10: Logical Separation Without Unnecessary Hardware

Responsibility separation does not require every function to run on a different physical computer.

The Raspberry Pi 5 currently hosts multiple logically distinct responsibilities, including:

* process orchestration
* camera-processing logic
* monitoring software
* SQLite persistence
* Grafana services

These responsibilities share one physical platform because:

* the current workload is modest
* the proof-of-concept scale does not require multiple computers
* additional platforms would increase cost and setup effort
* Linux provides sufficient isolation through separate software components

The architecture therefore distinguishes:

```text
Logical responsibility separation
```

from:

```text
Physical deployment separation
```

The current proof of concept emphasizes the former.

---

## Current Responsibility Model

The current system can be summarized as follows.

| Responsibility                              | Current owner                                 |
| ------------------------------------------- | --------------------------------------------- |
| Physical motion and manipulation            | Mechanical mechanism                          |
| Electrical actuator/sensor connection       | Hardware boards                               |
| Power distribution and branch monitoring    | Power Monitor Board and host-side observation |
| Device-local behavior                       | Node-specific Pico firmware                   |
| Shared node lifecycle and register behavior | Common Pico firmware core                     |
| Host-to-node transport                      | I²C                                           |
| Multi-node process sequence                 | Raspberry Pi 5 orchestration                  |
| Camera-based classification                 | Raspberry Pi 5 process-specific software      |
| Node inspection                             | I2C Debugger                                  |
| Event and snapshot logging                  | I2C Debugger                                  |
| Persistent records                          | SQLite                                        |
| Historical visualization                    | Grafana                                       |

---

## Control Path

The current control path is:

```text
Process-specific host logic
        ↓
Host orchestration
        ↓
Common register command
        ↓ I²C
Common firmware core
        ↓
Node-specific callback
        ↓
Hardware output / sensor acquisition
        ↓
Physical mechanism
```

Feedback returns through:

```text
Physical / device-local result
        ↓
Node-specific firmware
        ↓
Common feedback and lifecycle state
        ↓ I²C
Host software
```

The host may use this exposed state as input to later process decisions.

---

## Monitoring Path

Monitoring is structurally different from control.

```text
Firmware state / feedback
        ↓
I²C polling
        ↓
I2C Debugger
        ↓
SQLite
        ↓
Grafana
```

Power-related observations are also collected on the host side and can be persisted alongside node observations.

The monitoring path exists to make internal system information observable.

It is not the primary source of actuator commands.

---

## Physical Outcome Boundary

The architecture distinguishes between:

```text
Software-visible state
```

and:

```text
Physical outcome
```

Examples of software-visible information include:

* BUSY
* READY
* completion flags
* feedback values
* command results
* power measurements

These values describe what the system exposes through its interfaces.

They do not automatically prove every physical consequence.

A mechanically jammed actuator, detached mechanism, or unexpected workpiece behavior may require direct physical observation or additional sensing.

This distinction prevents the monitoring architecture from claiming more evidence than it actually provides.

---

## Risks and Mitigations

| Risk                                                       | Current mitigation                                                  |
| ---------------------------------------------------------- | ------------------------------------------------------------------- |
| Responsibility boundaries become unclear                   | Layer responsibilities and public interfaces are documented         |
| Common firmware accumulates device-specific logic          | Device-specific callbacks and profile boundary                      |
| Host becomes dependent on GPIO details                     | Host uses common register interface                                 |
| Node firmware accumulates process logic                    | Process orchestration remains on host                               |
| Monitoring becomes part of control path                    | Monitoring and Grafana remain logically separate from orchestration |
| Visualization failure affects control                      | Grafana reads persisted data only                                   |
| Too many separate physical systems increase complexity     | Multiple logical responsibilities share the Raspberry Pi 5          |
| Layering creates excessive abstraction                     | Boundaries are limited to responsibilities that materially differ   |
| Physical failure is mistaken for successful software state | Physical outcome is treated separately from firmware-visible state  |
| Changes cross multiple boundaries                          | Interfaces identify where coordinated changes are required          |

---

## Comparison Summary

| Alternative                              | Main strengths                                         | Main limitations                               | Fit for current PoC |
| ---------------------------------------- | ------------------------------------------------------ | ---------------------------------------------- | :-----------------: |
| Centralized Raspberry Pi 5               | Simple physical system and one development environment | Strong coupling across responsibilities        |         Weak        |
| Process logic inside nodes               | Greater local autonomy                                 | Process knowledge distributed across firmware  |         Weak        |
| Independent firmware per node            | Maximum node-local freedom                             | Duplicated shared behavior and interface drift |         Weak        |
| Monitoring integrated with orchestration | Fewer software components                              | Control and observation become tightly coupled |       Moderate      |
| Logical separation across current layers | Clear boundaries with limited infrastructure           | Requires explicit interfaces and documentation |        Strong       |
| Fully separated physical services        | Strong isolation and scalability                       | Excessive infrastructure for tabletop PoC      |         Weak        |

The ratings describe suitability for the current proof of concept.

They are not general rankings of software or control architectures.

---

## Considered Alternatives

### Alternative A: Centralized Responsibility on Raspberry Pi 5

The Raspberry Pi 5 could own nearly all responsibilities.

This could include:

* GPIO control
* sensor acquisition
* process sequence
* logging
* visualization
* actuator-specific behavior

#### Advantages

* fewer controllers
* fewer explicit software interfaces
* fast implementation for a small fixed prototype
* all logic is available in one environment

#### Disadvantages

* strong coupling between device internals and process logic
* hardware changes propagate into host software
* fault isolation becomes harder
* host becomes responsible for low-level timing and device behavior
* reuse of local device behavior becomes weaker
* one implementation accumulates unrelated responsibilities

#### Evaluation

This approach would be simpler for a very small fixed demonstration.

It was not selected because the project requires modularity, reconfiguration, and observable responsibility boundaries.

---

### Alternative B: Process Logic Distributed Into Device Nodes

Each node could contain knowledge of neighboring devices or complete process sequences.

#### Advantages

* reduced host orchestration
* nodes could perform autonomous sequences
* selected operations could continue without frequent host intervention

#### Disadvantages

* process behavior becomes distributed across firmware
* sequence changes require reflashing multiple nodes
* reuse across different use cases becomes harder
* dependencies between nodes become less visible
* debugging complete process behavior becomes more difficult

#### Evaluation

This model could become useful for autonomous or deterministic distributed control.

It was not selected for the current proof of concept because process sequencing changes more frequently than device-local behavior.

---

### Alternative C: Unified Firmware Without Common / Device-Specific Separation

Each node could contain one independent firmware implementation.

#### Advantages

* straightforward implementation per device
* no common callback architecture required
* complete freedom for each node

#### Disadvantages

* lifecycle logic is duplicated
* register behavior can diverge
* E-STOP behavior can diverge
* bugs must be corrected in multiple implementations
* host compatibility becomes harder to maintain

#### Evaluation

This approach was rejected in favor of a shared common core with device-specific extensions.

---

### Alternative D: Monitoring Integrated Into Orchestration

Process orchestration could also own all monitoring and historical presentation responsibilities.

#### Advantages

* fewer software components
* orchestration has direct access to all observed values
* simple initial development

#### Disadvantages

* control code becomes coupled to logging and presentation
* monitoring changes can affect process execution
* visualization failures can interfere with operation
* testing individual responsibilities becomes harder
* historical analysis concerns enter the control path

#### Evaluation

The current system instead keeps monitoring and visualization logically separate from control.

---

### Alternative E: Fully Physically Separated Services

Each responsibility could run on independent hardware.

For example:

```text
Control computer
Monitoring computer
Database server
Visualization server
```

#### Advantages

* strong failure isolation
* independent resource allocation
* deployment can scale per responsibility
* production infrastructure can be separated cleanly

#### Disadvantages

* significantly more hardware
* additional networking
* configuration and deployment complexity
* higher cost
* unnecessary infrastructure for a tabletop proof of concept

#### Evaluation

Physical separation may become appropriate at production scale.

For the current project, logical separation on the Raspberry Pi 5 provides sufficient architectural clarity at much lower cost.

---

## Selected Responsibility Model

The selected model separates responsibilities according to their reason for change.

The primary allocation is:

```text
Physical process
→ mechanism

Electrical interface
→ hardware

Local device behavior
→ node-specific firmware

Shared node protocol and lifecycle
→ common firmware

Process coordination
→ host orchestration

Observation and logging
→ monitoring software

Persistence
→ SQLite

Historical presentation
→ Grafana
```

This allocation provides the strongest fit for:

* modularity
* change localization
* system observability
* debugging
* reuse
* incremental development
* tabletop implementation

---

## Consequences

### Positive Consequences

The selected responsibility model provides:

* clearer system boundaries
* localized device-specific changes
* reusable firmware infrastructure
* reusable host interfaces
* easier fault investigation
* independent monitoring development
* independent visualization changes
* reduced coupling between physical control and data presentation
* clearer architectural explanation

---

### Negative Consequences

The separation also introduces:

* more interfaces
* more documentation
* additional integration points
* potential mismatch between layers
* need to maintain responsibility discipline
* possibility of duplicated information across boundaries
* more components to inspect during debugging

---

### Context-Dependent Consequences

The selected boundaries reflect the current proof-of-concept scale.

Future requirements may justify moving responsibilities.

Examples include:

| Future requirement                | Possible change                                              |
| --------------------------------- | ------------------------------------------------------------ |
| Hard real-time multi-axis control | Move selected coordination closer to embedded control        |
| Large-scale production monitoring | Separate monitoring and storage onto external infrastructure |
| Higher system autonomy            | Move selected process decisions into local controllers       |
| Large historical datasets         | Replace or supplement local SQLite storage                   |
| Multiple production stations      | Distribute host responsibilities across networked systems    |
| Additional physical validation    | Add independent sensors or measurement systems               |

Responsibility allocation should therefore be reconsidered when the reason for change shifts.

---

## Validation Through Current Implementation

The current implementation demonstrates the selected separation through:

* dedicated physical mechanisms
* custom hardware interface boards
* distributed Pico nodes
* shared common firmware
* node-specific firmware callbacks
* Raspberry Pi 5 orchestration
* separate I2C Debugger monitoring
* SQLite persistence
* Grafana historical visualization
* separate camera-processing logic for the sorting use case

The architecture therefore demonstrates multiple independently identifiable responsibilities within one end-to-end tabletop system.

---

## Decision Boundary

This decision establishes:

* separation of process-level and device-local behavior
* separation of common and node-specific firmware
* separation of control and monitoring
* separation of monitoring and visualization
* separation of current state from historical persistence
* logical responsibility separation even when software shares one physical host

This decision does not establish:

* the final physical mechanism design
* the detailed reconfiguration method
* production deployment topology
* high-availability architecture
* safety-certified responsibility allocation
* enterprise-system integration

The physical reconfiguration strategy is addressed in the next architectural decision.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)
* [System Architecture](../Docs/System_Architecture/)
* [Firmware Overview](../Firmware/)
* [Software Overview](../Software/)
* [Register Map](../Docs/Register_Map/)
* [Use Cases](../Use_cases/)
