# ADL-009: Monitoring and Control Separation

## Purpose of This Document

This document explains the decision to separate monitoring, persistence, and historical visualization from process control in the Modular Factory System.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [ADL-005: Communication Interface Selection](./ADL-005%20Communication%20Interface%20Selection.md)
* [ADL-006: Common Register Interface](./ADL-006%20Common%20Register%20Interface.md)
* [ADL-007: Separation of Responsibilities](./ADL-007%20Separation%20of%20Responsibilities.md)
* [ADL-008: Reconfigurable Physical Mechanisms](./ADL-008%20Reconfigurable%20Physical%20Mechanisms.md)

ADL-007 establishes the broader separation of responsibilities across the system.

This document focuses specifically on the boundary between:

* process and device control
* live observation
* persistent logging
* historical visualization

---

## Decision Summary

The Modular Factory System keeps control and monitoring as logically separate responsibilities.

The control path is:

```text
Host orchestration
        ↓
I²C command
        ↓
Common firmware
        ↓
Node-specific firmware
        ↓
Actuator or sensor behavior
```

The monitoring path is:

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

Power-related monitoring follows a related observation path:

```text
Power branch
        ↓
INA219
        ↓
TCA9548A
        ↓
I2C Debugger
        ↓
SQLite
        ↓
Grafana
```

The architecture assigns:

* process execution to orchestration and node firmware
* live observation and event detection to the I2C Debugger
* persistence to SQLite
* historical analysis and visualization to Grafana

Monitoring information may support human investigation and future decisions.

It does not become the authoritative actuator-control or safety path.

---

## Decision Context

The project needed both:

* control of the physical process
* visibility into what the system was doing

These needs are related but have different purposes.

Control answers questions such as:

* What should happen next?
* Which node should receive a command?
* Should an actuator start or stop?
* Is the requested device-local operation valid?

Monitoring answers questions such as:

* Which node is currently observed as READY or BUSY?
* Did a node disappear?
* When did a state change?
* What feedback value was exposed?
* What voltage, current, or power value was recorded?
* What happened during an earlier test run?

Historical analysis answers another class of questions:

* How long did a state continue?
* Did the same condition occur repeatedly?
* Did electrical load change between runs?
* What happened before an ERROR or E-STOP observation?

Combining these responsibilities into one control path would make observation, storage, and visualization part of the process-execution dependency chain.

The system therefore separates them.

---

## Separation Goals

The monitoring and control boundary was selected to support the following goals.

| ID   | Goal                                                                     |
| ---- | ------------------------------------------------------------------------ |
| MC1  | Keep process execution independent from historical visualization         |
| MC2  | Keep device-local control independent from monitoring UI behavior        |
| MC3  | Separate live observation from authoritative device state                |
| MC4  | Separate observation from persistence                                    |
| MC5  | Separate persistence from historical presentation                        |
| MC6  | Allow monitoring and visualization to evolve independently               |
| MC7  | Preserve data for later investigation                                    |
| MC8  | Support debugging without placing dashboards in the control path         |
| MC9  | Keep safety responsibility outside analytical visualization              |
| MC10 | Make monitoring limitations explicit                                     |
| MC11 | Support real and simulated monitoring through the same observation model |
| MC12 | Avoid unnecessary infrastructure for the tabletop PoC                    |

---

## MC1: Process Execution and Historical Visualization

Grafana is not part of the process-control path.

The process does not depend on a Grafana panel to:

* start an actuator
* stop an actuator
* select a node
* execute a process sequence
* reset E-STOP
* validate a device-local command

Grafana reads observations that have already been persisted.

Conceptually:

```text
Control:
Host → Node → Physical process

Historical analysis:
Recorded observation → Grafana
```

This allows dashboards and SQL queries to change without redefining node firmware or process orchestration.

---

## MC2: Device Control and Monitoring UI

The I2C Debugger observes the exposed node interface.

It does not own device-local actuator or sensor behavior.

The device firmware remains responsible for:

* lifecycle state
* command validation
* local output behavior
* sensor acquisition
* E-STOP firmware behavior
* completion reporting

The I2C Debugger is responsible for:

* polling
* interpreting exposed values
* tracking latest observed state
* detecting transitions
* displaying observations
* recording observations

The UI therefore reflects observed system information rather than replacing the firmware state machine.

---

## MC3: Observed State and Authoritative State

The architecture distinguishes:

```text
Firmware state
```

from:

```text
Monitoring application's latest observation of firmware state
```

