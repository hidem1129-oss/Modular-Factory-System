# ADL-005: Communication Interface Selection

## Purpose of This Document

This document explains the selection of I²C as the communication interface between the Raspberry Pi 5 host and the distributed Raspberry Pi Pico device nodes.

This decision follows the previously established architecture defined in:

* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)

ADL-004 establishes that device-local responsibilities are distributed across multiple nodes.

This document determines how those nodes communicate with the host.

The detailed register layout and command semantics are documented separately.

---

## Decision Summary

I²C was selected as the communication interface between the Raspberry Pi 5 host and the distributed Raspberry Pi Pico nodes.

The current implementation uses:

* Raspberry Pi 5 as the I²C host
* Raspberry Pi Pico devices as addressed nodes
* a shared two-wire communication bus
* 100 kHz communication speed
* register-based reads and writes
* node addresses derived from a base address and a four-bit DIP setting

The decision prioritizes:

* low implementation effort
* compact wiring
* direct support on both host and node platforms
* multiple addressed devices on one bus
* sufficient performance for tabletop control
* compatibility with a register-oriented device model

The selected interface is intended for short-distance communication inside the tabletop prototype.

It is not intended to represent a production factory communication network.

---

## Decision Context

The distributed-node architecture requires a communication path between:

```text
Raspberry Pi 5 host
        ↓
Communication interface
        ↓
Multiple Raspberry Pi Pico nodes
        ↓
Local actuator and sensor behavior
```

The communication interface must support several different device categories while preserving the responsibility boundary established in ADL-004.

The host needs to:

* identify individual nodes
* read node status
* write parameters
* issue commands
* read feedback
* inspect nodes during debugging
* coordinate multiple nodes in process order

The communication requirements are modest in data volume.

The physical processes consist mainly of:

* actuator commands
* parameter values
* state information
* sensor values
* feedback values
* identification information

Large continuous data streams are not transferred through the device-node bus.

Camera data is processed directly by the host and does not pass through the distributed-node communication interface.

---

## Communication Requirements

The interface was evaluated against the following requirements.

| ID  | Requirement                                      |
| --- | ------------------------------------------------ |
| I1  | Support multiple device nodes                    |
| I2  | Allow individual node addressing                 |
| I3  | Require limited wiring                           |
| I4  | Fit short-distance tabletop communication        |
| I5  | Be directly usable from Raspberry Pi 5           |
| I6  | Be directly usable from Raspberry Pi Pico        |
| I7  | Support simple command and feedback transactions |
| I8  | Require low learning and integration effort      |
| I9  | Support practical debugging                      |
| I10 | Provide sufficient speed for current processes   |
| I11 | Remain compatible with modular node replacement  |
| I12 | Avoid unnecessary infrastructure                 |

---

## I1: Multiple Device Nodes

The communication interface must support several actuator and sensor nodes from one host.

The architecture should not require a dedicated host communication port for every physical device.

A shared bus allows nodes to be added or removed while retaining the same host-side communication concept.

---

## I2: Individual Node Addressing

The host must distinguish multiple nodes connected to the same communication path.

Each node therefore requires an identifiable communication address.

The current implementation derives the node address from:

* a defined base address
* a four-bit DIP switch setting

This allows physical configuration of multiple nodes without rebuilding the host software for every address assignment.

Address management remains a configuration responsibility.

The architecture does not currently provide automatic address allocation.

---

## I3: Limited Wiring

The tabletop platform contains:

* multiple control boards
* actuators
* sensors
* power wiring
* camera equipment
* mechanical structures

Communication wiring should therefore remain simple.

I²C uses a shared clock and data pair for communication among multiple addressed nodes.

This reduces communication wiring compared with approaches requiring:

* dedicated point-to-point links
* separate chip-select lines
* individual network cables
* additional communication adapters

---

## I4: Tabletop Communication Distance

The communication interface is used only within the physical limits of the tabletop prototype.

The design does not require:

* factory-wide communication
* long cable runs
* communication between buildings
* high-noise industrial routing
* geographically distributed nodes

This significantly changes the communication trade-off.

An interface optimized for long-distance industrial communication may provide useful robustness but also introduce hardware and integration requirements that are unnecessary for the current system.

---

## I5 and I6: Platform Support

The selected interface should be practical on both:

* Raspberry Pi 5
* Raspberry Pi Pico

Using communication hardware already available on both platforms reduces:

* external component count
* software dependencies
* driver complexity
* development effort
* board complexity

I²C is supported directly by both current platforms.

---

## I7: Command and Feedback Transactions

The communication model primarily consists of small transactions.

Typical operations include:

* read identification
* read status
* write parameters
* apply parameters
* start or stop an operation
* read feedback
* inspect completion state

This communication pattern fits naturally with a register-oriented transaction model.

The communication interface does not need to carry high-bandwidth media or large application payloads.

---

## I8: Learning and Integration Effort

Communication technology was evaluated as an architectural cost rather than only as a technical capability.

The interface should minimize effort required for:

* initial setup
* host programming
* embedded implementation
* wiring
* protocol debugging
* node addition
* basic test programs

The objective was to obtain an observable working distributed system quickly enough that system-level architectural questions could be tested.

---

## I9: Debugging

The communication interface must allow practical investigation of node behavior.

Useful debugging operations include:

* scanning for available addresses
* reading identification values
* reading status registers
* writing a known parameter
* reading feedback
* isolating a node and testing it independently

A relatively simple host-controlled transaction model supports these tasks.

---

## I10: Sufficient Communication Speed

The current physical mechanisms operate on human-observable mechanical timescales.

Examples include:

* conveyor movement
* servo gate motion
* clamping
* stamping
* paper feeding
* sensor detection

The communication bus therefore does not require extremely high throughput.

The current implementation uses I²C at 100 kHz.

This is sufficient for the small command, status, and feedback transactions used by the existing tabletop processes.

The system does not claim that 100 kHz is suitable for all future processes.

---

## I11: Node Replacement

Communication configuration should remain local enough that a node can be replaced without redesigning unrelated device control.

The current addressing method allows a replacement node to be configured for the expected address.

The host can then continue interacting with that logical position through the same communication model.

Replacement still requires compatible:

* firmware
* node type
* register behavior
* electrical interface
* address configuration

The system does not currently provide transparent automatic replacement.

---

## I12: Limited Infrastructure

The communication interface should not require infrastructure disproportionate to the current tabletop system.

The current platform does not require:

* network switches
* industrial gateways
* protocol servers
* dedicated fieldbus interfaces
* managed network configuration
* additional communication processors

The communication interface should remain part of the prototype rather than becoming a separate infrastructure project.

---

## Current Communication Architecture

The current implementation follows this model:

```text
Raspberry Pi 5
      │
      ├── Power monitoring
      │       ↓
      │   Power Monitor Board
      │       ↓
      │   Voltage / current / power measurements
      │
      └── I²C device communication
              │
              ├── Pico node
              │     └── local device
              │
              ├── Pico node
              │     └── local device
              │
              ├── Pico node
              │     └── local device
              │
              └── Pico node
                    └── local device
```

The Raspberry Pi 5 initiates communication.

Each Pico node exposes its host-facing state through the common firmware interface.

The host communicates with a selected node using its configured address.

The communication transport provides access to the node interface.

It does not define the meaning of every register.

That responsibility belongs to the common register specification.

The Raspberry Pi 5 also observes the power-monitoring subsystem.

The Power Monitor Board is not a Raspberry Pi Pico-based device node and does not represent the same device-control responsibility boundary.

It is shown here because power-related measurements are acquired by the host alongside the distributed-node communication path.

The primary communication decision described in this document remains the I²C interface between the Raspberry Pi 5 and the distributed Pico nodes.

---

## Communication and Responsibility Boundary