These values may normally agree.

However, polling introduces observation delay and uncertainty.

For example:

* a node may change state between polling cycles
* a communication failure may prevent the current state from being read
* the latest displayed value may describe the most recent successful observation rather than the physical condition at the exact current instant

The monitoring application therefore uses states such as:

* UNKNOWN
* No Device
* Signal Lost

to represent monitoring-level interpretation of communication conditions.

These are not all firmware lifecycle states.

---

## MC4: Observation and Persistence

Live observation and persistent logging are separate concerns inside the monitoring application.

The I2C Debugger maintains the latest observed system condition while also writing selected information to SQLite.

The current persisted information includes:

* state-transition events
* periodic node snapshots
* state-duration segments
* monitoring sessions
* run-related records
* power-monitor snapshots

The live UI answers:

```text
What is currently being observed?
```

Persistence supports:

```text
What was observed previously?
```

The two use the same observation source but answer different questions.

---

## MC5: Persistence and Historical Presentation

SQLite acts as the current boundary between monitoring and historical visualization.

Conceptually:

```text
I2C Debugger
      ↓ writes
SQLite
      ↓ reads
Grafana
```

Grafana does not:

* poll the I²C nodes
* classify communication failures
* decode raw node protocol state independently
* write monitoring events
* determine the authoritative current node condition

Instead, it presents data already collected and persisted by the monitoring layer.

This prevents the dashboard from becoming another independent interpretation path to the hardware.

---

## MC6: Independent Evolution

The separation allows monitoring components to change independently.

Examples include:

* changing the PyQt monitoring UI
* changing event-display logic
* changing SQLite queries
* adding or changing Grafana panels
* changing historical statistics
* changing session filters

These changes should not require modification of:

* motor-control behavior
* servo-control behavior
* sensor acquisition
* common node lifecycle rules
* process-specific actuator sequencing

Some changes, such as database-schema changes, may require coordinated updates between persistence and visualization.

The objective is limited change propagation rather than complete isolation.

---

## MC7: Historical Investigation

The system stores observations because the latest state alone is insufficient for debugging and review.

For example, seeing:

```text
READY
```

now does not reveal whether the node previously experienced:

```text
READY
→ BUSY
→ ERROR
→ READY
```

Event logs and state-duration records preserve temporal context.

Monitoring sessions also separate observations from different executions.

This allows the system to compare:

* different demonstrations
* different parameter sets
* real and mock operation
* before and after a configuration change
* individual failure scenarios

---

## MC8: Debugging Without Dashboard Control

Monitoring data is intended to support debugging and explanation.

Examples include:

* checking whether a node was observed as BUSY
* identifying when communication was lost
* reviewing state transitions
* comparing electrical load with node activity
* inspecting repeated test sessions

This information can guide a developer or operator.

However, Grafana does not automatically convert an analytical observation into a physical control action.

For example:

```text
high recorded power
```

does not automatically imply:

```text
Grafana commands the actuator to stop
```

Any future automatic response would require an explicitly designed control or protection responsibility.

---

## MC9: Safety Boundary

Monitoring thresholds and visual warnings do not replace hardware or firmware safety mechanisms.

For example, configured electrical-load thresholds can support observation.

They do not replace:

* fuses
* current limiting
* power-supply protection
* firmware shutdown behavior
* emergency-stop handling
* safety-rated hardware

Grafana is also not responsible for real-time emergency decisions.

This boundary is particularly important because the monitoring system is intended for tabletop proof-of-concept observation rather than safety-certified control.

---

## MC10: Monitoring Limitations

The monitoring architecture does not claim to prove every physical outcome.

It can observe information such as:

* node state
* feedback registers
* communication condition
* events
* snapshots
* voltage
* current
* calculated power

It does not inherently prove:

* that an actuator physically moved
* that a workpiece reached its intended location
* that a gate reached the commanded angle
* that a stamp applied the intended force
* that a mark was produced correctly
* that the mechanism was not mechanically jammed

Additional sensing or direct physical observation is required when those outcomes must be verified.

---

## MC11: Real and Mock Monitoring

The I2C Debugger supports:

* real mode
* mock mode

Real mode reads actual hardware.

Mock mode simulates:

* node states
* power values
* missing devices
* transient communication failures
* signal loss
* warning and error conditions

Both modes feed the same monitoring architecture.

This allows UI, transition, logging, and failure-handling logic to be tested without the physical system.

Mock mode does not validate:

* the electrical I²C bus
* actual actuator behavior
* real sensor behavior
* physical power-monitor accuracy
* mechanical outcomes

The simulated observation path therefore supports software validation without being confused with physical-system validation.

---

## MC12: Tabletop-Scale Deployment

The current monitoring components run primarily on the same Raspberry Pi 5 that also hosts process-related software.

This avoids requiring:

* a separate monitoring computer
* a dedicated database server
* a separate visualization server
* production network infrastructure

The responsibilities are logically separated even though they are not all physically isolated.

This is appropriate for the current proof-of-concept scale.

---

## Current Monitoring Architecture

The current implementation follows:

```text
                     Raspberry Pi 5
                           │
              ┌────────────┴────────────┐
              │                         │
       Process orchestration       I2C Debugger
              │                         │
        command traffic            observation
              │                         │
              └────── I²C system ──────┘
                           │
                    Pico device nodes
```

The monitoring side additionally observes the power-monitoring subsystem.

```text
Power Monitor Board
        ↓
INA219 / TCA9548A measurements
        ↓
I2C Debugger
```

Monitoring records then flow through:

```text
I2C Debugger
        ↓
SQLite
        ↓
Grafana
```

This shows logical separation of responsibilities.

It does not imply complete physical or resource isolation.

---

## Shared Resource Boundary

The current implementation has an important limitation.

Process orchestration and monitoring may share:

* the Raspberry Pi 5
* operating-system resources
* CPU time
* memory
* local storage
* parts of the I²C communication environment

Therefore:

```text
logical separation
≠
complete failure isolation
```

Heavy polling, excessive logging, software faults, or resource contention could affect the host environment even though monitoring does not own process-control logic.

The current system accepts this limitation because:

* workload is modest
* operation is supervised
* the platform is a tabletop PoC
* separate monitoring infrastructure would add disproportionate complexity

A production architecture could require stronger physical or process-level isolation.

---

## Polling Model

The monitoring application uses periodic polling.

A polling cycle may:

1. read configured node addresses
2. update observed node models
3. evaluate communication-failure counters
4. decode protocol values
5. determine operator-facing states
6. detect transitions
7. read power-monitor values
8. update the UI
9. persist selected observations

Polling frequency therefore creates a trade-off.

Shorter intervals provide:

* lower observation delay
* more frequent snapshots

but also increase:

* I²C traffic
* database activity
* UI updates
* host workload

Longer intervals reduce resource usage but may delay or miss short-lived observations.

The monitoring interval is therefore an observation parameter, not a hard real-time control period.

---

## Communication-Failure Interpretation

The monitoring layer distinguishes between different communication conditions.

### UNKNOWN

Used when the current condition cannot yet be classified reliably.

### No Device

Used when an address has not been successfully observed and reaches the configured failure threshold.

### Signal Lost

Used when a previously observed node later exceeds the communication-failure threshold.

This distinction preserves monitoring context.

For example:

```text
Never observed
```

and:

```text
Previously operating but later unreachable
```

have different diagnostic meaning.

The monitoring classification does not itself determine the root cause.

Possible causes may remain in:

* power
* wiring
* firmware
* bus communication
* configuration
* hardware failure

---

## Event, Snapshot, and Segment Model

Different record types answer different questions.

| Record             | Main question                                       |
| ------------------ | --------------------------------------------------- |
| Transition event   | What changed?                                       |
| Snapshot           | What was observed at this time?                     |
| State segment      | How long did this observed state continue?          |
| Monitoring session | During which test or monitoring run did this occur? |
| Power snapshot     | What electrical values were recorded at this time?  |

This avoids forcing one record model to serve every analysis purpose.

---

## Power Monitoring Boundary

Power monitoring is part of the observation architecture.

The current system records:

* voltage
* current
* calculated power
* branch information
* main-line information

These values can be compared with observed node activity.

For example, they may help investigate whether electrical load changed while a motor was active.

However, the current monitoring implementation does not automatically map every power condition into:

* node WARN
* node ERROR
* actuator shutdown
* branch shutdown

Power-monitoring thresholds are therefore observational unless another explicitly designed control or protection layer acts on them.

---

## Risks and Mitigations