The selected communication interface should transport information without taking ownership of process behavior.

| Responsibility                 | Owner                               |
| ------------------------------ | ----------------------------------- |
| Decide process sequence        | Host orchestration                  |
| Select target node             | Host                                |
| Transfer command or data       | Communication interface             |
| Validate common command state  | Node common firmware                |
| Execute device-specific action | Node-specific firmware              |
| Produce device feedback        | Node firmware                       |
| Poll exposed information       | Host-side software                  |
| Interpret historical records   | Monitoring and visualization layers |

This distinction is important because changing the communication transport should not automatically redefine device or process responsibilities.

---

## Risks and Mitigations

| Risk                                              | Current mitigation                                                                          |
| ------------------------------------------------- | ------------------------------------------------------------------------------------------- |
| Address conflict                                  | Address configured using base address and four-bit DIP setting                              |
| Communication failure affects multiple nodes      | Node and bus behavior can be inspected separately from physical results                     |
| Shared bus creates a common dependency            | Communication scope remains limited to the tabletop PoC                                     |
| Wiring or connection error                        | Short and accessible tabletop wiring                                                        |
| Bus performance becomes insufficient              | Current workload remains low-bandwidth and 100 kHz is sufficient for demonstrated processes |
| One problematic node affects shared communication | Nodes can be disconnected and tested individually                                           |
| Host depends on I²C-specific access               | Host/node responsibility boundaries are documented separately from transport semantics      |
| Future process requires longer distance           | Communication transport can be reconsidered if system scale changes                         |
| Electrical noise reduces reliability              | Short wiring and controlled tabletop operating environment                                  |

---

## Comparison Summary

| Alternative            | Main strengths                                                                               | Main limitations                                                          | Fit for current PoC |
| ---------------------- | -------------------------------------------------------------------------------------------- | ------------------------------------------------------------------------- | :-----------------: |
| I²C                    | Shared addressed bus, low wiring count, direct Pi/Pico support, simple register transactions | Short-distance assumptions and shared-bus dependency                      |        Strong       |
| SPI                    | High throughput and simple synchronous transfers                                             | Additional chip-select wiring and stronger host/device physical coupling  |       Moderate      |
| UART / RS-485          | Simple serial communication; RS-485 can support longer and more robust links                 | Multi-node addressing and application framing require additional design   |       Moderate      |
| CAN                    | Robust multi-node communication with arbitration and error handling                          | Additional hardware and integration effort beyond current requirements    |       Moderate      |
| Ethernet               | High bandwidth, scalable networking, broad tool support                                      | Greater hardware, software, and network infrastructure                    |         Weak        |
| Wireless communication | Minimal communication cabling and flexible physical placement                                | Additional configuration, reliability, security, and network dependencies |         Weak        |

The comparison describes suitability for the current tabletop proof of concept.

It is not a general ranking of communication technologies.

---

## Considered Alternatives

### Alternative A: I²C

I²C provides an addressed shared bus using clock and data lines.

#### Advantages

* multiple devices can share one communication bus
* low communication wiring count
* direct support on Raspberry Pi 5
* direct support on Raspberry Pi Pico
* natural fit for register-oriented access
* simple host-initiated communication model
* practical address scanning and debugging
* sufficient throughput for current command and feedback traffic
* low additional hardware requirement

#### Disadvantages

* intended for relatively short communication distances
* shared bus can become a common failure domain
* address conflicts must be managed
* bus capacitance and wiring quality become more important as the bus grows
* communication bandwidth is shared across nodes
* electrical robustness is lower than communication systems designed for industrial networks

#### Evaluation

I²C provides sufficient performance while minimizing implementation and wiring complexity.

Its limitations are acceptable within the controlled tabletop environment.

It was selected.

---

### Alternative B: SPI

SPI could provide synchronous communication with higher data throughput.

#### Advantages

* high communication speed
* simple hardware-level transaction model
* full-duplex capability in common implementations
* direct support on Raspberry Pi and microcontroller platforms
* useful for high-bandwidth local peripherals

#### Disadvantages

* multiple devices commonly require additional chip-select signals
* wiring count increases as device count increases
* device selection becomes physically tied to host wiring
* less natural for adding many independently addressed modules
* additional speed provides little benefit for the current mechanical processes

#### Evaluation

SPI would be useful if high local data throughput were the primary requirement.

The current system instead prioritizes:

* multiple nodes
* simple shared wiring
* module rearrangement
* small command and feedback transactions

SPI was therefore not selected.

---

### Alternative C: UART or RS-485

UART provides a simple asynchronous serial interface.

A differential physical layer such as RS-485 could extend the concept to longer and electrically more robust communication.

#### Advantages

* conceptually simple serial communication
* broad microcontroller support
* easy point-to-point debugging
* RS-485 can support longer cable distances
* RS-485 provides improved electrical noise tolerance compared with single-ended local interfaces
* practical for multi-drop communication when an appropriate protocol is implemented

#### Disadvantages

* basic UART does not itself define multi-node addressing
* shared multi-node communication requires additional protocol design
* message framing must be defined
* collision or access behavior must be considered for multi-node operation
* RS-485 requires additional transceiver hardware
* the additional robustness is not currently required by the tabletop system

#### Evaluation

UART would be attractive for simple point-to-point communication.

RS-485 would become more attractive if communication distance or electrical noise became a significant requirement.

For the current short-distance multi-node prototype, the additional framing and physical-layer design were not necessary.

---

### Alternative D: CAN

CAN provides a multi-node bus designed for reliable communication among distributed controllers.

#### Advantages

* multi-node communication
* message arbitration
* error detection
* robust differential signaling
* strong suitability for distributed embedded control
* established use in automotive and industrial systems
* better physical robustness than the current I²C implementation

#### Disadvantages

* additional communication hardware may be required on the selected platforms
* software and protocol setup is more involved
* message-based communication differs from the current simple register-oriented access model
* introduces capability beyond the current tabletop communication requirement
* increases development and debugging effort

#### Evaluation

CAN is technically attractive for a more robust distributed control system.

It may become a stronger candidate if future requirements include:

* longer wiring
* electrically noisy environments
* larger node counts
* stronger communication fault handling
* distributed message arbitration

Those requirements are not currently central to the tabletop proof of concept.

CAN was therefore not selected.

---

### Alternative E: Ethernet

Ethernet could provide network-based communication between host and device nodes.

#### Advantages

* high bandwidth
* long practical communication distance compared with board-level buses
* standard networking tools
* scalable network topology
* compatibility with higher-level network protocols
* easy integration with other computers and infrastructure

#### Disadvantages

* additional hardware is required for small microcontroller nodes unless networking is already available
* greater physical connector and cabling size
* network configuration becomes an additional responsibility
* protocol and software stacks are more complex
* infrastructure such as switches may become necessary
* capabilities substantially exceed the current device-control data requirements

#### Evaluation

Ethernet would be relevant if the system were expanded toward:

* multiple physical stations
* larger distances
* integration with plant networks
* remote systems
* higher-bandwidth distributed processing

Those conditions are outside the current tabletop scope.

Ethernet was not selected for communication between the current local device nodes.

---

### Alternative F: Wireless Communication

Device nodes could communicate using Wi-Fi, Bluetooth, or another wireless technology.

#### Advantages

* reduced communication cabling
* flexible physical placement
* potential remote communication
* easier connection across mechanically separated areas

#### Disadvantages

* additional connection and configuration state
* variable latency and availability
* radio interference
* security considerations
* dependency on wireless stacks or network infrastructure
* more difficult fault isolation
* little benefit when all modules are located on the same tabletop

#### Evaluation

Wireless communication would reduce communication wiring but would introduce new configuration and reliability concerns.

Because all current nodes are physically close together and already require power wiring, eliminating only the communication pair provides limited architectural benefit.