| Risk                                                         | Current mitigation                                                             |
| ------------------------------------------------------------ | ------------------------------------------------------------------------------ |
| Monitoring logic becomes part of actuator control            | Process execution remains in orchestration and firmware                        |
| Grafana becomes a hidden control dependency                  | Grafana reads persisted data only                                              |
| Displayed state is mistaken for exact physical truth         | Monitoring is described as observation of exposed state                        |
| Communication loss is mistaken for device state              | UNKNOWN, No Device, and Signal Lost are monitoring-level classifications       |
| Dashboard failure blocks process logic                       | Visualization remains outside the direct control path                          |
| Historical analysis increases bus traffic                    | Grafana reads SQLite rather than polling hardware                              |
| Monitoring polling creates excessive bus load                | Polling interval is configurable                                               |
| Logging creates excessive host load                          | Current workload is limited to tabletop PoC scale                              |
| Monitoring thresholds are mistaken for protection            | Hardware and firmware protection remain separate responsibilities              |
| Mock data is mistaken for physical validation                | Mock mode limitations are explicitly documented                                |
| Monitoring and control are assumed to be physically isolated | Shared Raspberry Pi and resource dependencies are documented                   |
| Database-schema changes break dashboards                     | Persistence and visualization interface must be updated together when required |

---

## Comparison Summary

| Alternative                                   | Main strengths                                       | Main limitations                                    | Fit for current PoC |
| --------------------------------------------- | ---------------------------------------------------- | --------------------------------------------------- | :-----------------: |
| Control only, no structured monitoring        | Lowest software complexity                           | Weak observability and no historical investigation  |         Weak        |
| Monitoring embedded directly in orchestration | Simple initial integration                           | Strong coupling between control and observation     |       Moderate      |
| Separate live monitoring with no persistence  | Good current-state visibility                        | Limited retrospective analysis                      |       Moderate      |
| Monitoring + SQLite + separate Grafana        | Clear current/history boundary and reusable analysis | More software components and interfaces             |        Strong       |
| Fully separate monitoring infrastructure      | Stronger resource and failure isolation              | Excessive infrastructure for current tabletop scale |         Weak        |

The ratings describe suitability for the current proof of concept.

They are not general rankings of industrial monitoring architectures.

---

## Considered Alternatives

### Alternative A: Control Without Structured Monitoring

The system could execute the process using only orchestration and firmware.

Debugging could rely on:

* terminal output
* ad hoc register reads
* direct visual observation

#### Advantages

* lower software complexity
* no database
* no dashboard
* minimal host workload
* faster initial implementation

#### Disadvantages

* weak system-wide visibility
* difficult reconstruction of earlier events
* no consistent monitoring sessions
* limited comparison between runs
* communication failures are harder to contextualize
* power trends are harder to compare with node activity

#### Evaluation

This approach could operate a small fixed demonstration.

It was not selected because observability is a central part of the project.

---

### Alternative B: Monitoring Integrated Into Process Orchestration

The orchestration process could directly own:

* device control
* live status display
* event detection
* logging
* historical data preparation

#### Advantages

* fewer software processes
* direct access to control context
* simple sharing of in-memory data
* less initial interface design

#### Disadvantages

* monitoring changes can affect orchestration
* logging failures enter the control process
* control code accumulates unrelated responsibilities
* testing becomes harder
* historical concerns become mixed with process sequencing
* reuse of monitoring across different use cases decreases

#### Evaluation

This approach would be practical for a very small script-based prototype.

It was not selected because the project uses multiple use cases and requires reusable observation behavior.

---

### Alternative C: Separate Live Monitoring Without Persistence

A standalone monitor could show current node state but not retain historical data.

#### Advantages

* live observability
* less storage complexity
* no historical database schema
* fewer components than the selected architecture

#### Disadvantages

* previous conditions disappear
* failure sequences are difficult to reconstruct
* sessions cannot be compared easily
* long-duration trends are unavailable
* electrical observations cannot be reviewed later

#### Evaluation

This would satisfy immediate debugging needs but not retrospective analysis.

It was therefore not selected as the complete monitoring model.

---

### Alternative D: Separate Monitoring, Persistence, and Historical Visualization

The selected model uses:

```text
I2C Debugger
→ SQLite
→ Grafana
```

#### Advantages

* current and historical views are separated
* control does not depend on dashboards
* persisted data remains available after the live view ends
* Grafana does not add direct hardware traffic
* sessions can be compared
* monitoring behavior can be reused across use cases
* visualization can change independently
* mock and real observation can share the same higher-level model

#### Disadvantages

* more software components
* database schema must be maintained
* visualization depends on persisted-data structure
* monitoring introduces host and I²C resource usage
* synchronization between latest physical state and latest observation is not instantaneous

#### Evaluation

This provides the strongest fit for the current project.

It was selected.

---

### Alternative E: Fully Separate Monitoring Infrastructure

Monitoring could be moved to independent hardware and services.

For example:

```text
Control Raspberry Pi
        │
Physical system
        │
Independent monitor
        ↓
External database
        ↓
Separate dashboard server
```

#### Advantages

* stronger resource isolation
* monitoring failure is less likely to affect control-host resources
* larger storage capacity
* easier independent scaling
* closer to some production monitoring architectures

#### Disadvantages

* more hardware
* more network infrastructure
* synchronization and deployment complexity
* additional cost
* greater configuration burden
* excessive infrastructure for the current proof of concept

#### Evaluation

This may become appropriate at a larger scale or when stronger failure isolation is required.

It was not selected for the current tabletop implementation.

---

## Selected Monitoring Architecture

The selected approach uses:

* process orchestration for process-level control
* common and node-specific firmware for local control
* I2C Debugger for live observation and persistence
* SQLite as the persistence boundary
* Grafana for historical visualization

This provides the strongest current fit for:

* observability
* debugging
* historical investigation
* multiple use cases
* change localization
* simple deployment
* tabletop operation
* individual development

---

## Consequences

### Positive Consequences

The selected architecture provides:

* reusable live monitoring
* reusable event detection
* historical records
* session comparison
* separation between current and historical views
* independent Grafana development
* limited dashboard impact on hardware access
* mock-mode testing
* clearer responsibility boundaries
* stronger debugging context

---

### Negative Consequences

The architecture also introduces:

* additional host processes
* additional I²C polling
* database writes
* database-schema maintenance
* dashboard maintenance
* monitoring latency
* possible resource contention on the shared Raspberry Pi 5
* more interfaces to inspect during debugging

---

### Context-Dependent Consequences

The current logical separation is sufficient for a supervised tabletop proof of concept.

Future requirements could justify stronger separation.

| Future requirement                                | Possible change                                     |
| ------------------------------------------------- | --------------------------------------------------- |
| Stronger control/monitor failure isolation        | Separate monitoring host                            |
| Larger historical dataset                         | External database                                   |
| Multiple machines                                 | Central monitoring infrastructure                   |
| Higher monitoring rate                            | Dedicated acquisition path                          |
| Real-time protection from electrical measurements | Explicit protection/control layer                   |
| Remote multi-user dashboards                      | Networked visualization service                     |
| Production cybersecurity                          | Authentication, authorization, network segmentation |
| High availability                                 | Redundant monitoring and storage                    |

These changes would not necessarily require moving device-local control into the monitoring system.

---

## Validation Through Current Implementation

The current implementation demonstrates the selected separation through:

* process-specific orchestration scripts
* distributed Pico firmware
* standalone I2C Debugger monitoring
* node-state interpretation
* transition detection
* periodic snapshots
* monitoring sessions
* power-monitor acquisition
* SQLite persistence
* separate Grafana dashboards
* real and mock monitoring modes

Grafana reads persisted records rather than communicating directly with the physical nodes.

This demonstrates the intended control / observation / persistence / visualization boundary for the current proof-of-concept system.

---

## Decision Boundary

This decision establishes:

* logical separation between process control and monitoring
* I2C Debugger as the live observation layer
* SQLite as the current persistence boundary
* Grafana as the historical visualization layer
* monitoring-level communication-failure interpretation
* monitoring sessions and historical records
* monitoring as non-authoritative evidence of exposed system state
* separation of monitoring thresholds from safety protection

This decision does not establish:

* physical isolation between control and monitoring hosts
* hard real-time monitoring
* safety-rated monitoring
* automatic shutdown from Grafana
* automatic shutdown from every monitoring threshold
* proof of every physical process outcome
* production-scale database infrastructure
* production cybersecurity architecture
* high-availability monitoring

Component selection and reproducibility are addressed in the next architectural decision.

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
* [ADL-007: Separation of Responsibilities](./ADL-007%20Separation%20of%20Responsibilities.md)
* [ADL-008: Reconfigurable Physical Mechanisms](./ADL-008%20Reconfigurable%20Physical%20Mechanisms.md)
* [I2C Debugger](../Software/I2C_Debugger/)
* [Grafana](../Software/Grafana/)
* [Monitoring Data Flow](../Docs/System_Architecture/Monitoring_Data_Flow.md)
* [System Architecture](../Docs/System_Architecture/)