Wireless communication was therefore not selected.

---

## Selected Interface

I²C was selected because it provides the strongest overall fit for:

* short-distance tabletop communication
* multiple addressed nodes
* low wiring count
* direct Raspberry Pi 5 support
* direct Raspberry Pi Pico support
* register-based access
* simple debugging
* low development effort
* low additional hardware cost
* sufficient communication performance

The decision is based on fitness for the current physical scale rather than on maximum communication capability.

A more robust or higher-bandwidth interface would not materially improve the current demonstrations enough to justify its additional development and infrastructure cost.

---

## Consequences

### Positive Consequences

The selected interface provides:

* one shared host-to-node communication bus
* simple addition of addressed nodes
* low communication wiring count
* direct integration with current controllers
* simple register reads and writes
* straightforward host-side polling
* low hardware overhead
* compact tabletop implementation
* sufficient throughput for current processes

---

### Negative Consequences

The selection also introduces:

* a shared communication failure domain
* address-management responsibility
* limitations on practical cable length
* sensitivity to wiring quality and bus loading
* shared bandwidth among nodes
* lower electrical robustness than industrial field communication systems
* dependence on a host-controlled polling model

---

### Context-Dependent Consequences

I²C is appropriate because of the current physical and operational environment.

If the same architecture were deployed under different conditions, the communication decision might change.

For example:

| Changed requirement                   | More relevant candidate  |
| ------------------------------------- | ------------------------ |
| Longer cable distance                 | RS-485, CAN, or Ethernet |
| Higher electrical noise               | CAN or RS-485            |
| Much higher local bandwidth           | SPI or Ethernet          |
| Large distributed network             | Ethernet                 |
| Strong distributed arbitration        | CAN                      |
| Physically separated wireless devices | Wireless communication   |

The distributed-node responsibility model does not require I²C permanently.

A future communication change can therefore be evaluated separately from the basic decision to distribute device-local responsibilities.

---

## Validation Through Current Implementation

The current implementation demonstrates that I²C is sufficient for the present tabletop system.

Current evidence includes:

* communication between Raspberry Pi 5 and multiple Pico nodes
* register-based node access
* configurable node addressing
* motor-node communication
* servo-node communication
* sensor-node communication
* host-side state inspection
* process orchestration across multiple nodes
* operation at 100 kHz
* reuse across multiple physical demonstrations

This validates the communication interface for the current proof-of-concept workload.

It does not validate I²C for:

* production machinery
* long-distance communication
* harsh electrical environments
* safety communication
* hard real-time distributed control
* large-scale factory networks

---

## Decision Boundary

This decision establishes:

* I²C as the current host-to-node communication transport
* Raspberry Pi 5 as the initiating host
* multiple addressed Raspberry Pi Pico nodes
* short-distance tabletop communication
* 100 kHz as the current operating bus speed
* configurable node addressing

This decision does not establish:

* the semantic meaning of individual registers
* the complete common register layout
* process orchestration behavior
* production communication requirements
* automatic node discovery
* automatic address allocation
* Plug & Play behavior
* industrial network integration

The common register interface is addressed in the next architectural decision.

---

## Related Documents

* [Architecture Decision Log overview](./README.md)
* [ADL-000: Requirements Definition and Success Conditions](./ADL-000%20Requirements%20Definition%20and%20Success%20Conditions.md)
* [ADL-001: System Context and Goals](./ADL-001%20System%20Context%20and%20Goals.md)
* [ADL-002: Design and Development Constraints](./ADL-002%20Design%20and%20Development%20Constraints.md)
* [ADL-003: Host Platform Selection](./ADL-003%20Host%20Platform%20Selection.md)
* [ADL-004: Distributed Node Architecture](./ADL-004%20Distributed%20Node%20Architecture.md)
* [Firmware Overview](../Firmware/)
* [System Architecture](../Docs/System_Architecture/)
* [Register Map](../Docs/Register_Map/)
* [Software](../Software/)
